#!/usr/bin/env python3
"""Generate the 2026 Budget Planner: 10-page printable PDF, US Letter + A4.

Each page is composed as an SVG sized in real inches, rendered to a single-page
PDF with cairosvg, then merged with pypdf. Layout is parametric on page size.
"""
import io
import os

import cairosvg
from pypdf import PdfWriter, PdfReader

HERE = os.path.dirname(os.path.abspath(__file__))
PRODUCT = os.path.normpath(os.path.join(HERE, "..", "02-budget-planner-2026"))

INK = "#3F3A33"
SOFT = "#8A8070"
LINE = "#D8D0C2"
CREAM = "#FBF8F2"
SAGE = "#8C9A78"
DEEP_SAGE = "#6F7D5C"
TERRACOTTA = "#C08552"
PAPER = "#FFFFFF"

SERIF = "Liberation Serif"
SANS = "Liberation Sans"

SIZES = {
    "us-letter": (816, 1056, "8.5in", "11in"),
    "a4": (794, 1123, "210mm", "297mm"),
}
MARGIN = 64


def esc(s):
    return s.replace("&", "&amp;").replace("<", "&lt;").replace(">", "&gt;")


def text(x, y, s, size=14, font=SANS, weight="normal", color=INK, anchor="start",
         spacing=None, style="normal"):
    sp = f" letter-spacing='{spacing}'" if spacing else ""
    return (f"<text x='{x:.1f}' y='{y:.1f}' font-family='{font}' font-size='{size}' "
            f"font-weight='{weight}' font-style='{style}' fill='{color}' "
            f"text-anchor='{anchor}'{sp}>{esc(s)}</text>")


def hline(x1, x2, y, color=LINE, w=1.2):
    return f"<line x1='{x1:.1f}' y1='{y:.1f}' x2='{x2:.1f}' y2='{y:.1f}' stroke='{color}' stroke-width='{w}'/>"


def rect(x, y, w, h, fill="none", stroke=None, sw=1.2, rx=0):
    s = f"stroke='{stroke}' stroke-width='{sw}'" if stroke else "stroke='none'"
    return f"<rect x='{x:.1f}' y='{y:.1f}' width='{w:.1f}' height='{h:.1f}' rx='{rx}' fill='{fill}' {s}/>"


def sprig(cx, cy, scale=1.0, color=SAGE):
    """Small eucalyptus sprig accent."""
    out = [f"<g transform='translate({cx:.1f} {cy:.1f}) scale({scale})'>"]
    out.append(f"<path d='M 0 40 C -6 10 2 -14 6 -40' fill='none' stroke='{INK}' stroke-width='2'/>")
    import math
    for i in range(6):
        t = i / 5.0
        x = -6 + 12 * t + (2 if i % 2 else -2)
        y = 34 - 68 * t
        side = 1 if i % 2 == 0 else -1
        lx, ly = x + side * 12, y - 4
        out.append(f"<line x1='{x}' y1='{y}' x2='{lx}' y2='{ly}' stroke='{INK}' stroke-width='1.5'/>")
        r = 9 - 3.5 * t
        out.append(f"<circle cx='{lx + side * r * 0.7:.1f}' cy='{ly:.1f}' r='{r:.1f}' fill='{color}'/>")
    out.append("</g>")
    return "".join(out)


def page_header(W, title, subtitle=None):
    out = [text(MARGIN, MARGIN + 22, title.upper(), size=26, font=SERIF, weight="bold",
                spacing="4")]
    if subtitle:
        out.append(text(MARGIN, MARGIN + 44, subtitle, size=11, color=SOFT, spacing="1"))
    out.append(hline(MARGIN, W - MARGIN, MARGIN + 58, color=INK, w=2))
    out.append(hline(MARGIN, W - MARGIN, MARGIN + 63, color=LINE, w=1))
    return out, MARGIN + 92


def table(x, y, w, headers, widths, rows, row_h=26, header_fill=CREAM):
    """widths are fractions of w."""
    out = [rect(x, y, w, row_h, fill=header_fill, stroke=LINE)]
    cx = x
    for htxt, frac in zip(headers, widths):
        out.append(text(cx + 8, y + row_h - 8, htxt.upper(), size=9.5, weight="bold",
                        color=SOFT, spacing="1"))
        cx += w * frac
    yy = y + row_h
    for _ in range(rows):
        out.append(rect(x, yy, w, row_h, fill="none", stroke=LINE, sw=1))
        yy += row_h
    # column separators
    cx = x
    for frac in widths[:-1]:
        cx += w * frac
        out.append(f"<line x1='{cx:.1f}' y1='{y:.1f}' x2='{cx:.1f}' y2='{yy:.1f}' stroke='{LINE}' stroke-width='1'/>")
    out.append(rect(x, y, w, yy - y, fill="none", stroke=INK, sw=1.4))
    return out, yy


