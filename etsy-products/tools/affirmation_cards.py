#!/usr/bin/env python3
"""Generate the affirmation card deck: 36 cards at 2.5x3.5in, 9 per page,
with cover page and decorative card backs. US Letter + A4 PDFs.
"""
import io
import math
import os

import cairosvg
from pypdf import PdfWriter, PdfReader

HERE = os.path.dirname(os.path.abspath(__file__))
PRODUCT = os.path.normpath(os.path.join(HERE, "..", "03-affirmation-cards"))

CREAM = "#F6F1E7"
INK = "#4A4238"
SOFT = "#8A8070"
SAGE = "#8C9A78"
DEEP_SAGE = "#6F7D5C"
TERRACOTTA = "#C08552"
SAND = "#CBB491"
SERIF = "Liberation Serif"
SANS = "Liberation Sans"

SIZES = {
    "us-letter": (816, 1056, "8.5in", "11in"),
    "a4": (794, 1123, "210mm", "297mm"),
}

# 96 units = 1in
CW, CH = 240, 336  # 2.5 x 3.5 in

AFFIRMATIONS = [
    "I am exactly where I need to be today.",
    "I choose progress over perfection.",
    "My pace is the right pace.",
    "I am allowed to take up space.",
    "Rest is productive too.",
    "I can do hard things.",
    "Today, I choose calm over chaos.",
    "I release what I cannot control.",
    "Small steps still move me forward.",
    "I am worthy of good things.",
    "My feelings are valid and welcome.",
    "I speak to myself with kindness.",
    "I trust the timing of my life.",
    "Growth is quiet, and I am growing.",
    "I am more than my productivity.",
    "It is safe for me to slow down.",
    "I attract what I make room for.",
    "Every breath resets my day.",
    "I am building a life I love.",
    "Asking for help is a strength.",
    "I forgive myself for yesterday.",
    "My best today is enough.",
    "I am the author of this chapter.",
    "Joy is allowed, even now.",
    "I honor what my body needs.",
    "Boundaries are how I love myself.",
    "I make space for what matters.",
    "I am becoming, not behind.",
    "Peace begins with my next thought.",
    "I celebrate my quiet wins.",
    "I choose gratitude on purpose.",
    "My presence is a gift.",
    "I let good things be easy.",
    "I water what I want to grow.",
    "Today has enough room for me.",
    "I begin again, gently.",
]


def esc(s):
    return s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def wrap(s, width=17):
    words, lines, cur = s.split(), [], ""
    for w in words:
        if len(cur) + len(w) + 1 > width and cur:
            lines.append(cur)
            cur = w
        else:
            cur = f"{cur} {w}".strip()
    if cur:
        lines.append(cur)
    return lines


def motif_sprig(color=SAGE):
    """Tiny eucalyptus branch."""
    out = [f"<path d='M 0 22 C -3 8 1 -6 3 -22' fill='none' stroke='{INK}' stroke-width='1.6'/>"]
    for i in range(5):
        t = i / 4.0
        x = -3 + 6 * t
        y = 18 - 36 * t
        side = 1 if i % 2 == 0 else -1
        out.append(f"<circle cx='{x + side * 8:.1f}' cy='{y - 2:.1f}' r='{5.5 - 2.2 * t:.1f}' fill='{color}'/>")
    return "".join(out)


def motif_sun(color=TERRACOTTA):
    out = [f"<circle cx='0' cy='0' r='11' fill='{color}'/>"]
    for i in range(8):
        a = math.pi * 2 * i / 8
        x1, y1 = math.cos(a) * 16, math.sin(a) * 16
        x2, y2 = math.cos(a) * 22, math.sin(a) * 22
        out.append(f"<line x1='{x1:.1f}' y1='{y1:.1f}' x2='{x2:.1f}' y2='{y2:.1f}' "
                   f"stroke='{color}' stroke-width='2' stroke-linecap='round'/>")
    return "".join(out)


def motif_moon(color=SAND):
    return (f"<path d='M 6 -18 A 19 19 0 1 0 6 18 A 15 15 0 1 1 6 -18 Z' fill='{color}'/>"
            f"<circle cx='13' cy='-2' r='2.2' fill='{INK}' fill-opacity='0.55'/>")


MOTIFS = [motif_sprig, motif_sun, motif_moon]
MOTIF_COLORS = [SAGE, TERRACOTTA, SAND]


