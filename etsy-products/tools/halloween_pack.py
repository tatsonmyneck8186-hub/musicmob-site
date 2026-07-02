#!/usr/bin/env python3
"""Generate the Retro Halloween SVG/PNG pack: 15 designs, transparent PNGs.

Designs live in a 1500x1800 viewBox and render to 4500x5400 px (15x18in at
300 DPI — standard print-on-demand size). Text is converted to vector paths
with fontTools so the SVGs need no installed fonts (Cricut/Silhouette safe).
"""
import math
import os

import cairosvg
from fontTools.ttLib import TTFont
from fontTools.pens.svgPathPen import SVGPathPen

HERE = os.path.dirname(os.path.abspath(__file__))
PRODUCT = os.path.normpath(os.path.join(HERE, "..", "04-retro-halloween-svg-png-pack"))

# retro 70s Halloween palette
ORANGE = "#EE7A1E"
MUSTARD = "#F2A93B"
RUST = "#B44819"
CREAM = "#F2E1C1"
BROWN = "#3A2A20"
PURPLE = "#6C4E8E"
GREEN = "#7E9C4E"

W, H = 1500.0, 1800.0
CX = W / 2

FONT_PATH = "/usr/share/fonts/truetype/liberation/LiberationSerif-Bold.ttf"
_font = TTFont(FONT_PATH)
_glyphset = _font.getGlyphSet()
_cmap = _font.getBestCmap()
_upem = _font["head"].unitsPerEm


def glyph_path(ch):
    gname = _cmap[ord(ch)]
    pen = SVGPathPen(_glyphset)
    _glyphset[gname].draw(pen)
    return pen.getCommands(), _glyphset[gname].width


def arc_text(text, cx, cy, radius, size, color, arc="top", spacing=1.06,
             stroke=None, sw=0):
    """Text on a circle, converted to filled paths. arc='top' or 'bottom'."""
    scale = size / _upem
    widths = []
    for ch in text:
        if ch == " ":
            widths.append(_upem * 0.30 * scale * spacing)
        else:
            widths.append(glyph_path(ch)[1] * scale * spacing)
    total_ang = sum(widths) / radius
    out = []
    if arc == "top":
        a = -math.pi / 2 - total_ang / 2
    else:
        a = math.pi / 2 + total_ang / 2
    for ch, w in zip(text, widths):
        da = w / radius
        amid = a + (da / 2 if arc == "top" else -da / 2)
        px = cx + radius * math.cos(amid)
        py = cy + radius * math.sin(amid)
        rot = math.degrees(amid + math.pi / 2) if arc == "top" else math.degrees(amid - math.pi / 2)
        if ch != " ":
            d, adv = glyph_path(ch)
            s = f"stroke='{stroke}' stroke-width='{sw / scale:.0f}'" if stroke else "stroke='none'"
            out.append(
                f"<g transform='translate({px:.1f} {py:.1f}) rotate({rot:.2f}) "
                f"scale({scale:.5f} {-scale:.5f}) translate({-adv / 2:.0f} 0)'>"
                f"<path d='{d}' fill='{color}' {s}/></g>")
        a += da if arc == "top" else -da
    return "".join(out)


def flat_text(text, cx, y, size, color, spacing=1.02):
    """Straight centered text as paths (baseline at y)."""
    scale = size / _upem
    widths = [(_upem * 0.30 if c == " " else glyph_path(c)[1]) * scale * spacing for c in text]
    x = cx - sum(widths) / 2
    out = []
    for ch, w in zip(text, widths):
        if ch != " ":
            d, _ = glyph_path(ch)
            out.append(f"<g transform='translate({x:.1f} {y:.1f}) scale({scale:.5f} {-scale:.5f})'>"
                       f"<path d='{d}' fill='{color}'/></g>")
        x += w
    return "".join(out)


def sparkle(x, y, r, color):
    return (f"<path d='M {x} {y - r} Q {x} {y} {x + r} {y} Q {x} {y} {x} {y + r} "
            f"Q {x} {y} {x - r} {y} Q {x} {y} {x} {y - r} Z' fill='{color}'/>")


