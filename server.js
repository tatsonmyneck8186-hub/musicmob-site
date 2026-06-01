const http = require("http");
const https = require("https");
const fs = require("fs");
const path = require("path");

const PORT = Number(process.env.PORT || 8080);
const HOST = "0.0.0.0";
const ROOT = __dirname;
const CANONICAL_HOST = "musicmob.me";
const HOSTED_CHECKOUT_URL = "https://buy.stripe.com/5kQdR84Zv7mHc4FdbG4wM00";

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
    "metadata[source]": "musicmob.me",
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

    serveStatic(req, res);
  })
  .listen(PORT, HOST, () => {
    console.log(`MusicMob running on http://${HOST}:${PORT}`);
  });