def card(x, y, idx, text_s):
    m = MOTIFS[idx % 3]
    out = [f"<g transform='translate({x} {y})'>"]
    out.append(f"<rect width='{CW}' height='{CH}' fill='{CREAM}'/>")
    out.append(f"<rect x='10' y='10' width='{CW - 20}' height='{CH - 20}' fill='none' "
               f"stroke='{INK}' stroke-width='1.4'/>")
    out.append(f"<rect x='14' y='14' width='{CW - 28}' height='{CH - 28}' fill='none' "
               f"stroke='{INK}' stroke-opacity='0.35' stroke-width='0.8'/>")
    out.append(f"<g transform='translate({CW / 2} 72)'>{m()}</g>")
    lines = wrap(text_s)
    lh = 26
    y0 = CH / 2 + 14 - (len(lines) - 1) * lh / 2
    for i, ln in enumerate(lines):
        out.append(f"<text x='{CW / 2}' y='{y0 + i * lh:.1f}' font-family='{SERIF}' "
                   f"font-style='italic' font-size='19' fill='{INK}' "
                   f"text-anchor='middle'>{esc(ln)}</text>")
    out.append(f"<line x1='{CW / 2 - 22}' y1='{CH - 52}' x2='{CW / 2 + 22}' y2='{CH - 52}' "
               f"stroke='{MOTIF_COLORS[idx % 3]}' stroke-width='2'/>")
    out.append(f"<text x='{CW / 2}' y='{CH - 30}' font-family='{SANS}' font-size='8' "
               f"letter-spacing='2' fill='{SOFT}' text-anchor='middle'>DAILY AFFIRMATION · {idx + 1:02d}</text>")
    out.append("</g>")
    return "".join(out)


def card_back(x, y):
    out = [f"<g transform='translate({x} {y})'>"]
    out.append(f"<rect width='{CW}' height='{CH}' fill='{DEEP_SAGE}'/>")
    out.append(f"<rect x='10' y='10' width='{CW - 20}' height='{CH - 20}' fill='none' "
               f"stroke='{CREAM}' stroke-opacity='0.8' stroke-width='1.2'/>")
    # scattered sprig pattern
    for r in range(4):
        for c in range(3):
            px = 46 + c * 74
            py = 58 + r * 76 + (18 if c % 2 else 0)
            rot = (r * 3 + c) * 40
            out.append(f"<g transform='translate({px} {py}) rotate({rot}) scale(0.6)' opacity='0.75'>"
                       f"{motif_sprig(color=CREAM)}</g>")
    out.append(f"<circle cx='{CW / 2}' cy='{CH / 2}' r='34' fill='{DEEP_SAGE}'/>")
    out.append(f"<circle cx='{CW / 2}' cy='{CH / 2}' r='33' fill='none' stroke='{CREAM}' stroke-width='1'/>")
    out.append(f"<text x='{CW / 2}' y='{CH / 2 + 5}' font-family='{SERIF}' font-style='italic' "
               f"font-size='14' fill='{CREAM}' text-anchor='middle'>breathe</text>")
    out.append("</g>")
    return "".join(out)


def cut_marks(W, H, ox, oy, cols, rows, gx, gy):
    out = []
    for c in range(cols + 1):
        x = ox + c * (CW + gx) - (gx if c else 0) + (0 if c == 0 else -gx / 2 if c < cols else 0)
    # simpler: marks at each grid line
    xs = [ox + c * (CW + gx) - gx / 2 for c in range(1, cols)] + [ox, ox + cols * CW + (cols - 1) * gx]
    ys = [oy + r * (CH + gy) - gy / 2 for r in range(1, rows)] + [oy, oy + rows * CH + (rows - 1) * gy]
    for x in xs:
        out.append(f"<line x1='{x:.1f}' y1='{oy - 18}' x2='{x:.1f}' y2='{oy - 6}' stroke='{SOFT}' stroke-width='1'/>")
        out.append(f"<line x1='{x:.1f}' y1='{oy + rows * CH + (rows - 1) * gy + 6}' "
                   f"x2='{x:.1f}' y2='{oy + rows * CH + (rows - 1) * gy + 18}' stroke='{SOFT}' stroke-width='1'/>")
    for y in ys:
        out.append(f"<line x1='{ox - 18}' y1='{y:.1f}' x2='{ox - 6}' y2='{y:.1f}' stroke='{SOFT}' stroke-width='1'/>")
        out.append(f"<line x1='{ox + cols * CW + (cols - 1) * gx + 6}' y1='{y:.1f}' "
                   f"x2='{ox + cols * CW + (cols - 1) * gx + 18}' y2='{y:.1f}' stroke='{SOFT}' stroke-width='1'/>")
    return out


