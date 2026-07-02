# Etsy Digital Products — Ready to List

Three complete digital products based on what's currently selling well on Etsy
(printable wall art, budget planners, and affirmation/self-care printables are
among the steadiest, least seasonal digital download categories in 2026).

**These products are unrelated to the MusicMob site** — they live in this
folder only so the files are stored somewhere. Nothing in the site references
them.

## The products

| # | Product | Files | Suggested price |
|---|---------|-------|-----------------|
| 1 | Neutral Botanical Wall Art (set of 6) | 18 PNGs, 300 DPI, 3 ratios | $6.99 |
| 2 | 2026 Budget Planner (10 pages) | US Letter + A4 PDFs | $5.99 |
| 3 | Daily Affirmation Cards (36 cards) | US Letter + A4 PDFs | $4.99 |

Each product folder contains:
- `print-files/` — the actual files buyers download
- `listing.md` — copy-paste title, 13 tags, description, price, and a photo plan
- `listing-preview.png` — a ready-made first listing photo

## How to list on Etsy (per product)

1. Etsy > Shop Manager > Listings > Add a listing
2. Choose **Digital files** as the listing type
3. Upload `listing-preview.png` as photo 1 (add mockups per the photo plan in `listing.md`)
4. Paste the title, description, price and 13 tags from `listing.md`
5. Upload the files from `print-files/`
   - Etsy allows max 5 files per listing at 20 MB each.
   - Wall art: zip each ratio folder (`2x3.zip`, `4x5.zip`, `iso.zip`) and upload the 3 zips.
   - Planners/cards: upload the 2 PDFs directly.
6. Publish ($0.20 listing fee)

## Tips that actually move sales

- Run a launch sale (~30% off) for the first 2 weeks — new listings with early
  sales get pushed harder by Etsy search.
- Renew or add one new listing per week; shop activity is a ranking signal.
- The photo plan in each `listing.md` matters: listings with 8–10 photos
  convert far better than listings with 1–2.
- Answer messages fast — "Star Seller" status compounds visibility.

## Regenerating / customizing

Everything is generated from code in `tools/` (Python, needs
`pip install cairosvg pypdf`):

- `botanicals.py` — wall art set (edit palette or add designs)
- `budget_planner.py` — planner pages (change the year on the cover for 2027)
- `affirmation_cards.py` — card deck (swap affirmation text freely)
- `previews.py` — listing hero images

Run any script directly, e.g. `python3 tools/botanicals.py`.
