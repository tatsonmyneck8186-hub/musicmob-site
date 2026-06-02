const http = require("http");
const https = require("https");
const crypto = require("crypto");
const fs = require("fs");
const path = require("path");

const PORT = Number(process.env.PORT || 8080);
const HOST = "0.0.0.0";
const ROOT = __dirname;
const CANONICAL_HOST = "www.musicmob.me";
const HOSTED_CHECKOUT_URL = "https://buy.stripe.com/5kQdR84Zv7mHc4FdbG4wM00";
const DEFAULT_NOTIFY_FROM = "MusicMob <onboarding@resend.dev>";

const MIME = {
  ".html": "text/html; charset=utf-8",
  ".css": "text/css; charset=utf-8",
  ".js": "application/javascript; charset=utf-8",
  ".json": "application/json; charset=utf-8",
  ".png": "image/png",
  ".jpg": "image/jpeg",
  ".jpeg": "image/jpeg",
  ".svg": "image/svg+xml",
  ".ico": "image/x-icon",
  ".mp3": "audio/mpeg"
};

function getOrigin(req) {
  const hostHeader = req.headers["x-forwarded-host"] || req.headers.host || CANONICAL_HOST;
  const host = Array.isArray(hostHeader) ? hostHeader[0] : hostHeader.split(",")[0].trim();
  const protoHeader = req.headers["x-forwarded-proto"];
  const proto = Array.isArray(protoHeader) ? protoHeader[0] : protoHeader || "http";
  const isLocal = host.startsWith("localhost") || host.startsWith("127.0.0.1");
  return `${isLocal ? "http" : proto}://${host}`;
}

function sendText(res, status, text) {
  res.writeHead(status, { "Content-Type": "text/plain; charset=utf-8" });
  res.end(text);
}

function redirect(res, status, location) {
  res.writeHead(status, {
    Location: location,
    "Content-Type": "text/plain; charset=utf-8"
  });
  res.end(`Redirecting to ${location}`);
}

function readRequestBody(req, maxBytes = 64 * 1024) {
  return new Promise((resolve, reject) => {
    let body = "";
    req.on("data", (chunk) => {
      body += chunk;
      if (Buffer.byteLength(body) > maxBytes) {
        reject(new Error("Request body too large"));
        req.destroy();
      }
    });
    req.on("end", () => resolve(body));
    req.on("error", reject);
  });
}

function truncate(value, max = 480) {
  const text = String(value || "").trim();
  return text.length > max ? `${text.slice(0, max - 3)}...` : text;
}