def star5(x, y, r, color, rot=0):
    pts = []
    for i in range(10):
        rr = r if i % 2 == 0 else r * 0.42
        a = math.radians(rot) - math.pi / 2 + i * math.pi / 5
        pts.append(f"{x + rr * math.cos(a):.1f},{y + rr * math.sin(a):.1f}")
    return f"<polygon points='{' '.join(pts)}' fill='{color}'/>"


def rays(cx, cy, r1, r2, n, color, w=14, rot=0.0):
    out = []
    for i in range(n):
        a = rot + 2 * math.pi * i / n
        out.append(f"<line x1='{cx + r1 * math.cos(a):.1f}' y1='{cy + r1 * math.sin(a):.1f}' "
                   f"x2='{cx + r2 * math.cos(a):.1f}' y2='{cy + r2 * math.sin(a):.1f}' "
                   f"stroke='{color}' stroke-width='{w}' stroke-linecap='round'/>")
    return "".join(out)


def happy_eyes(cx, cy, gap, r, color, w=16):
    """Closed happy eyes: two upside-down arcs."""
    out = []
    for s in (-1, 1):
        x = cx + s * gap
        out.append(f"<path d='M {x - r} {cy} Q {x} {cy - r * 1.3} {x + r} {cy}' "
                   f"fill='none' stroke='{color}' stroke-width='{w}' stroke-linecap='round'/>")
    return "".join(out)


def smile(cx, cy, w2, depth, color, sw=16, fill=False):
    if fill:
        return (f"<path d='M {cx - w2} {cy} Q {cx} {cy + depth} {cx + w2} {cy} "
                f"Q {cx} {cy + depth * 0.45} {cx - w2} {cy} Z' fill='{color}'/>")
    return (f"<path d='M {cx - w2} {cy} Q {cx} {cy + depth} {cx + w2} {cy}' "
            f"fill='none' stroke='{color}' stroke-width='{sw}' stroke-linecap='round'/>")


def cheeks(cx, cy, gap, r, color):
    return (f"<circle cx='{cx - gap}' cy='{cy}' r='{r}' fill='{color}' fill-opacity='0.75'/>"
            f"<circle cx='{cx + gap}' cy='{cy}' r='{r}' fill='{color}' fill-opacity='0.75'/>")


def ghost_shape(cx, cy, w2, h2, fill, stroke, sw=18):
    """Classic wavy-bottom ghost, centered-ish at cx, top at cy-h2."""
    top = cy - h2
    bot = cy + h2
    return (f"<path d='M {cx - w2} {bot} L {cx - w2} {cy - h2 * 0.25} "
            f"Q {cx - w2} {top} {cx} {top} Q {cx + w2} {top} {cx + w2} {cy - h2 * 0.25} "
            f"L {cx + w2} {bot} "
            f"Q {cx + w2 * 0.833} {bot - h2 * 0.28} {cx + w2 * 0.667} {bot} "
            f"Q {cx + w2 * 0.5} {bot + h2 * 0.28} {cx + w2 * 0.333} {bot} "
            f"Q {cx + w2 * 0.167} {bot - h2 * 0.28} {cx} {bot} "
            f"Q {cx - w2 * 0.167} {bot + h2 * 0.28} {cx - w2 * 0.333} {bot} "
            f"Q {cx - w2 * 0.5} {bot - h2 * 0.28} {cx - w2 * 0.667} {bot} "
            f"Q {cx - w2 * 0.833} {bot + h2 * 0.28} {cx - w2} {bot} Z' "
            f"fill='{fill}' stroke='{stroke}' stroke-width='{sw}' stroke-linejoin='round'/>")