# ---------------------------------------------------------------- pages

def p_cover(W, H):
    out = [rect(0, 0, W, H, fill=CREAM)]
    out.append(rect(MARGIN, MARGIN, W - 2 * MARGIN, H - 2 * MARGIN, stroke=INK, sw=1.6))
    out.append(rect(MARGIN + 10, MARGIN + 10, W - 2 * MARGIN - 20, H - 2 * MARGIN - 20,
                    stroke=LINE, sw=1))
    cy = H * 0.36
    out.append(sprig(W / 2, cy - 130, 1.6))
    out.append(text(W / 2, cy, "2026", size=110, font=SERIF, weight="bold", anchor="middle",
                    spacing="10"))
    out.append(hline(W / 2 - 120, W / 2 + 120, cy + 40, color=INK, w=1.6))
    out.append(text(W / 2, cy + 86, "BUDGET PLANNER", size=30, font=SERIF, anchor="middle",
                    spacing="10"))
    out.append(text(W / 2, cy + 120, "monthly budgets  ·  savings goals  ·  debt payoff",
                    size=12, color=SOFT, anchor="middle", spacing="2"))
    out.append(text(W / 2, H - MARGIN - 40, "THIS PLANNER BELONGS TO", size=10, color=SOFT,
                    anchor="middle", spacing="3"))
    out.append(hline(W / 2 - 130, W / 2 + 130, H - MARGIN - 20, color=INK, w=1))
    return out


