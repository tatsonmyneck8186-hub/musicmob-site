#!/usr/bin/env python3
"""Generate the Neutral Botanical wall art set: 6 designs, 3 print ratios each.

Outputs SVG sources and 300dpi PNG print files. All geometry is parametric so
every ratio recomposes cleanly around the artwork's center line.
"""
import math
import os

import cairosvg

HERE = os.path.dirname(os.path.abspath(__file__))
PRODUCT = os.path.normpath(os.path.join(HERE, "..", "01-neutral-botanical-wall-art"))

CREAM = "#F6F1E7"
INK = "#4A4238"
SAGE = "#8C9A78"
DEEP_SAGE = "#6F7D5C"
TERRACOTTA = "#C08552"
SAND = "#CBB491"

H = 1800.0  # master art height in viewBox units

RATIOS = {
    "2x3": (1200.0, "24x36in-300dpi", 7200),   # covers 4x6 -> 24x36
    "4x5": (1440.0, "16x20in-300dpi", 4800),   # covers 8x10, 16x20
    "iso": (1272.8, "A2-300dpi", 4961),        # covers A5 -> A1
}


def bez(p0, p1, p2, p3, t):
    mt = 1 - t
    x = mt**3 * p0[0] + 3 * mt**2 * t * p1[0] + 3 * mt * t**2 * p2[0] + t**3 * p3[0]
    y = mt**3 * p0[1] + 3 * mt**2 * t * p1[1] + 3 * mt * t**2 * p2[1] + t**3 * p3[1]
    return x, y


def bez_tan(p0, p1, p2, p3, t):
    mt = 1 - t
    dx = 3 * mt**2 * (p1[0] - p0[0]) + 6 * mt * t * (p2[0] - p1[0]) + 3 * t**2 * (p3[0] - p2[0])
    dy = 3 * mt**2 * (p1[1] - p0[1]) + 6 * mt * t * (p2[1] - p1[1]) + 3 * t**2 * (p3[1] - p2[1])
    return math.atan2(dy, dx)


def stem_path(p0, p1, p2, p3, width, color=INK, cap="round"):
    return (f"<path d='M {p0[0]:.1f} {p0[1]:.1f} C {p1[0]:.1f} {p1[1]:.1f} "
            f"{p2[0]:.1f} {p2[1]:.1f} {p3[0]:.1f} {p3[1]:.1f}' fill='none' "
            f"stroke='{color}' stroke-width='{width}' stroke-linecap='{cap}'/>")


def leaf(base, angle, length, width, fill=None, stroke=None, sw=4.0):
    """Lens-shaped leaf from base point along angle."""
    tipx = base[0] + math.cos(angle) * length
    tipy = base[1] + math.sin(angle) * length
    px = math.cos(angle + math.pi / 2)
    py = math.sin(angle + math.pi / 2)
    midx, midy = base[0] + math.cos(angle) * length * 0.45, base[1] + math.sin(angle) * length * 0.45
    c1 = (midx + px * width, midy + py * width)
    c2 = (midx - px * width, midy - py * width)
    d = (f"M {base[0]:.1f} {base[1]:.1f} Q {c1[0]:.1f} {c1[1]:.1f} {tipx:.1f} {tipy:.1f} "
         f"Q {c2[0]:.1f} {c2[1]:.1f} {base[0]:.1f} {base[1]:.1f} Z")
    f = fill or "none"
    s = f"stroke='{stroke}' stroke-width='{sw}' stroke-linejoin='round'" if stroke else "stroke='none'"
    return f"<path d='{d}' fill='{f}' {s}/>"


def jitter(i, scale=1.0):
    return math.sin(i * 12.9898 + 4.1414) * scale


# ---------------------------------------------------------------- designs

