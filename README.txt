MusicMob Railway deployment

Start command:
npm start

The Node server reads Railway's PORT environment variable and serves the static site from this directory.

Stripe checkout:
Set STRIPE_SECRET_KEY in Railway to create Checkout Sessions from the order form.
Optional: set STRIPE_PRICE_ID to use a Stripe Dashboard price instead of the built-in $9.99 price_data.

Order details:
The order form posts to /create-checkout-session. Song idea, genre, reference artist, email, and an order id are attached to the Stripe Checkout Session metadata.

Canonical domain:
https://musicmob.me

The server redirects www.musicmob.me requests to musicmob.me.

Payments:
If STRIPE_SECRET_KEY is missing, the form falls back to the existing hosted Stripe checkout link so local previews still work.
