MusicMob Railway deployment

Start command:
npm start

The Node server reads Railway's PORT environment variable and serves the static site from this directory.

Stripe checkout:
Set STRIPE_SECRET_KEY in Railway to create Checkout Sessions from the order form.
Optional: set STRIPE_PRICE_ID to use a Stripe Dashboard price instead of the built-in $9.99 price_data.

Order details:
The order form posts to /create-checkout-session. Song idea, genre, reference artist, email, and an order id are attached to the Stripe Checkout Session metadata.

Paid order email notifications:
The server exposes /stripe-webhook for Stripe webhook events. Add this endpoint in Stripe:
https://www.musicmob.me/stripe-webhook

Listen for this event:
checkout.session.completed

Set these Railway variables:
STRIPE_WEBHOOK_SECRET=whsec_... from the Stripe webhook endpoint
RESEND_API_KEY=re_... from Resend
ORDER_NOTIFY_EMAIL=j.cooper@musicmob.me
ORDER_NOTIFY_FROM=MusicMob <orders@musicmob.me>

If ORDER_NOTIFY_FROM is not set, the server uses MusicMob <onboarding@resend.dev>. For production, verify musicmob.me in Resend and use an orders@musicmob.me sender.

Canonical domain:
https://www.musicmob.me

Use www.musicmob.me as the Railway custom domain on the current Railway plan. Forward musicmob.me to https://www.musicmob.me at IONOS, but do not include the www subdomain in IONOS forwarding.

Payments:
If STRIPE_SECRET_KEY is missing, the form falls back to the existing hosted Stripe checkout link so local previews still work.