def daisy(cx, cy, rp, rc, petal_fill, center_fill, n=10, stroke=BROWN, sw=12):
    out = []
    for i in range(n):
        a = 2 * math.pi * i / n
        px, py = cx + math.cos(a) * rp, cy + math.sin(a) * rp
        deg = math.degrees(a) + 90
        out.append(f"<ellipse cx='{px:.1f}' cy='{py:.1f}' rx='{rp * 0.30:.1f}' ry='{rp * 0.52:.1f}' "
                   f"transform='rotate({deg:.1f} {px:.1f} {py:.1f})' fill='{petal_fill}' "
                   f"stroke='{stroke}' stroke-width='{sw}'/>")
    out.append(f"<circle cx='{cx}' cy='{cy}' r='{rc}' fill='{center_fill}' "
               f"stroke='{stroke}' stroke-width='{sw}'/>")
    return out


def pumpkin(cx, cy, rx, ry, body=ORANGE, line=BROWN, sw=18, face=True):
    out = [f"<ellipse cx='{cx}' cy='{cy}' rx='{rx}' ry='{ry}' fill='{body}' "
           f"stroke='{line}' stroke-width='{sw}'/>"]
    for s in (-1, 1):
        out.append(f"<path d='M {cx + s * rx * 0.62} {cy - ry * 0.62} "
                   f"Q {cx + s * rx * 1.05} {cy} "
                   f"{cx + s * rx * 0.62} {cy + ry * 0.62}' "
                   f"fill='none' stroke='{line}' stroke-width='{sw * 0.6}' stroke-linecap='round'/>")
    # stem
    out.insert(0, f"<path d='M {cx - 26} {cy - ry - 60} Q {cx - 10} {cy - ry - 130} {cx + 40} {cy - ry - 120} "
                  f"L {cx + 30} {cy - ry + 10} L {cx - 40} {cy - ry + 10} Z' fill='{GREEN}' "
                  f"stroke='{line}' stroke-width='{sw * 0.7}' stroke-linejoin='round'/>")
    if face:
        out.append(happy_eyes(cx, cy - ry * 0.15, rx * 0.38, rx * 0.16, line, sw))
        out.append(smile(cx, cy + ry * 0.22, rx * 0.34, ry * 0.42, line, fill=True))
        out.append(cheeks(cx, cy + ry * 0.10, rx * 0.62, rx * 0.11, RUST))
    return out


# ---------------------------------------------------------------- 15 designs

def d01_groovy_pumpkin():
    out = [sparkle(CX - 480, 520, 55, MUSTARD), sparkle(CX + 470, 460, 70, ORANGE),
           sparkle(CX + 420, 1350, 50, RUST), sparkle(CX - 450, 1300, 62, MUSTARD),
           star5(CX - 380, 720, 40, PURPLE), star5(CX + 400, 800, 34, PURPLE)]
    out += pumpkin(CX, 1000, 430, 340)
    return out


def d02_happy_ghost():
    out = [rays(CX, 950, 560, 660, 16, MUSTARD, 16)]
    out.append(ghost_shape(CX, 950, 330, 430, CREAM, BROWN))
    out.append(happy_eyes(CX, 870, 120, 55, BROWN, 20))
    out.append(smile(CX, 990, 70, 90, BROWN, 20))
    out.append(cheeks(CX, 950, 205, 42, RUST))
    out += [sparkle(CX - 430, 600, 48, PURPLE), sparkle(CX + 430, 640, 56, ORANGE),
            sparkle(CX + 380, 1370, 44, MUSTARD)]
    return out


def d03_moon_and_stars():
    # crescent: big outer arc down the left, shallower inner arc back up
    top = (CX + 130, 560)
    bot = (CX + 130, 1440)
    out = [f"<path d='M {top[0]} {top[1]} A 445 445 0 1 0 {bot[0]} {bot[1]} "
           f"A 560 560 0 0 1 {top[0]} {top[1]} Z' fill='{MUSTARD}' "
           f"stroke='{BROWN}' stroke-width='18' stroke-linejoin='round'/>"]
    # sleepy face on the moon body
    fx = CX - 190
    out.append(f"<path d='M {fx - 60} {900} Q {fx - 20} {850} {fx + 20} {900}' fill='none' "
               f"stroke='{BROWN}' stroke-width='16' stroke-linecap='round'/>")
    out.append(smile(fx - 20, 1040, 55, 60, BROWN, 16))
    out.append(f"<circle cx='{fx - 130}' cy='1000' r='34' fill='{RUST}' fill-opacity='0.75'/>")
    out += [star5(CX + 330, 660, 68, ORANGE), star5(CX + 430, 900, 44, PURPLE),
            star5(CX + 350, 1160, 56, ORANGE), sparkle(CX + 460, 1350, 52, MUSTARD),
            sparkle(CX - 460, 560, 50, PURPLE), sparkle(CX + 240, 1060, 40, RUST)]
    return out