function escapeHtml(value) {
  return String(value || "")
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;")
    .replace(/'/g, "&#39;");
}

function formatDollars(cents) {
  const amount = Number(cents || 0) / 100;
  return `$${amount.toFixed(2)}`;
}

function createStripeSession(payload, secretKey) {
  return new Promise((resolve, reject) => {
    const data = new URLSearchParams(payload).toString();
    const request = https.request(
      {
        hostname: "api.stripe.com",
        path: "/v1/checkout/sessions",
        method: "POST",
        headers: {
          Authorization: `Bearer ${secretKey}`,
          "Content-Type": "application/x-www-form-urlencoded",
          "Content-Length": Buffer.byteLength(data)
        }
      },
      (response) => {
        let responseBody = "";
        response.on("data", (chunk) => {
          responseBody += chunk;
        });
        response.on("end", () => {
          let json;
          try {
            json = JSON.parse(responseBody);
          } catch (error) {
            reject(new Error("Stripe returned an unreadable response"));
            return;
          }
          if (response.statusCode < 200 || response.statusCode >= 300) {
            reject(new Error(json.error?.message || "Stripe checkout could not be created"));
            return;
          }
          resolve(json);
        });
      }
    );
    request.on("error", reject);
    request.write(data);
    request.end();
  });
}

function verifyStripeWebhookSignature(payload, signatureHeader, webhookSecret) {
  if (!signatureHeader || !webhookSecret) return false;

  const parts = Object.fromEntries(
    signatureHeader.split(",").map((part) => {
      const [key, value] = part.split("=");
      return [key, value];
    })
  );
  const timestamp = parts.t;
  const expected = parts.v1;
  if (!timestamp || !expected) return false;

  const signedPayload = `${timestamp}.${payload}`;
  const actual = crypto.createHmac("sha256", webhookSecret).update(signedPayload).digest("hex");
  const actualBuffer = Buffer.from(actual, "hex");
  const expectedBuffer = Buffer.from(expected, "hex");
  return actualBuffer.length === expectedBuffer.length && crypto.timingSafeEqual(actualBuffer, expectedBuffer);
}

function sendEmailWithResend({ to, from, subject, html, text }) {
  return new Promise((resolve, reject) => {
    const apiKey = process.env.RESEND_API_KEY;
    if (!apiKey) {
      resolve({ skipped: true, reason: "RESEND_API_KEY is not set" });
      return;
    }

    const data = JSON.stringify({ from, to, subject, html, text });
    const request = https.request(
      {
        hostname: "api.resend.com",
        path: "/emails",
        method: "POST",
        headers: {
          Authorization: `Bearer ${apiKey}`,
          "Content-Type": "application/json",
          "Content-Length": Buffer.byteLength(data)
        }
      },
      (response) => {
        let responseBody = "";
        response.on("data", (chunk) => {
          responseBody += chunk;
        });
        response.on("end", () => {
          if (response.statusCode < 200 || response.statusCode >= 300) {
            reject(new Error(`Resend email failed: ${responseBody}`));
            return;
          }
          resolve({ skipped: false });
        });
      }
    );
    request.on("error", reject);
    request.write(data);
    request.end();
  });
}

async function notifyPaidOrder(session) {
  const to = process.env.ORDER_NOTIFY_EMAIL || "j.cooper@musicmob.me";
  const from = process.env.ORDER_NOTIFY_FROM || DEFAULT_NOTIFY_FROM;
  const metadata = session.metadata || {};
  const orderId = metadata.order_id || session.client_reference_id || session.id;
  const email = metadata.email || session.customer_email || "not provided";
  const genre = metadata.genre || "not provided";
  const songIdea = metadata.song_idea || "not provided";
  const referenceArtist = metadata.reference_artist || "none";
  const amount = formatDollars(session.amount_total);

  const subject = `Paid MusicMob order: ${orderId}`;
  const text = [
    "A MusicMob order was paid.",
    "",
    `Order ID: ${orderId}`,
    `Customer email: ${email}`,
    `Amount: ${amount}`,
    `Genre: ${genre}`,
    `Reference artist: ${referenceArtist}`,
    "",
    "Song idea:",
    songIdea,
    "",
    `Stripe session: ${session.id}`
  ].join("\n");
  const html = `
    <h2>Paid MusicMob order</h2>
    <p><strong>Order ID:</strong> ${escapeHtml(orderId)}</p>
    <p><strong>Customer email:</strong> ${escapeHtml(email)}</p>
    <p><strong>Amount:</strong> ${escapeHtml(amount)}</p>
    <p><strong>Genre:</strong> ${escapeHtml(genre)}</p>
    <p><strong>Reference artist:</strong> ${escapeHtml(referenceArtist)}</p>
    <p><strong>Song idea:</strong></p>
    <p>${escapeHtml(songIdea).replace(/\n/g, "<br>")}</p>
    <p><strong>Stripe session:</strong> ${escapeHtml(session.id)}</p>
  `;

  return sendEmailWithResend({ to, from, subject, html, text });
}

async function handleStripeWebhook(req, res) {
  if (req.method !== "POST") {
    sendText(res, 405, "Method not allowed");
    return;
  }

  let body;
  try {
    body = await readRequestBody(req, 256 * 1024);
  } catch (error) {
    sendText(res, 413, "Request too large");
    return;
  }

  const webhookSecret = process.env.STRIPE_WEBHOOK_SECRET;
  if (!webhookSecret) {
    sendText(res, 500, "Stripe webhook is not configured");
    return;
  }
  if (!verifyStripeWebhookSignature(body, req.headers["stripe-signature"], webhookSecret)) {
    sendText(res, 400, "Invalid Stripe signature");
    return;
  }

  let event;
  try {
    event = JSON.parse(body);
  } catch (error) {
    sendText(res, 400, "Invalid webhook body");
    return;
  }

  if (event.type === "checkout.session.completed") {
    try {
      const result = await notifyPaidOrder(event.data.object);
      if (result.skipped) {
        console.log(`Order email skipped: ${result.reason}`);
      }
    } catch (error) {
      console.error(error);
      sendText(res, 500, "Order notification failed");
      return;
    }
  }

  sendText(res, 200, "ok");
}

async function handleCreateCheckoutSession(req, res) {
  if (req.method !== "POST") {
    sendText(res, 405, "Method not allowed");
    return;
  }

  let params;
  try {
    const body = await readRequestBody(req);
    params = new URLSearchParams(body);
  } catch (error) {
    sendText(res, 413, "Request too large");
    return;
  }

  const email = truncate(params.get("email"), 250);
  const songIdea = truncate(params.get("song_idea"));
  const genre = truncate(params.get("genre"), 100);
  const referenceArtist = truncate(params.get("reference_artist"), 120);

  if (!email || !songIdea || !genre) {
    sendText(res, 400, "Please complete the song idea, genre, and email fields.");
    return;
  }

  const secretKey = process.env.STRIPE_SECRET_KEY;
  if (!secretKey) {
    redirect(res, 303, HOSTED_CHECKOUT_URL);
    return;
  }

  const origin = getOrigin(req);
  const orderId = `musicmob_${Date.now()}`;
  const stripePayload = {
    mode: "payment",
    success_url: `${origin}/order-success.html?session_id={CHECKOUT_SESSION_ID}`,
    cancel_url: `${origin}/order-cancel.html#order`,
    customer_email: email,
    client_reference_id: orderId,
    "metadata[order_id]": orderId,
    "metadata[source]": "www.musicmob.me",
    "metadata[email]": email,
    "metadata[genre]": genre,
    "metadata[song_idea]": songIdea,
    "metadata[reference_artist]": referenceArtist || "none"
  };

  if (process.env.STRIPE_PRICE_ID) {
    stripePayload["line_items[0][price]"] = process.env.STRIPE_PRICE_ID;
    stripePayload["line_items[0][quantity]"] = "1";
  } else {
    stripePayload["line_items[0][price_data][currency]"] = "usd";
    stripePayload["line_items[0][price_data][product_data][name]"] = "MusicMob Custom Song";
    stripePayload["line_items[0][price_data][product_data][description]"] =
      "One custom song concept delivered as an MP3.";
    stripePayload["line_items[0][price_data][unit_amount]"] = "999";
    stripePayload["line_items[0][quantity]"] = "1";
  }

  try {
    const session = await createStripeSession(stripePayload, secretKey);
    redirect(res, 303, session.url);
  } catch (error) {
    console.error(error);
    sendText(res, 502, "Stripe checkout is temporarily unavailable. Please email j.cooper@musicmob.me.");
  }
}

function serveStatic(req, res) {
  const reqPath = decodeURIComponent((req.url || "/").split("?")[0]);
  const target = reqPath === "/" ? "/index.html" : reqPath;
  const filePath = path.resolve(ROOT, `.${target}`);

  if (!filePath.startsWith(`${ROOT}${path.sep}`) && filePath !== ROOT) {
    sendText(res, 403, "Forbidden");
    return;
  }

  fs.readFile(filePath, (err, data) => {
    if (err) {
      sendText(res, 404, "Not found");
      return;
    }
    const ext = path.extname(filePath).toLowerCase();
    const headers = {
      "Content-Type": MIME[ext] || "application/octet-stream",
      "X-Frame-Options": "DENY",
      "X-Content-Type-Options": "nosniff",
      "Referrer-Policy": "strict-origin-when-cross-origin"
    };
    if (filePath.includes(`${path.sep}assets${path.sep}`)) {
      headers["Cache-Control"] = "public, max-age=31536000, immutable";
    }
    res.writeHead(200, headers);
    res.end(data);
  });
}

http
  .createServer((req, res) => {
    const reqPath = (req.url || "/").split("?")[0];
    if (reqPath === "/create-checkout-session") {
      handleCreateCheckoutSession(req, res);
      return;
    }
    if (reqPath === "/stripe-webhook") {
      handleStripeWebhook(req, res);
      return;
    }

    serveStatic(req, res);
  })
  .listen(PORT, HOST, () => {
    console.log(`MusicMob running on http://${HOST}:${PORT}`);
  });