def grid_page(W, H, render_cell):
    cols, rows, gx, gy = 3, 3, 12, 12
    gw = cols * CW + (cols - 1) * gx
    gh = rows * CH + (rows - 1) * gy
    ox, oy = (W - gw) / 2, (H - gh) / 2
    out = []
    for i in range(rows * cols):
        x = ox + (i % cols) * (CW + gx)
        y = oy + (i // cols) * (CH + gy)
        cell = render_cell(i, x, y)
        if cell:
            out.append(cell)
    out += cut_marks(W, H, ox, oy, cols, rows, gx, gy)
    return out


def p_cover(W, H):
    out = [f"<rect width='{W}' height='{H}' fill='{CREAM}'/>"]
    out.append(f"<rect x='48' y='48' width='{W - 96}' height='{H - 96}' fill='none' stroke='{INK}' stroke-width='1.6'/>")
    out.append(f"<rect x='58' y='58' width='{W - 116}' height='{H - 116}' fill='none' stroke='{INK}' stroke-opacity='0.3' stroke-width='1'/>")
    cy = H * 0.34
    out.append(f"<g transform='translate({W / 2} {cy - 150}) scale(1.5)'>{motif_sun()}</g>")
    out.append(f"<text x='{W / 2}' y='{cy}' font-family='{SERIF}' font-weight='bold' font-size='54' "
               f"letter-spacing='6' fill='{INK}' text-anchor='middle'>DAILY</text>")
    out.append(f"<text x='{W / 2}' y='{cy + 64}' font-family='{SERIF}' font-weight='bold' font-size='54' "
               f"letter-spacing='6' fill='{INK}' text-anchor='middle'>AFFIRMATIONS</text>")
    out.append(f"<line x1='{W / 2 - 130}' y1='{cy + 96}' x2='{W / 2 + 130}' y2='{cy + 96}' stroke='{INK}' stroke-width='1.4'/>")
    out.append(f"<text x='{W / 2}' y='{cy + 128}' font-family='{SANS}' font-size='14' letter-spacing='3' "
               f"fill='{SOFT}' text-anchor='middle'>36 PRINTABLE CARDS · 2.5 x 3.5 IN</text>")
    how = [
        "HOW TO USE",
        "1. Print on cardstock (65 lb or heavier) at 100% scale.",
        "2. Cut along the crop marks — 9 cards per page.",
        "3. Optional: print the back pattern page on the reverse side.",
        "4. Pull one card each morning. Read it out loud. Mean it.",
    ]
    hy = H * 0.62
    for i, ln in enumerate(how):
        if i == 0:
            out.append(f"<text x='{W / 2}' y='{hy}' font-family='{SANS}' font-weight='bold' font-size='13' "
                       f"letter-spacing='3' fill='{DEEP_SAGE}' text-anchor='middle'>{ln}</text>")
        else:
            out.append(f"<text x='{W / 2}' y='{hy + 10 + i * 26}' font-family='{SANS}' font-size='13' "
                       f"fill='{INK}' text-anchor='middle'>{esc(ln)}</text>")
    out.append(f"<g transform='translate({W / 2} {H - 140}) scale(1.2)'>{motif_sprig()}</g>")
    return out


def build_pdf(size_key):
    W, H, win, hin = SIZES[size_key]
    pages = [p_cover(W, H)]
    for p in range(4):
        def cell(i, x, y, p=p):
            idx = p * 9 + i
            return card(x, y, idx, AFFIRMATIONS[idx]) if idx < len(AFFIRMATIONS) else None
        pages.append(grid_page(W, H, cell))
    pages.append(grid_page(W, H, lambda i, x, y: card_back(x, y)))

    writer = PdfWriter()
    for body in pages:
        svg = (f"<svg xmlns='http://www.w3.org/2000/svg' width='{win}' height='{hin}' "
               f"viewBox='0 0 {W} {H}'><rect width='{W}' height='{H}' fill='#FFFFFF'/>"
               + "".join(body) + "</svg>")
        writer.append(PdfReader(io.BytesIO(cairosvg.svg2pdf(bytestring=svg.encode()))))
    os.makedirs(os.path.join(PRODUCT, "print-files"), exist_ok=True)
    out = os.path.join(PRODUCT, "print-files", f"daily-affirmation-cards-{size_key}.pdf")
    with open(out, "wb") as f:
        writer.write(f)
    print("wrote", out)


if __name__ == "__main__":
    for k in SIZES:
        build_pdf(k)