def d04_black_cat():
    out = [f"<circle cx='{CX}' cy='980' r='520' fill='{MUSTARD}'/>",
           rays(CX, 980, 560, 640, 20, ORANGE, 14)]
    # body: sitting silhouette
    out.append(f"<path d='M {CX - 210} {1400} Q {CX - 260} {1080} {CX - 120} {940} "
               f"L {CX - 150} {760} L {CX - 60} {840} Q {CX} {820} {CX + 60} {840} "
               f"L {CX + 150} {760} L {CX + 120} {940} Q {CX + 260} {1080} {CX + 210} {1400} Z' "
               f"fill='#221C26'/>")
    # tail
    out.append(f"<path d='M {CX + 200} {1390} Q {CX + 420} {1330} {CX + 380} {1120}' "
               f"fill='none' stroke='#221C26' stroke-width='64' stroke-linecap='round'/>")
    # face
    out.append(f"<circle cx='{CX - 70}' cy='1020' r='16' fill='{MUSTARD}'/>")
    out.append(f"<circle cx='{CX + 70}' cy='1020' r='16' fill='{MUSTARD}'/>")
    out.append(f"<path d='M {CX - 20} {1080} L {CX + 20} {1080} L {CX} {1112} Z' fill='{ORANGE}'/>")
    for s in (-1, 1):
        for dy in (-14, 14):
            out.append(f"<line x1='{CX + s * 95}' y1='{1085 + dy}' x2='{CX + s * 185}' y2='{1080 + dy * 2}' "
                       f"stroke='{CREAM}' stroke-width='10' stroke-linecap='round'/>")
    out.append(f"<line x1='{CX - 210}' y1='1400' x2='{CX + 210}' y2='1400' "
               f"stroke='#221C26' stroke-width='40' stroke-linecap='round'/>")
    return out


def d05_flower_pumpkin():
    out = [sparkle(CX - 460, 620, 56, ORANGE), sparkle(CX + 450, 580, 48, PURPLE),
           sparkle(CX - 400, 1380, 46, MUSTARD), star5(CX + 430, 1320, 52, RUST)]
    out += daisy(CX, 980, 400, 240, ORANGE, MUSTARD, n=12)
    out.append(happy_eyes(CX, 930, 95, 42, BROWN, 18))
    out.append(smile(CX, 1030, 85, 90, BROWN, fill=True))
    out.append(cheeks(CX, 990, 165, 34, RUST))
    return out


def d06_witch_hat():
    out = [rays(CX, 1010, 600, 690, 18, PURPLE, 14, rot=0.17)]
    # brim
    out.append(f"<path d='M {CX - 520} {1180} Q {CX} {1310} {CX + 520} {1180} "
               f"Q {CX} {1080} {CX - 520} {1180} Z' fill='{PURPLE}' "
               f"stroke='{BROWN}' stroke-width='18' stroke-linejoin='round'/>")
    # cone with bend
    out.append(f"<path d='M {CX - 260} {1160} Q {CX - 160} {760} {CX - 40} {620} "
               f"Q {CX - 220} {560} {CX - 60} {440} Q {CX + 60} {360} {CX + 40} {520} "
               f"Q {CX + 260} {700} {CX + 280} {1150} Q {CX} {1240} {CX - 260} {1160} Z' "
               f"fill='{PURPLE}' stroke='{BROWN}' stroke-width='18' stroke-linejoin='round'/>")
    # band + buckle
    out.append(f"<path d='M {CX - 250} {1130} Q {CX} {1215} {CX + 272} {1120} L {CX + 262} {1030} "
               f"Q {CX} {1120} {CX - 235} {1040} Z' fill='{ORANGE}' stroke='{BROWN}' stroke-width='16' "
               f"stroke-linejoin='round'/>")
    out.append(f"<rect x='{CX - 55}' y='1075' width='110' height='100' rx='16' fill='{MUSTARD}' "
               f"stroke='{BROWN}' stroke-width='16'/>")
    out += [star5(CX - 420, 700, 56, ORANGE), star5(CX + 420, 640, 46, MUSTARD),
            sparkle(CX + 380, 1420, 52, RUST), sparkle(CX - 430, 1450, 44, MUSTARD)]
    return out