def eucalyptus(cx):
    p0, p1, p2, p3 = (cx + 10, 1660), (cx - 70, 1250), (cx + 10, 780), (cx + 40, 250)
    out = [stem_path(p0, p1, p2, p3, 7)]
    n = 12
    for i in range(n):
        t = 0.10 + 0.85 * i / (n - 1)
        x, y = bez(p0, p1, p2, p3, t)
        ang = bez_tan(p0, p1, p2, p3, t)
        side = 1 if i % 2 == 0 else -1
        la = ang + side * (1.15 + 0.12 * jitter(i))
        pet = 26 + 6 * jitter(i + 3)
        lx, ly = x + math.cos(la) * pet, y + math.sin(la) * pet
        r = 62 - 40 * t + 4 * jitter(i + 7)
        out.append(f"<line x1='{x:.1f}' y1='{y:.1f}' x2='{lx:.1f}' y2='{ly:.1f}' "
                   f"stroke='{INK}' stroke-width='5'/>")
        fill = SAGE if i % 3 else DEEP_SAGE
        deg = math.degrees(la)
        out.append(f"<ellipse cx='{lx + math.cos(la) * r * 0.9:.1f}' cy='{ly + math.sin(la) * r * 0.9:.1f}' "
                   f"rx='{r:.1f}' ry='{r * 0.88:.1f}' transform='rotate({deg:.1f} "
                   f"{lx + math.cos(la) * r * 0.9:.1f} {ly + math.sin(la) * r * 0.9:.1f})' "
                   f"fill='{fill}' fill-opacity='0.92'/>")
    # top bud
    x, y = bez(p0, p1, p2, p3, 1.0)
    out.append(f"<circle cx='{x:.1f}' cy='{y:.1f}' r='16' fill='{DEEP_SAGE}'/>")
    return out


def fern(cx):
    p0, p1, p2, p3 = (cx - 70, 1670), (cx - 130, 1150), (cx + 20, 700), (cx + 150, 300)
    out = [stem_path(p0, p1, p2, p3, 6.5)]
    n = 24
    for i in range(n):
        t = 0.06 + 0.90 * i / (n - 1)
        x, y = bez(p0, p1, p2, p3, t)
        ang = bez_tan(p0, p1, p2, p3, t)
        L = 165 * (1 - t) + 42
        for side in (1, -1):
            la = ang + side * (0.95 + 0.10 * jitter(i * side))
            out.append(leaf((x, y), la, L * (1 + 0.06 * jitter(i + side)), L * 0.16,
                            fill="none", stroke=INK, sw=4.5))
            # center vein
            tx, ty = x + math.cos(la) * L * 0.8, y + math.sin(la) * L * 0.8
            out.append(f"<line x1='{x:.1f}' y1='{y:.1f}' x2='{tx:.1f}' y2='{ty:.1f}' "
                       f"stroke='{INK}' stroke-width='2' stroke-opacity='0.55'/>")
    return out


def olive(cx):
    out = []
    branches = [
        ((cx - 10, 1660), (cx - 60, 1200), (cx + 30, 750), (cx + 20, 330), 7.0),
        ((cx - 18, 1180), (cx - 150, 980), (cx - 230, 800), (cx - 270, 620), 5.5),
        ((cx + 16, 900), (cx + 140, 760), (cx + 210, 640), (cx + 250, 470), 5.5),
    ]
    olives = []
    for bi, (p0, p1, p2, p3, w) in enumerate(branches):
        out.append(stem_path(p0, p1, p2, p3, w))
        n = 9 if bi == 0 else 6
        for i in range(n):
            t = 0.15 + 0.80 * i / (n - 1)
            x, y = bez(p0, p1, p2, p3, t)
            ang = bez_tan(p0, p1, p2, p3, t)
            for side in (1, -1):
                la = ang + side * (0.85 + 0.15 * jitter(i + bi))
                L = 125 - 45 * t + 8 * jitter(i * 2 + bi)
                out.append(leaf((x, y), la, L, L * 0.14, fill=SAGE, stroke=None))
        # olives near branch midpoints
        for tt in (0.35, 0.62):
            x, y = bez(p0, p1, p2, p3, tt + 0.04 * jitter(bi + tt))
            ox = x + 30 * jitter(bi * 3 + tt)
            oy = y + 24
            olives.append((ox, oy))
    for i, (ox, oy) in enumerate(olives[:6]):
        out.append(f"<circle cx='{ox:.1f}' cy='{oy:.1f}' r='27' fill='{TERRACOTTA}'/>")
        out.append(f"<circle cx='{ox - 8:.1f}' cy='{oy - 9:.1f}' r='6' fill='{CREAM}' fill-opacity='0.85'/>")
    return out


