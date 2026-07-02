#!/usr/bin/env python3
"""Compose Etsy listing hero images (2000x1500 PNG) for all three products."""
import os
import sys

import cairosvg

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import botanicals as bt  # noqa: E402
import budget_planner as bp  # noqa: E402
import affirmation_cards as ac  # noqa: E402

ROOT = os.path.normpath(os.path.join(HERE, ".."))
BG = "#EBE1D3"
INK = "#4A4238"
SOFT = "#8A8070"
SERIF = "Liberation Serif"
SANS = "Liberation Sans"
W, H = 2000, 1500


def shadow(x, y, w, h):
    return (f"<rect x='{x + 10}' y='{y + 14}' width='{w}' height='{h}' fill='#3F3A33' "
            f"fill-opacity='0.18' rx='2'/>")


def header(title, sub):
    return (f"<text x='{W / 2}' y='120' font-family='{SERIF}' font-weight='bold' font-size='68' "
            f"letter-spacing='6' fill='{INK}' text-anchor='middle'>{title}</text>"
            f"<line x1='{W / 2 - 200}' y1='158' x2='{W / 2 + 200}' y2='158' stroke='{INK}' stroke-width='2'/>"
            f"<text x='{W / 2}' y='205' font-family='{SANS}' font-size='26' letter-spacing='6' "
            f"fill='{SOFT}' text-anchor='middle'>{sub}</text>")


def wall_art_preview():
    out = [f"<rect width='{W}' height='{H}' fill='{BG}'/>",
           header("NEUTRAL BOTANICAL WALL ART", "SET OF 6  ·  PRINTABLE  ·  INSTANT DOWNLOAD")]
    pw, ph = 380, 570
    gap = 62
    total = 3 * pw + 2 * gap
    scale = pw / 1200.0
    for i, (name, fn) in enumerate(bt.DESIGNS):
        x = (W - total) / 2 + (i % 3) * (pw + gap)
        y = 265 + (i // 3) * (ph + 55)
        out.append(shadow(x, y, pw, ph))
        out.append(f"<g transform='translate({x} {y}) scale({scale})'>"
                   f"<rect width='1200' height='1800' fill='{bt.CREAM}'/>"
                   + "".join(fn(600)) + "</g>")
        out.append(f"<rect x='{x}' y='{y}' width='{pw}' height='{ph}' fill='none' "
                   f"stroke='#FFFFFF' stroke-width='10'/>")
    return "".join(out)


def planner_preview():
    out = [f"<rect width='{W}' height='{H}' fill='{BG}'/>",
           header("2026 BUDGET PLANNER", "10 PAGES  ·  US LETTER + A4  ·  PRINT AT HOME")]
    pages = [bp.p_cover, bp.p_monthly, bp.p_bills, bp.p_savings]
    pw = 420
    LW, LH = bp.SIZES["us-letter"][:2]
    ph = pw * LH / LW
    gap = 55
    total = 4 * pw + 3 * gap
    scale = pw / LW
    for i, fn in enumerate(pages):
        x = (W - total) / 2 + i * (pw + gap)
        y = 330
        out.append(shadow(x, y, pw, ph))
        out.append(f"<g transform='translate({x} {y}) scale({scale})'>"
                   f"<rect width='{LW}' height='{LH}' fill='#FFFFFF'/>"
                   + "".join(fn(LW, LH)) + "</g>")
        out.append(f"<rect x='{x}' y='{y}' width='{pw}' height='{ph:.0f}' fill='none' "
                   f"stroke='{INK}' stroke-opacity='0.25' stroke-width='2'/>")
    out.append(f"<text x='{W / 2}' y='{H - 80}' font-family='{SANS}' font-size='24' letter-spacing='4' "
               f"fill='{SOFT}' text-anchor='middle'>MONTHLY BUDGETS · BILL TRACKER · SAVINGS GOALS · "
               f"DEBT PAYOFF · SINKING FUNDS</text>")
    return "".join(out)


def cards_preview():
    out = [f"<rect width='{W}' height='{H}' fill='{BG}'/>",
           header("DAILY AFFIRMATION CARDS", "36 CARDS  ·  2.5 x 3.5 IN  ·  PRINTABLE DECK")]
    cw = 340
    ch = cw * ac.CH / ac.CW
    scale = cw / ac.CW
    picks = [0, 5, 8, 33]
    gap = 48
    total = 4 * cw + 3 * gap
    for i, idx in enumerate(picks):
        x = (W - total) / 2 + i * (cw + gap)
        y = 330
        rot = (-3, 2, -2, 3)[i]
        out.append(f"<g transform='translate({x + cw / 2} {y + ch / 2}) rotate({rot}) "
                   f"translate({-cw / 2} {-ch / 2})'>")
        out.append(shadow(0, 0, cw, ch))
        out.append(f"<g transform='scale({scale})'>" + ac.card(0, 0, idx, ac.AFFIRMATIONS[idx]) + "</g>")
        out.append("</g>")
    # card back centered below
    x = W / 2 - cw / 2
    y = 330 + ch + 60
    out.append(f"<g transform='translate({x + cw / 2} {y + ch / 2}) rotate(2) translate({-cw / 2} {-ch / 2})'>")
    out.append(shadow(0, 0, cw, ch))
    out.append(f"<g transform='scale({scale})'>" + ac.card_back(0, 0) + "</g></g>")
    out.append(f"<text x='{W / 2 - cw / 2 - 40}' y='{y + ch / 2}' font-family='{SERIF}' font-style='italic' "
               f"font-size='30' fill='{SOFT}' text-anchor='end'>matching card backs</text>")
    out.append(f"<text x='{W / 2 + cw / 2 + 40}' y='{y + ch / 2}' font-family='{SERIF}' font-style='italic' "
               f"font-size='30' fill='{SOFT}' text-anchor='start'>print · cut · breathe</text>")
    return "".join(out)


JOBS = [
    ("01-neutral-botanical-wall-art", wall_art_preview),
    ("02-budget-planner-2026", planner_preview),
    ("03-affirmation-cards", cards_preview),
]

if __name__ == "__main__":
    for folder, fn in JOBS:
        svg = f"<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 {W} {H}'>{fn()}</svg>"
        out = os.path.join(ROOT, folder, "listing-preview.png")
        cairosvg.svg2png(bytestring=svg.encode(), write_to=out, output_width=W)
        print("wrote", out)