def d07_bat_badge():
    out = [f"<circle cx='{CX}' cy='950' r='500' fill='{RUST}'/>",
           f"<circle cx='{CX}' cy='950' r='430' fill='{MUSTARD}' stroke='{BROWN}' stroke-width='14'/>"]
    bw = 380
    out.append(f"<path d='M {CX} {1000} "
               f"Q {CX - 60} {900} {CX - 130} {930} L {CX - 110} {880} "
               f"Q {CX - 260} {820} {CX - bw} {900} Q {CX - 300} {960} {CX - 240} {1010} "
               f"Q {CX - 160} {980} {CX - 120} {1030} Q {CX - 60} {1000} {CX} {1060} "
               f"Q {CX + 60} {1000} {CX + 120} {1030} Q {CX + 160} {980} {CX + 240} {1010} "
               f"Q {CX + 300} {960} {CX + bw} {900} Q {CX + 260} {820} {CX + 110} {880} "
               f"L {CX + 130} {930} Q {CX + 60} {900} {CX} {1000} Z' fill='{BROWN}'/>")
    # head + ears
    out.append(f"<circle cx='{CX}' cy='880' r='80' fill='{BROWN}'/>")
    out.append(f"<path d='M {CX - 60} {830} L {CX - 78} {742} L {CX - 18} {800} Z' fill='{BROWN}'/>")
    out.append(f"<path d='M {CX + 60} {830} L {CX + 78} {742} L {CX + 18} {800} Z' fill='{BROWN}'/>")
    out.append(f"<circle cx='{CX - 28}' cy='872' r='12' fill='{MUSTARD}'/>")
    out.append(f"<circle cx='{CX + 28}' cy='872' r='12' fill='{MUSTARD}'/>")
    out += [star5(CX - 250, 700, 40, RUST), star5(CX + 250, 700, 40, RUST),
            star5(CX, 1240, 46, RUST)]
    return out


def d08_candy_corn_rainbow():
    bands = [(CREAM, 520), (ORANGE, 420), (MUSTARD, 320)]
    cy = 1120
    out = []
    for col, r in bands:
        out.append(f"<path d='M {CX - r} {cy} A {r} {r} 0 0 1 {CX + r} {cy}' fill='none' "
                   f"stroke='{col}' stroke-width='96' stroke-linecap='round'/>")
        out.append(f"<path d='M {CX - r} {cy} A {r} {r} 0 0 1 {CX + r} {cy}' fill='none' "
                   f"stroke='{BROWN}' stroke-width='8' stroke-opacity='0.0'/>")
    # candy corn pieces at the feet
    for s in (-1, 1):
        x = CX + s * 420
        out.append(f"<g transform='translate({x} {cy + 150}) rotate({-8 * s})'>"
                   f"<path d='M -70 60 L 0 -120 L 70 60 Q 0 110 -70 60 Z' fill='{CREAM}' "
                   f"stroke='{BROWN}' stroke-width='14' stroke-linejoin='round'/>"
                   f"<path d='M -47 0 L 47 0 L 70 60 Q 0 110 -70 60 Z' fill='{ORANGE}' "
                   f"stroke='{BROWN}' stroke-width='14' stroke-linejoin='round'/>"
                   f"<path d='M -70 60 Q 0 110 70 60 L 58 30 Q 0 72 -58 30 Z' fill='{MUSTARD}' "
                   f"stroke='{BROWN}' stroke-width='12' stroke-linejoin='round'/></g>")
    out += [sparkle(CX, 460, 60, PURPLE), star5(CX - 380, 560, 48, RUST),
            star5(CX + 380, 560, 48, RUST), sparkle(CX - 620, 1000, 46, MUSTARD),
            sparkle(CX + 620, 1000, 46, MUSTARD)]
    return out