def pampas(cx):
    out = []
    stems = [(cx - 150, 560, -1), (cx + 15, 340, 1), (cx + 165, 660, -1)]
    for si, (sx, top, lean) in enumerate(stems):
        p0 = (sx, 1660)
        p3 = (sx + lean * 55, top)
        p1 = (sx - lean * 30, 1660 - (1660 - top) * 0.35)
        p2 = (sx + lean * 45, 1660 - (1660 - top) * 0.72)
        out.append(stem_path(p0, p1, p2, p3, 5))
        # plume: fine arcs along the top 38% of the stem
        m = 56
        for i in range(m):
            t = 0.62 + 0.38 * i / (m - 1)
            x, y = bez(p0, p1, p2, p3, t)
            base_ang = bez_tan(p0, p1, p2, p3, t)
            for k in range(2):
                fan = jitter(si * 100 + i * 2 + k) * 1.25
                la = base_ang + fan
                L = (150 - 85 * abs(fan) / 1.25) * (0.55 + 0.45 * t) + 10 * jitter(i + k)
                ex, ey = x + math.cos(la) * L, y + math.sin(la) * L
                # droop the tip slightly
                qx = x + math.cos(la) * L * 0.55
                qy = y + math.sin(la) * L * 0.55 - 12
                col = SAND if (i + k) % 3 else "#B9A17C"
                out.append(f"<path d='M {x:.1f} {y:.1f} Q {qx:.1f} {qy:.1f} {ex:.1f} {ey:.1f}' "
                           f"fill='none' stroke='{col}' stroke-width='3.4' "
                           f"stroke-opacity='0.85' stroke-linecap='round'/>")
    return out


def allium(cx):
    out = []
    heads = [
        (cx - 190, 620, 82, TERRACOTTA), (cx - 80, 430, 68, SAGE),
        (cx + 30, 720, 60, SAND), (cx + 130, 380, 78, DEEP_SAGE),
        (cx + 215, 610, 56, TERRACOTTA),
    ]
    for hi, (hx, hy, r, col) in enumerate(heads):
        bx = cx + (hx - cx) * 0.35
        p0, p3 = (bx, 1660), (hx, hy + r * 0.4)
        p1 = (bx + 20 * jitter(hi), 1300)
        p2 = (hx - 30 * jitter(hi + 2), hy + 420)
        out.append(stem_path(p0, p1, p2, p3, 5.5))
        # small leaf near base
        la = -math.pi / 2 + 0.6 * jitter(hi + 5)
        out.append(leaf((bx, 1500 + 60 * jitter(hi)), la, 130, 16, fill="none", stroke=INK, sw=4))
        # seed head: rays + dots
        m = 18
        for i in range(m):
            a = 2 * math.pi * i / m + 0.15 * jitter(hi * 10 + i)
            rr = r * (0.92 + 0.10 * jitter(i + hi))
            dx, dy = hx + math.cos(a) * rr, hy + math.sin(a) * rr
            out.append(f"<line x1='{hx:.1f}' y1='{hy:.1f}' x2='{dx:.1f}' y2='{dy:.1f}' "
                       f"stroke='{INK}' stroke-width='2.6' stroke-opacity='0.7'/>")
            out.append(f"<circle cx='{dx:.1f}' cy='{dy:.1f}' r='9' fill='{col}'/>")
        out.append(f"<circle cx='{hx:.1f}' cy='{hy:.1f}' r='7' fill='{INK}'/>")
    return out