def p_year(W, H):
    out, y0 = page_header(W, "Year at a Glance", "Track every month's totals in one place")
    cw = (W - 2 * MARGIN - 2 * 18) / 3
    ch = (H - y0 - MARGIN - 3 * 18) / 4
    months = ["JANUARY", "FEBRUARY", "MARCH", "APRIL", "MAY", "JUNE", "JULY",
              "AUGUST", "SEPTEMBER", "OCTOBER", "NOVEMBER", "DECEMBER"]
    for i, m in enumerate(months):
        x = MARGIN + (i % 3) * (cw + 18)
        y = y0 + (i // 3) * (ch + 18)
        out.append(rect(x, y, cw, ch, fill=PAPER, stroke=INK, sw=1.2, rx=4))
        out.append(rect(x, y, cw, 24, fill=CREAM, stroke=INK, sw=1.2, rx=4))
        out.append(text(x + cw / 2, y + 17, m, size=11, weight="bold", anchor="middle",
                        spacing="2", font=SERIF))
        for j, label in enumerate(["Income", "Expenses", "Saved"]):
            ly = y + 48 + j * 26
            out.append(text(x + 10, ly, label, size=10, color=SOFT))
            out.append(hline(x + 68, x + cw - 12, ly + 2))
    return out


def p_monthly(W, H):
    out, y0 = page_header(W, "Monthly Budget", "One page per month — print 12 copies")
    out.append(text(MARGIN, y0, "MONTH:", size=11, weight="bold", color=SOFT, spacing="2"))
    out.append(hline(MARGIN + 62, MARGIN + 250, y0 + 2, color=INK, w=1))
    y = y0 + 24
    w = W - 2 * MARGIN
    out.append(text(MARGIN, y + 16, "INCOME", size=13, font=SERIF, weight="bold", spacing="2"))
    t, y = table(MARGIN, y + 26, w, ["Source", "Expected", "Actual"], [0.55, 0.225, 0.225], 3)
    out += t
    y += 26
    out.append(text(MARGIN, y + 16, "FIXED EXPENSES", size=13, font=SERIF, weight="bold", spacing="2"))
    t, y = table(MARGIN, y + 26, w, ["Expense", "Due", "Budget", "Actual"], [0.46, 0.14, 0.20, 0.20], 8)
    out += t
    y += 26
    out.append(text(MARGIN, y + 16, "VARIABLE SPENDING", size=13, font=SERIF, weight="bold", spacing="2"))
    t, y = table(MARGIN, y + 26, w, ["Category", "Budget", "Actual", "Diff"], [0.46, 0.18, 0.18, 0.18], 6)
    out += t
    # summary strip
    y += 28
    bw = (w - 2 * 16) / 3
    for i, label in enumerate(["TOTAL INCOME", "TOTAL SPENT", "LEFT OVER"]):
        x = MARGIN + i * (bw + 16)
        fill = CREAM if i < 2 else "#EEF0E4"
        out.append(rect(x, y, bw, 56, fill=fill, stroke=INK, sw=1.3, rx=5))
        out.append(text(x + bw / 2, y + 20, label, size=9.5, weight="bold", color=SOFT,
                        anchor="middle", spacing="2"))
        out.append(text(x + bw / 2, y + 42, "$", size=15, anchor="middle", color=INK, font=SERIF))
    return out


def p_expense(W, H):
    out, y0 = page_header(W, "Expense Tracker", "Log every purchase — awareness is the budget superpower")
    w = W - 2 * MARGIN
    rows = int((H - y0 - MARGIN - 26) // 26)
    t, _ = table(MARGIN, y0, w, ["Date", "Description", "Category", "Amount"],
                 [0.13, 0.47, 0.22, 0.18], rows)
    out += t
    return out


def p_bills(W, H):
    out, y0 = page_header(W, "Bill Payment Tracker", "Check off each bill as it's paid, all year long")
    w = W - 2 * MARGIN
    left = 0.34
    months = "JFMAMJJASOND"
    mw = w * (1 - left - 0.12) / 12
    row_h = 30
    out.append(rect(MARGIN, y0, w, row_h, fill=CREAM, stroke=LINE))
    out.append(text(MARGIN + 8, y0 + row_h - 10, "BILL", size=9.5, weight="bold", color=SOFT, spacing="1"))
    out.append(text(MARGIN + w * left + 8, y0 + row_h - 10, "AMT", size=9.5, weight="bold", color=SOFT))
    for i, mch in enumerate(months):
        x = MARGIN + w * (left + 0.12) + i * mw
        out.append(text(x + mw / 2, y0 + row_h - 10, mch, size=9.5, weight="bold",
                        color=SOFT, anchor="middle"))
    y = y0 + row_h
    nrows = int((H - y - MARGIN) // row_h)
    for r in range(nrows):
        out.append(rect(MARGIN, y, w, row_h, fill="none", stroke=LINE, sw=1))
        for i in range(12):
            x = MARGIN + w * (left + 0.12) + i * mw
            out.append(rect(x + mw / 2 - 7, y + row_h / 2 - 7, 14, 14, stroke=SOFT, sw=1.1, rx=3))
        y += row_h
    for frac in (left, left + 0.12):
        x = MARGIN + w * frac
        out.append(f"<line x1='{x:.1f}' y1='{y0}' x2='{x:.1f}' y2='{y:.1f}' stroke='{LINE}' stroke-width='1'/>")
    out.append(rect(MARGIN, y0, w, y - y0, fill="none", stroke=INK, sw=1.4))
    return out


def p_savings(W, H):
    out, y0 = page_header(W, "Savings Goals", "Color a segment for every 10% saved")
    w = W - 2 * MARGIN
    bh = (H - y0 - MARGIN - 3 * 22) / 4
    for g in range(4):
        y = y0 + g * (bh + 22)
        out.append(rect(MARGIN, y, w, bh, fill=PAPER, stroke=INK, sw=1.3, rx=6))
        out.append(text(MARGIN + 16, y + 28, "GOAL:", size=10, weight="bold", color=SOFT, spacing="2"))
        out.append(hline(MARGIN + 66, MARGIN + w * 0.55, y + 30, color=INK, w=1))
        out.append(text(MARGIN + w * 0.60, y + 28, "TARGET  $", size=10, weight="bold", color=SOFT, spacing="1"))
        out.append(hline(MARGIN + w * 0.60 + 66, MARGIN + w - 120, y + 30, color=INK, w=1))
        out.append(text(MARGIN + w - 110, y + 28, "BY:", size=10, weight="bold", color=SOFT))
        out.append(hline(MARGIN + w - 84, MARGIN + w - 16, y + 30, color=INK, w=1))
        seg_w = (w - 32 - 9 * 6) / 10
        for s in range(10):
            x = MARGIN + 16 + s * (seg_w + 6)
            out.append(rect(x, y + bh - 52, seg_w, 30, fill=CREAM, stroke=DEEP_SAGE, sw=1.2, rx=4))
            out.append(text(x + seg_w / 2, y + bh - 8, f"{(s + 1) * 10}%", size=8.5,
                            color=SOFT, anchor="middle"))
    return out


def p_debt(W, H):
    out, y0 = page_header(W, "Debt Payoff Tracker", "List debts smallest to largest and snowball them")
    w = W - 2 * MARGIN
    t, y = table(MARGIN, y0, w, ["Debt", "Balance", "Rate", "Min. Pmt", "Payoff Date"],
                 [0.34, 0.19, 0.13, 0.16, 0.18], 7)
    out += t
    y += 30
    out.append(text(MARGIN, y + 4, "PAYMENT LOG", size=13, font=SERIF, weight="bold", spacing="2"))
    rows = int((H - (y + 14) - MARGIN - 26) // 26)
    t, _ = table(MARGIN, y + 14, w, ["Date", "Debt", "Payment", "New Balance"],
                 [0.16, 0.42, 0.20, 0.22], rows)
    out += t
    return out


def p_subs(W, H):
    out, y0 = page_header(W, "Subscription Audit", "The fastest budget win: cancel what you forgot")
    w = W - 2 * MARGIN
    rows = int((H - y0 - MARGIN - 26 - 90) // 28)
    t, y = table(MARGIN, y0, w, ["Service", "Cost", "Cycle", "Renews", "Keep?"],
                 [0.36, 0.15, 0.15, 0.18, 0.16], rows, row_h=28)
    out += t
    y += 26
    out.append(rect(MARGIN, y, w, 56, fill="#F3E9DC", stroke=TERRACOTTA, sw=1.3, rx=6))
    out.append(text(MARGIN + 16, y + 23, "MONTHLY TOTAL  $", size=11, weight="bold", color=INK, spacing="1"))
    out.append(hline(MARGIN + 140, MARGIN + 260, y + 25, color=INK, w=1))
    out.append(text(MARGIN + 16, y + 45, "CANCELLED THIS YEAR — SAVED  $", size=11, weight="bold",
                    color=DEEP_SAGE, spacing="1"))
    out.append(hline(MARGIN + 238, MARGIN + 360, y + 47, color=INK, w=1))
    return out


def p_sinking(W, H):
    out, y0 = page_header(W, "Sinking Funds", "Save a little monthly for the big irregular expenses")
    w = W - 2 * MARGIN
    cw = (w - 18) / 2
    ch = (H - y0 - MARGIN - 2 * 18) / 3
    labels = ["Car repairs", "Holidays + gifts", "Travel", "Medical", "Home + appliances", "Annual bills"]
    for i, lab in enumerate(labels):
        x = MARGIN + (i % 2) * (cw + 18)
        y = y0 + (i // 2) * (ch + 18)
        out.append(rect(x, y, cw, ch, fill=PAPER, stroke=INK, sw=1.2, rx=6))
        out.append(rect(x, y, cw, 26, fill=CREAM, stroke=INK, sw=1.2, rx=6))
        out.append(text(x + 10, y + 18, lab.upper(), size=10.5, weight="bold", font=SERIF, spacing="1"))
        out.append(text(x + cw - 10, y + 18, "GOAL $______", size=9.5, color=SOFT, anchor="end"))
        mw = (cw - 20 - 5 * 6) / 6
        for r in range(2):
            for c in range(6):
                mi = r * 6 + c
                mx = x + 10 + c * (mw + 6)
                my = y + 40 + r * (mw * 0.9 + 22)
                out.append(rect(mx, my, mw, mw * 0.9, fill=CREAM, stroke=SOFT, sw=1, rx=3))
                out.append(text(mx + mw / 2, my + mw * 0.9 + 12, "JFMAMJJASOND"[mi], size=8,
                                color=SOFT, anchor="middle"))
    return out


def p_notes(W, H):
    out, y0 = page_header(W, "Notes + Ideas", "Money thoughts, plans, and wins worth remembering")
    dots = []
    step = 24
    y = y0 + 10
    while y < H - MARGIN:
        x = MARGIN + 4
        while x < W - MARGIN:
            dots.append(f"<circle cx='{x}' cy='{y}' r='1.4' fill='{LINE}'/>")
            x += step
        y += step
    return out + dots


PAGES = [p_cover, p_year, p_monthly, p_expense, p_bills, p_savings, p_debt, p_subs, p_sinking, p_notes]


def build_pdf(size_key):
    W, H, win, hin = SIZES[size_key]
    writer = PdfWriter()
    for fn in PAGES:
        body = "\n".join(fn(W, H))
        svg = (f"<svg xmlns='http://www.w3.org/2000/svg' width='{win}' height='{hin}' "
               f"viewBox='0 0 {W} {H}'>"
               f"<rect width='{W}' height='{H}' fill='{PAPER}'/>{body}</svg>")
        pdf_bytes = cairosvg.svg2pdf(bytestring=svg.encode())
        writer.append(PdfReader(io.BytesIO(pdf_bytes)))
    os.makedirs(os.path.join(PRODUCT, "print-files"), exist_ok=True)
    out = os.path.join(PRODUCT, "print-files", f"2026-budget-planner-{size_key}.pdf")
    with open(out, "wb") as f:
        writer.write(f)
    print("wrote", out)


if __name__ == "__main__":
    for k in SIZES:
        build_pdf(k)