def d09_spider_web():
    # half-web hanging from the top, spider dangling below
    cx, cy = CX, 430
    out = []
    n = 8
    for r in (170, 300, 430, 560):
        pts = []
        for i in range(n + 1):
            a = math.pi * i / n  # 0..pi sweeps the lower semicircle
            pts.append((cx + r * math.cos(a), cy + r * math.sin(a)))
        d = f"M {pts[0][0]:.1f} {pts[0][1]:.1f}"
        for i in range(1, len(pts)):
            # sag each segment toward the web center
            mx = (pts[i - 1][0] + pts[i][0]) / 2
            my = (pts[i - 1][1] + pts[i][1]) / 2
            vx, vy = mx - cx, my - cy
            vlen = math.hypot(vx, vy) or 1
            mx -= vx / vlen * r * 0.13
            my -= vy / vlen * r * 0.13
            d += f" Q {mx:.1f} {my:.1f} {pts[i][0]:.1f} {pts[i][1]:.1f}"
        out.append(f"<path d='{d}' fill='none' stroke='{BROWN}' stroke-width='14'/>")
    for i in range(n + 1):
        a = math.pi * i / n
        out.append(f"<line x1='{cx}' y1='{cy}' x2='{cx + 560 * math.cos(a):.1f}' "
                   f"y2='{cy + 560 * math.sin(a):.1f}' stroke='{BROWN}' stroke-width='14'/>")
    # hanging spider
    out.append(f"<line x1='{cx}' y1='{cy + 560}' x2='{cx}' y2='1240' stroke='{BROWN}' stroke-width='12'/>")
    out.append(f"<circle cx='{cx}' cy='1330' r='90' fill='{ORANGE}' stroke='{BROWN}' stroke-width='16'/>")
    out.append(f"<circle cx='{cx}' cy='1240' r='46' fill='{BROWN}'/>")
    for s in (-1, 1):
        for i, (dx, dy) in enumerate([(150, -40), (170, 20), (160, 90), (130, 150)]):
            out.append(f"<path d='M {cx} {1320 + i * 8} Q {cx + s * dx * 0.7} {1300 + dy * 0.4} "
                       f"{cx + s * dx} {1330 + dy}' fill='none' stroke='{BROWN}' "
                       f"stroke-width='14' stroke-linecap='round'/>")
    out.append(happy_eyes(cx, 1320, 34, 16, BROWN, 10))
    out.append(smile(cx, 1352, 22, 24, BROWN, 10))
    out += [sparkle(CX - 480, 1180, 46, MUSTARD), sparkle(CX + 480, 1180, 46, PURPLE)]
    return out


