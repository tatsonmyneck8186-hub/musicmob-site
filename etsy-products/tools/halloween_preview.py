#!/usr/bin/env python3
"""Compose the Etsy listing hero image for the retro Halloween pack."""
import os
import sys

import cairosvg

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import halloween_pack as hp  # noqa: E402

PRODUCT = hp.PRODUCT
W, H = 2000, 1500
INK = "#3A2A20"
SERIF = "Liberation Serif"
SANS = "Liberation Sans"


def main():
    out = [f"<rect width='{W}' height='{H}' fill='#EFE2C8'/>"]
    out.append(f"<text x='{W / 2}' y='108' font-family='{SERIF}' font-weight='bold' font-size='62' "
               f"letter-spacing='4' fill='{hp.RUST}' text-anchor='middle'>RETRO HALLOWEEN SVG + PNG PACK</text>")
    out.append(f"<line x1='{W / 2 - 220}' y1='142' x2='{W / 2 + 220}' y2='142' stroke='{INK}' stroke-width='2'/>")
    out.append(f"<text x='{W / 2}' y='185' font-family='{SANS}' font-size='25' letter-spacing='5' "
               f"fill='{INK}' text-anchor='middle'>15 DESIGNS · TRANSPARENT PNG · CUT-READY SVG · "
               f"COMMERCIAL USE</text>")
    tw = 356
    th = tw * hp.H / hp.W
    gap = 26
    cols = 5
    total = cols * tw + (cols - 1) * gap
    scale = tw / hp.W
    for i, (name, fn) in enumerate(hp.DESIGNS):
        x = (W - total) / 2 + (i % cols) * (tw + gap)
        y = 235 + (i // cols) * (th + gap)
        tile = "#F7EEDC" if (i % 2 == 0) else "#4A4238"
        out.append(f"<rect x='{x}' y='{y}' width='{tw}' height='{th:.0f}' rx='10' fill='{tile}'/>")
        out.append(f"<g transform='translate({x} {y}) scale({scale})'>" + "".join(fn()) + "</g>")
    svg = f"<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 {W} {H}'>{''.join(out)}</svg>"
    dest = os.path.join(PRODUCT, "listing-preview.png")
    cairosvg.svg2png(bytestring=svg.encode(), write_to=dest, output_width=W)
    print("wrote", dest)


if __name__ == "__main__":
    main()