def arch_grass(cx):
    out = []
    aw, atop, abot = 330, 760, 1470
    out.append(f"<path d='M {cx - aw:.1f} {abot} L {cx - aw:.1f} {atop} "
               f"A {aw} {aw} 0 0 1 {cx + aw:.1f} {atop} L {cx + aw:.1f} {abot} Z' "
               f"fill='{TERRACOTTA}' fill-opacity='0.20'/>")
    out.append(f"<circle cx='{cx + 150:.1f}' cy='620' r='95' fill='{SAND}' fill-opacity='0.55'/>")
    out.append(f"<line x1='{cx - 420:.1f}' y1='{abot}' x2='{cx + 420:.1f}' y2='{abot}' "
               f"stroke='{INK}' stroke-width='5' stroke-linecap='round'/>")
    # wheat-like stems
    for si, (ox, top, lean) in enumerate([(-140, 560, -1), (-30, 420, 1), (90, 640, 1)]):
        sx = cx + ox
        p0 = (sx, abot)
        p3 = (sx + lean * 60, top)
        p1 = (sx - lean * 25, abot - (abot - top) * 0.4)
        p2 = (sx + lean * 40, abot - (abot - top) * 0.75)
        out.append(stem_path(p0, p1, p2, p3, 5.5))
        n = 8
        for i in range(n):
            t = 0.45 + 0.5 * i / (n - 1)
            x, y = bez(p0, p1, p2, p3, t)
            ang = bez_tan(p0, p1, p2, p3, t)
            side = 1 if i % 2 == 0 else -1
            la = ang + side * 0.75
            L = 95 - 50 * (t - 0.45) / 0.5
            out.append(leaf((x, y), la, L, L * 0.22, fill=INK, stroke=None))
        x, y = bez(p0, p1, p2, p3, 1.0)
        out.append(leaf((x, y), bez_tan(p0, p1, p2, p3, 1.0), 70, 15, fill=INK))
    # eucalyptus sprig crossing from bottom-left
    p0, p1, p2, p3 = (cx - 330, abot), (cx - 300, 1250), (cx - 200, 1080), (cx - 90, 900)
    out.append(stem_path(p0, p1, p2, p3, 5.5))
    for i in range(7):
        t = 0.2 + 0.75 * i / 6
        x, y = bez(p0, p1, p2, p3, t)
        ang = bez_tan(p0, p1, p2, p3, t)
        side = 1 if i % 2 == 0 else -1
        la = ang + side * 1.2
        r = 34 - 12 * t
        lx, ly = x + math.cos(la) * 20, y + math.sin(la) * 20
        out.append(f"<line x1='{x:.1f}' y1='{y:.1f}' x2='{lx:.1f}' y2='{ly:.1f}' stroke='{INK}' stroke-width='4'/>")
        out.append(f"<circle cx='{lx + math.cos(la) * r * 0.8:.1f}' cy='{ly + math.sin(la) * r * 0.8:.1f}' "
                   f"r='{r:.1f}' fill='{SAGE}' fill-opacity='0.9'/>")
    return out


DESIGNS = [
    ("01-eucalyptus", eucalyptus),
    ("02-fern", fern),
    ("03-olive-branch", olive),
    ("04-pampas-grass", pampas),
    ("05-allium-meadow", allium),
    ("06-terracotta-arch", arch_grass),
]


def build_svg(fn, width):
    cx = width / 2
    body = "\n".join(fn(cx))
    return (f"<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 {width:.0f} {H:.0f}'>\n"
            f"<rect width='{width:.0f}' height='{H:.0f}' fill='{CREAM}'/>\n{body}\n</svg>")


def main():
    src = os.path.join(PRODUCT, "source-svg")
    os.makedirs(src, exist_ok=True)
    for name, fn in DESIGNS:
        for ratio, (w, label, px_w) in RATIOS.items():
            svg = build_svg(fn, w)
            if ratio == "2x3":
                with open(os.path.join(src, f"{name}.svg"), "w") as f:
                    f.write(svg)
            outdir = os.path.join(PRODUCT, "print-files", ratio)
            os.makedirs(outdir, exist_ok=True)
            out = os.path.join(outdir, f"{name}-{label}.png")
            cairosvg.svg2png(bytestring=svg.encode(), write_to=out,
                             output_width=px_w, output_height=int(round(px_w * H / w)))
            print("wrote", out)


if __name__ == "__main__":
    main()