def d10_cauldron():
    out = []
    # bubbles
    for (bx, by, r, col) in [(CX - 120, 560, 44, GREEN), (CX + 40, 460, 62, GREEN),
                             (CX + 190, 580, 36, MUSTARD), (CX - 30, 640, 26, GREEN)]:
        out.append(f"<circle cx='{bx}' cy='{by}' r='{r}' fill='{col}' stroke='{BROWN}' stroke-width='12'/>")
    # goo top
    out.append(f"<ellipse cx='{CX}' cy='800' rx='330' ry='90' fill='{GREEN}' "
               f"stroke='{BROWN}' stroke-width='16'/>")
    # pot
    out.append(f"<path d='M {CX - 330} {800} Q {CX - 380} {820} {CX - 360} {880} "
               f"Q {CX - 340} {1240} {CX} {1290} Q {CX + 340} {1240} {CX + 360} {880} "
               f"Q {CX + 380} {820} {CX + 330} {800} Q {CX} {880} {CX - 330} {800} Z' "
               f"fill='{PURPLE}' stroke='{BROWN}' stroke-width='18' stroke-linejoin='round'/>")
    # legs
    for s in (-1, 1):
        out.append(f"<path d='M {CX + s * 200} {1250} L {CX + s * 250} {1370}' stroke='{BROWN}' "
                   f"stroke-width='40' stroke-linecap='round' fill='none'/>")
    # drip
    out.append(f"<path d='M {CX - 150} {830} q 0 90 -34 96 q -30 6 -30 -40 q 0 -50 30 -70 Z' "
               f"fill='{GREEN}' stroke='{BROWN}' stroke-width='12' stroke-linejoin='round'/>")
    out.append(happy_eyes(CX, 1000, 120, 52, MUSTARD, 18))
    out.append(smile(CX, 1100, 80, 80, MUSTARD, 18))
    out += [sparkle(CX - 440, 700, 52, ORANGE), sparkle(CX + 460, 760, 46, MUSTARD),
            star5(CX + 400, 1300, 44, RUST), star5(CX - 420, 1340, 40, ORANGE)]
    return out


def d11_eye_daisy():
    out = [sparkle(CX - 460, 640, 52, RUST), sparkle(CX + 470, 600, 46, GREEN),
           star5(CX - 400, 1340, 48, PURPLE), star5(CX + 430, 1300, 42, MUSTARD)]
    out += daisy(CX, 980, 400, 230, CREAM, CREAM, n=12)
    out.append(f"<circle cx='{CX}' cy='980' r='150' fill='{GREEN}' stroke='{BROWN}' stroke-width='14'/>")
    out.append(f"<circle cx='{CX}' cy='980' r='70' fill='{BROWN}'/>")
    out.append(f"<circle cx='{CX + 26}' cy='954' r='22' fill='{CREAM}'/>")
    return out


def d12_daisy_skull():
    out = [rays(CX, 960, 560, 640, 14, ORANGE, 16, rot=0.22)]
    # skull
    out.append(f"<path d='M {CX - 300} {880} Q {CX - 300} {580} {CX} {580} Q {CX + 300} {580} {CX + 300} {880} "
               f"Q {CX + 300} {1050} {CX + 180} {1090} L {CX + 180} {1200} Q {CX} {1260} {CX - 180} {1200} "
               f"L {CX - 180} {1090} Q {CX - 300} {1050} {CX - 300} {880} Z' "
               f"fill='{CREAM}' stroke='{BROWN}' stroke-width='18' stroke-linejoin='round'/>")
    # daisy eyes
    for s in (-1, 1):
        ex = CX + s * 120
        for i in range(8):
            a = 2 * math.pi * i / 8
            px, py = ex + math.cos(a) * 62, 880 + math.sin(a) * 62
            deg = math.degrees(a) + 90
            out.append(f"<ellipse cx='{px:.1f}' cy='{py:.1f}' rx='20' ry='34' "
                       f"transform='rotate({deg:.1f} {px:.1f} {py:.1f})' fill='{ORANGE}' "
                       f"stroke='{BROWN}' stroke-width='8'/>")
        out.append(f"<circle cx='{ex}' cy='880' r='34' fill='{BROWN}'/>")
    # nose + teeth
    out.append(f"<path d='M {CX - 24} {1035} Q {CX} {985} {CX + 24} {1035} Q {CX} {1065} {CX - 24} {1035} Z' "
               f"fill='{BROWN}'/>")
    for i in range(3):
        x = CX - 60 + i * 60
        out.append(f"<line x1='{x + 30}' y1='1160' x2='{x + 30}' y2='1230' stroke='{BROWN}' stroke-width='12'/>")
    out += [sparkle(CX - 440, 1330, 48, PURPLE), sparkle(CX + 440, 1330, 48, MUSTARD)]
    return out


def d13_stay_spooky():
    out = [arc_text("STAY SPOOKY", CX, 1120, 620, 190, RUST, arc="top")]
    out.append(ghost_shape(CX, 1120, 240, 310, CREAM, BROWN, 16))
    out.append(happy_eyes(CX, 1060, 88, 40, BROWN, 16))
    out.append(smile(CX, 1150, 52, 66, BROWN, 16))
    out.append(cheeks(CX, 1090, 150, 32, RUST))
    out += [star5(CX - 520, 1150, 56, ORANGE), star5(CX + 520, 1150, 56, ORANGE),
            sparkle(CX - 380, 1420, 48, MUSTARD), sparkle(CX + 380, 1420, 48, MUSTARD),
            sparkle(CX, 330, 62, PURPLE)]
    return out


def d14_spooky_season():
    cy = 950
    out = [f"<circle cx='{CX}' cy='{cy}' r='620' fill='none' stroke='{BROWN}' stroke-width='16'/>",
           f"<circle cx='{CX}' cy='{cy}' r='340' fill='none' stroke='{BROWN}' stroke-width='12'/>"]
    out.append(arc_text("SPOOKY", CX, cy, 455, 150, RUST, arc="top"))
    out.append(arc_text("SEASON", CX, cy, 475, 150, RUST, arc="bottom"))
    out += pumpkin(CX, cy + 30, 230, 185, sw=14)
    for s in (-1, 1):
        out.append(star5(CX + s * 480, cy, 42, ORANGE))
    return out


def d15_hey_boo():
    out = []
    bands = [(PURPLE, 500), (ORANGE, 400), (MUSTARD, 300)]
    cy = 1000
    for col, r in bands:
        out.append(f"<path d='M {CX - r} {cy} A {r} {r} 0 0 1 {CX + r} {cy}' fill='none' "
                   f"stroke='{col}' stroke-width='84' stroke-linecap='round'/>")
    out.append(flat_text("HEY BOO", CX, 1240, 220, BROWN))
    out.append(f"<path d='M {CX - 380} {1330} Q {CX} {1430} {CX + 380} {1330}' fill='none' "
               f"stroke='{RUST}' stroke-width='20' stroke-linecap='round'/>")
    out += [sparkle(CX - 560, 780, 52, MUSTARD), sparkle(CX + 560, 780, 52, MUSTARD),
            star5(CX - 420, 500, 50, RUST), star5(CX + 420, 500, 50, RUST),
            sparkle(CX, 380, 66, ORANGE)]
    return out


DESIGNS = [
    ("01-groovy-pumpkin", d01_groovy_pumpkin),
    ("02-happy-ghost", d02_happy_ghost),
    ("03-moon-and-stars", d03_moon_and_stars),
    ("04-black-cat", d04_black_cat),
    ("05-flower-pumpkin", d05_flower_pumpkin),
    ("06-witch-hat", d06_witch_hat),
    ("07-bat-badge", d07_bat_badge),
    ("08-candy-corn-rainbow", d08_candy_corn_rainbow),
    ("09-spider-web", d09_spider_web),
    ("10-cauldron", d10_cauldron),
    ("11-eye-daisy", d11_eye_daisy),
    ("12-daisy-skull", d12_daisy_skull),
    ("13-stay-spooky", d13_stay_spooky),
    ("14-spooky-season", d14_spooky_season),
    ("15-hey-boo", d15_hey_boo),
]


def build_svg(fn):
    return (f"<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 {W:.0f} {H:.0f}'>\n"
            + "\n".join(fn()) + "\n</svg>")


def main():
    svg_dir = os.path.join(PRODUCT, "print-files", "svg")
    png_dir = os.path.join(PRODUCT, "print-files", "png-transparent")
    os.makedirs(svg_dir, exist_ok=True)
    os.makedirs(png_dir, exist_ok=True)
    for name, fn in DESIGNS:
        svg = build_svg(fn)
        with open(os.path.join(svg_dir, f"{name}.svg"), "w") as f:
            f.write(svg)
        cairosvg.svg2png(bytestring=svg.encode(),
                         write_to=os.path.join(png_dir, f"{name}-4500x5400-300dpi.png"),
                         output_width=4500, output_height=5400)
        print("wrote", name)


if __name__ == "__main__":
    main()
