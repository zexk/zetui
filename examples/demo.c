/*
 * demo.c -- zetui comprehensive feature showcase
 *
 * Every public API symbol is exercised here. Run with:
 *   zig build run-demo
 *
 * Navigate:  Tab / 1-6   switch panel
 * Quit:      q / Ctrl-C
 */

#include <string.h>
#include "zetui.h"

/* ================================================================== */
/*  Layout constants                                                   */
/* ================================================================== */

#define SIDEBAR_W  16   /* includes the heavy border column */
#define NUM_PANELS  6

/* ================================================================== */
/*  String building helpers (no snprintf in C89)                      */
/* ================================================================== */

static int demo_itoa(int n, char *buf)
{
    char tmp[12];
    int  i, neg, len;

    if (n == 0) { buf[0] = '0'; buf[1] = '\0'; return 1; }
    neg = (n < 0);
    if (neg) n = -n;
    i = 0;
    while (n > 0) { tmp[i++] = (char)('0' + (n % 10)); n /= 10; }
    len = neg ? 1 : 0;
    while (i > 0) buf[len++] = tmp[--i];
    if (neg) buf[0] = '-';
    buf[len] = '\0';
    return len;
}

/* Append integer n to buf at *pos, advance *pos. */
static void sb_int(char *buf, int *pos, int n)
{
    char tmp[12];
    int  len;
    len = demo_itoa(n, tmp);
    memcpy(buf + *pos, tmp, (size_t)len);
    *pos += len;
    buf[*pos] = '\0';
}

/* Append string s to buf at *pos, advance *pos. */
static void sb_str(char *buf, int *pos, const char *s)
{
    while (*s) buf[(*pos)++] = *s++;
    buf[*pos] = '\0';
}

/* Append a single char. */
static void sb_ch(char *buf, int *pos, char c)
{
    buf[(*pos)++] = c;
    buf[*pos] = '\0';
}

/* Append four hex digits for a Unicode codepoint. */
static void sb_hex(char *buf, int *pos, zetui_u32 cp)
{
    static const char hex[] = "0123456789ABCDEF";
    int shift;
    int started;
    started = 0;
    for (shift = 28; shift >= 0; shift -= 4) {
        unsigned d;
        d = (unsigned)((cp >> shift) & 0xFu);
        if (d || started || shift == 0) {
            buf[(*pos)++] = hex[d];
            started = 1;
        }
    }
    buf[*pos] = '\0';
}

/* ================================================================== */
/*  Helper: draw a box using an arbitrary codepoint table             */
/*  (zetui_draw_box always uses the light set; this covers all three) */
/* ================================================================== */

static void draw_box_table(zetui_ctx_t *ctx,
                            int x, int y, int w, int h,
                            const zetui_u32 *t, zetui_style_t sty)
{
    int i;
    if (w < 2 || h < 2) return;
    zetui_set_cell(ctx, x,         y,         zetui_cell_make(t[ZETUI_BOX_TL], sty));
    zetui_set_cell(ctx, x + w - 1, y,         zetui_cell_make(t[ZETUI_BOX_TR], sty));
    zetui_set_cell(ctx, x,         y + h - 1, zetui_cell_make(t[ZETUI_BOX_BL], sty));
    zetui_set_cell(ctx, x + w - 1, y + h - 1, zetui_cell_make(t[ZETUI_BOX_BR], sty));
    for (i = 1; i < w - 1; i++) {
        zetui_set_cell(ctx, x + i, y,         zetui_cell_make(t[ZETUI_BOX_H], sty));
        zetui_set_cell(ctx, x + i, y + h - 1, zetui_cell_make(t[ZETUI_BOX_H], sty));
    }
    for (i = 1; i < h - 1; i++) {
        zetui_set_cell(ctx, x,         y + i, zetui_cell_make(t[ZETUI_BOX_V], sty));
        zetui_set_cell(ctx, x + w - 1, y + i, zetui_cell_make(t[ZETUI_BOX_V], sty));
    }
}

/* ================================================================== */
/*  Style shortcuts                                                    */
/* ================================================================== */

static zetui_style_t mk_style(zetui_i32 fg, zetui_i32 bg, zetui_u32 attrs)
{
    zetui_style_t s;
    s.fg    = fg;
    s.bg    = bg;
    s.attrs = attrs;
    return s;
}

/* ================================================================== */
/*  Panel 1: Colors                                                    */
/* ================================================================== */

static void panel_colors(zetui_ctx_t *ctx, int x, int y, int w, int h)
{
    static const char *names[16] = {
        "black  ", "red    ", "green  ", "yellow ",
        "blue   ", "magenta", "cyan   ", "white  ",
        "Bblack ", "Bred   ", "Bgreen ", "Byellow",
        "Bblue  ", "Bmagnt.", "Bcyan  ", "Bwhite "
    };
    zetui_style_t hdr, s;
    int           ci, r, c;

    hdr = mk_style(ZETUI_COLOR_BRIGHT_WHITE, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_BOLD);

    /* -- Foreground colors ----------------------------------------- */
    zetui_draw_str(ctx, x, y, "16 foreground colors:", hdr);
    for (ci = 0; ci < 16; ci++) {
        s = mk_style(ci, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
        zetui_draw_str(ctx, x + (ci % 8) * 9, y + 1 + (ci / 8), names[ci], s);
    }

    /* -- Background colors ----------------------------------------- */
    zetui_draw_str(ctx, x, y + 4, "16 background colors:", hdr);
    for (ci = 0; ci < 16; ci++) {
        s = mk_style(ZETUI_COLOR_DEFAULT, ci, ZETUI_ATTR_NONE);
        zetui_draw_str(ctx, x + (ci % 8) * 9, y + 5 + (ci / 8), names[ci], s);
    }

    /* -- fg x bg matrix (8x8) -------------------------------------- */
    zetui_draw_str(ctx, x, y + 8, "fg x bg matrix (8 basic x 8 basic):", hdr);
    for (r = 0; r < 8; r++) {
        for (c = 0; c < 8; c++) {
            zetui_cell_t cell;
            cell = zetui_cell_make(
                (zetui_u32)'A' + (zetui_u32)r,
                mk_style(r, c, ZETUI_ATTR_NONE));
            zetui_set_cell(ctx, x + c * 2, y + 9 + r, cell);
        }
    }

    /* -- zetui_style_default() demo -------------------------------- */
    zetui_draw_str(ctx, x, y + 18, "zetui_style_default():", hdr);
    zetui_draw_str(ctx, x + 23, y + 18, "terminal default fg/bg", zetui_style_default());

    (void)w; (void)h;
}

/* ================================================================== */
/*  Panel 2: Attributes                                                */
/* ================================================================== */

static void panel_attrs(zetui_ctx_t *ctx, int x, int y, int w, int h)
{
    zetui_style_t hdr, s;
    int           row;

    hdr = mk_style(ZETUI_COLOR_BRIGHT_WHITE, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_BOLD);
    zetui_draw_str(ctx, x, y, "Text attributes (terminal support may vary):", hdr);

    row = y + 2;
#define ATROW(label, fg, bg, attr) \
    s = mk_style(fg, bg, attr); \
    zetui_draw_str(ctx, x, row++, "  " label, s)

    ATROW("NONE       normal text -- baseline",
          ZETUI_COLOR_DEFAULT, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
    ATROW("BOLD       heavier weight",
          ZETUI_COLOR_DEFAULT, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_BOLD);
    ATROW("DIM        reduced intensity",
          ZETUI_COLOR_DEFAULT, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_DIM);
    ATROW("ITALIC     slanted (if supported)",
          ZETUI_COLOR_DEFAULT, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_ITALIC);
    ATROW("UNDERLINE  single underline",
          ZETUI_COLOR_DEFAULT, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_UNDERLINE);
    ATROW("BLINK      slow blink (if supported)",
          ZETUI_COLOR_DEFAULT, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_BLINK);
    ATROW("REVERSE    swap fg/bg",
          ZETUI_COLOR_WHITE,   ZETUI_COLOR_BLACK,   ZETUI_ATTR_REVERSE);
    ATROW("HIDDEN     invisible text (select to reveal)",
          ZETUI_COLOR_WHITE,   ZETUI_COLOR_DEFAULT, ZETUI_ATTR_HIDDEN);
    ATROW("STRIKE     strikethrough",
          ZETUI_COLOR_DEFAULT, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_STRIKE);

#undef ATROW

    /* Combinations */
    row++;
    zetui_draw_str(ctx, x, row, "Combinations:", hdr);
    row++;

    s = mk_style(ZETUI_COLOR_CYAN, ZETUI_COLOR_DEFAULT,
                 ZETUI_ATTR_BOLD | ZETUI_ATTR_UNDERLINE);
    zetui_draw_str(ctx, x, row++, "  BOLD | UNDERLINE  (cyan fg)", s);

    s = mk_style(ZETUI_COLOR_RED, ZETUI_COLOR_YELLOW,
                 ZETUI_ATTR_BOLD | ZETUI_ATTR_REVERSE);
    zetui_draw_str(ctx, x, row++, "  BOLD | REVERSE    (red fg, yellow bg)", s);

    s = mk_style(ZETUI_COLOR_DEFAULT, ZETUI_COLOR_DEFAULT,
                 ZETUI_ATTR_DIM | ZETUI_ATTR_ITALIC | ZETUI_ATTR_STRIKE);
    zetui_draw_str(ctx, x, row++, "  DIM | ITALIC | STRIKE", s);

    s = mk_style(ZETUI_COLOR_BRIGHT_GREEN, ZETUI_COLOR_DEFAULT,
                 ZETUI_ATTR_BOLD | ZETUI_ATTR_BLINK | ZETUI_ATTR_UNDERLINE);
    zetui_draw_str(ctx, x, row++, "  BOLD | BLINK | UNDERLINE  (bright green)", s);

    (void)w; (void)h;
}

/* ================================================================== */
/*  Panel 3: Box styles + drawing primitives                          */
/* ================================================================== */

static void panel_boxes(zetui_ctx_t *ctx, int x, int y, int w, int h)
{
    zetui_style_t hdr, plain, accent, fill_sty;
    zetui_cell_t  fill_cell;
    int           bw, bh, bx, i;

    hdr      = mk_style(ZETUI_COLOR_BRIGHT_WHITE, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_BOLD);
    plain    = mk_style(ZETUI_COLOR_DEFAULT,       ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
    accent   = mk_style(ZETUI_COLOR_CYAN,          ZETUI_COLOR_DEFAULT, ZETUI_ATTR_BOLD);
    fill_sty = mk_style(ZETUI_COLOR_BLACK,         ZETUI_COLOR_GREEN,   ZETUI_ATTR_NONE);
    fill_cell = zetui_cell_make((zetui_u32)' ', fill_sty);

    /* -- Three box styles ------------------------------------------ */
    zetui_draw_str(ctx, x, y, "zetui_box_light / heavy / double  via draw_box_table():", hdr);

    bw = (w - 2) / 3 - 1;
    bh = 7;

    /* Light: zetui_draw_box uses light internally */
    bx = x;
    zetui_draw_box(ctx, bx, y + 1, bw, bh, plain);
    zetui_draw_str(ctx, bx + 2, y + 2, "light", accent);
    /* horizontal divider inside using T-junctions */
    zetui_set_cell(ctx, bx,          y + 4, zetui_cell_make(zetui_box_light[ZETUI_BOX_LT], plain));
    zetui_draw_hline(ctx, bx + 1, y + 4, bw - 2, zetui_box_light[ZETUI_BOX_H], plain);
    zetui_set_cell(ctx, bx + bw - 1, y + 4, zetui_cell_make(zetui_box_light[ZETUI_BOX_RT], plain));

    /* Heavy */
    bx = x + bw + 1;
    draw_box_table(ctx, bx, y + 1, bw, bh, zetui_box_heavy, plain);
    zetui_draw_str(ctx, bx + 2, y + 2, "heavy", accent);
    zetui_set_cell(ctx, bx,          y + 4, zetui_cell_make(zetui_box_heavy[ZETUI_BOX_LT], plain));
    zetui_draw_hline(ctx, bx + 1, y + 4, bw - 2, zetui_box_heavy[ZETUI_BOX_H], plain);
    zetui_set_cell(ctx, bx + bw - 1, y + 4, zetui_cell_make(zetui_box_heavy[ZETUI_BOX_RT], plain));

    /* Double */
    bx = x + (bw + 1) * 2;
    draw_box_table(ctx, bx, y + 1, bw, bh, zetui_box_double, plain);
    zetui_draw_str(ctx, bx + 2, y + 2, "double", accent);
    zetui_set_cell(ctx, bx,          y + 4, zetui_cell_make(zetui_box_double[ZETUI_BOX_LT], plain));
    zetui_draw_hline(ctx, bx + 1, y + 4, bw - 2, zetui_box_double[ZETUI_BOX_H], plain);
    zetui_set_cell(ctx, bx + bw - 1, y + 4, zetui_cell_make(zetui_box_double[ZETUI_BOX_RT], plain));

    /* -- hlines ---------------------------------------------------- */
    zetui_draw_str(ctx, x, y + 9, "zetui_draw_hline (light / heavy / double):", hdr);
    zetui_draw_hline(ctx, x, y + 10, w - 1, zetui_box_light[ZETUI_BOX_H],  plain);
    zetui_draw_hline(ctx, x, y + 11, w - 1, zetui_box_heavy[ZETUI_BOX_H],  plain);
    zetui_draw_hline(ctx, x, y + 12, w - 1, zetui_box_double[ZETUI_BOX_H], plain);

    /* -- vlines ---------------------------------------------------- */
    zetui_draw_str(ctx, x, y + 14, "zetui_draw_vline:", hdr);
    for (i = 0; i < 3; i++) {
        const zetui_u32 *t;
        t = (i == 0) ? zetui_box_light
          : (i == 1) ? zetui_box_heavy
          :             zetui_box_double;
        zetui_draw_vline(ctx, x + 18 + i * 2, y + 14, 5, t[ZETUI_BOX_V], plain);
    }

    /* -- fill_rect ------------------------------------------------- */
    zetui_draw_str(ctx, x, y + 14, "zetui_fill_rect:", hdr); /* overlaps vline label intentionally as demo */
    zetui_draw_str(ctx, x, y + 20, "zetui_fill_rect:", hdr);
    zetui_fill_rect(ctx, x + 17, y + 20, 18, 3, fill_cell);
    {
        zetui_style_t ts;
        ts = mk_style(ZETUI_COLOR_BRIGHT_WHITE, ZETUI_COLOR_GREEN, ZETUI_ATTR_BOLD);
        zetui_draw_str(ctx, x + 18, y + 21, "filled region", ts);
    }

    (void)h;
}

/* ================================================================== */
/*  Panel 4: Input events                                             */
/* ================================================================== */

static const char *key_name(zetui_key_t k)
{
    switch ((int)k) {
    case ZETUI_KEY_NONE:        return "NONE";
    case ZETUI_KEY_CTRL_A:      return "Ctrl-A";
    case ZETUI_KEY_CTRL_B:      return "Ctrl-B";
    case ZETUI_KEY_CTRL_C:      return "Ctrl-C";
    case ZETUI_KEY_CTRL_D:      return "Ctrl-D";
    case ZETUI_KEY_CTRL_E:      return "Ctrl-E";
    case ZETUI_KEY_CTRL_F:      return "Ctrl-F";
    case ZETUI_KEY_CTRL_G:      return "Ctrl-G";
    case ZETUI_KEY_BACKSPACE:   return "Backspace";
    case ZETUI_KEY_TAB:         return "Tab";
    case ZETUI_KEY_ENTER:       return "Enter";
    case ZETUI_KEY_CTRL_K:      return "Ctrl-K";
    case ZETUI_KEY_CTRL_L:      return "Ctrl-L";
    case ZETUI_KEY_CTRL_N:      return "Ctrl-N";
    case ZETUI_KEY_CTRL_P:      return "Ctrl-P";
    case ZETUI_KEY_CTRL_Q:      return "Ctrl-Q";
    case ZETUI_KEY_CTRL_R:      return "Ctrl-R";
    case ZETUI_KEY_CTRL_S:      return "Ctrl-S";
    case ZETUI_KEY_CTRL_U:      return "Ctrl-U";
    case ZETUI_KEY_CTRL_W:      return "Ctrl-W";
    case ZETUI_KEY_CTRL_X:      return "Ctrl-X";
    case ZETUI_KEY_CTRL_Y:      return "Ctrl-Y";
    case ZETUI_KEY_CTRL_Z:      return "Ctrl-Z";
    case ZETUI_KEY_ESC:         return "Escape";
    case ZETUI_KEY_DEL:         return "Del(127)";
    case ZETUI_KEY_ARROW_UP:    return "Arrow Up";
    case ZETUI_KEY_ARROW_DOWN:  return "Arrow Down";
    case ZETUI_KEY_ARROW_LEFT:  return "Arrow Left";
    case ZETUI_KEY_ARROW_RIGHT: return "Arrow Right";
    case ZETUI_KEY_HOME:        return "Home";
    case ZETUI_KEY_END:         return "End";
    case ZETUI_KEY_PAGE_UP:     return "Page Up";
    case ZETUI_KEY_PAGE_DOWN:   return "Page Down";
    case ZETUI_KEY_INSERT:      return "Insert";
    case ZETUI_KEY_DELETE:      return "Delete";
    case ZETUI_KEY_F1:          return "F1";
    case ZETUI_KEY_F2:          return "F2";
    case ZETUI_KEY_F3:          return "F3";
    case ZETUI_KEY_F4:          return "F4";
    case ZETUI_KEY_F5:          return "F5";
    case ZETUI_KEY_F6:          return "F6";
    case ZETUI_KEY_F7:          return "F7";
    case ZETUI_KEY_F8:          return "F8";
    case ZETUI_KEY_F9:          return "F9";
    case ZETUI_KEY_F10:         return "F10";
    case ZETUI_KEY_F11:         return "F11";
    case ZETUI_KEY_F12:         return "F12";
    default:                    return "unknown";
    }
}

/* UTF-8 encode cp into null-terminated buf (buf must be >= 5 bytes). */
static void cp_to_utf8(zetui_u32 cp, char *buf)
{
    if (cp < 0x80u) {
        buf[0] = (char)cp; buf[1] = '\0';
    } else if (cp < 0x800u) {
        buf[0] = (char)(0xC0u | (cp >> 6));
        buf[1] = (char)(0x80u | (cp & 0x3Fu));
        buf[2] = '\0';
    } else if (cp < 0x10000u) {
        buf[0] = (char)(0xE0u | (cp >> 12));
        buf[1] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
        buf[2] = (char)(0x80u | (cp & 0x3Fu));
        buf[3] = '\0';
    } else {
        buf[0] = (char)(0xF0u | (cp >> 18));
        buf[1] = (char)(0x80u | ((cp >> 12) & 0x3Fu));
        buf[2] = (char)(0x80u | ((cp >> 6)  & 0x3Fu));
        buf[3] = (char)(0x80u | (cp & 0x3Fu));
        buf[4] = '\0';
    }
}

static void panel_input(zetui_ctx_t *ctx, int x, int y, int w, int h,
                         zetui_event_t *last, int total)
{
    zetui_style_t hdr, lbl, val, dim;
    char          buf[64];
    int           pos, row;

    hdr = mk_style(ZETUI_COLOR_BRIGHT_WHITE, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_BOLD);
    lbl = mk_style(ZETUI_COLOR_YELLOW,       ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
    val = mk_style(ZETUI_COLOR_CYAN,         ZETUI_COLOR_DEFAULT, ZETUI_ATTR_BOLD);
    dim = mk_style(ZETUI_COLOR_BRIGHT_BLACK, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);

    zetui_draw_str(ctx, x, y, "Input event inspector", hdr);
    zetui_draw_str(ctx, x, y + 1,
                   "(Tab and 1-6 switch panels -- all other keys reported here)", dim);

    row = y + 3;

    if (last->type == ZETUI_EVENT_NONE) {
        zetui_draw_str(ctx, x, row, "No event yet -- press any key.", dim);
    } else if (last->type == ZETUI_EVENT_KEY) {
        zetui_key_event_t *ke;
        char               glyph[5];

        ke = &last->data.key;

        zetui_draw_str(ctx, x,      row, "event type  :", lbl);
        zetui_draw_str(ctx, x + 14, row, "KEY",           val);
        row++;

        /* Key name */
        zetui_draw_str(ctx, x, row, "key name    :", lbl);
        if (ke->ch >= 32u && ke->ch < 127u && ke->key == ZETUI_KEY_NONE) {
            buf[0] = (char)ke->ch; buf[1] = '\0';
        } else {
            const char *n;
            int         i;
            n = key_name(ke->key);
            for (i = 0; n[i] && i < 62; i++) buf[i] = n[i];
            buf[i] = '\0';
        }
        zetui_draw_str(ctx, x + 14, row, buf, val);
        row++;

        /* Key code */
        pos = 0; sb_int(buf, &pos, (int)ke->key);
        zetui_draw_str(ctx, x,      row, "key code    :", lbl);
        zetui_draw_str(ctx, x + 14, row, buf,             val);
        row++;

        /* Codepoint */
        pos = 0; sb_str(buf, &pos, "U+"); sb_hex(buf, &pos, ke->ch);
        zetui_draw_str(ctx, x,      row, "codepoint   :", lbl);
        zetui_draw_str(ctx, x + 14, row, buf,             val);
        row++;

        /* Modifiers */
        {
            const char *m;
            if      (ke->mods & ZETUI_MOD_CTRL)  m = "CTRL";
            else if (ke->mods & ZETUI_MOD_ALT)   m = "ALT";
            else if (ke->mods & ZETUI_MOD_SHIFT) m = "SHIFT";
            else                                  m = "none";
            zetui_draw_str(ctx, x,      row, "modifiers   :", lbl);
            zetui_draw_str(ctx, x + 14, row, m,               val);
        }
        row += 2;

        /* Visual glyph (using zetui_fill_rect + zetui_draw_str) */
        if (ke->ch >= 32u && ke->ch != 127u) {
            zetui_style_t box_sty, gly_sty;
            box_sty = mk_style(ZETUI_COLOR_DEFAULT, ZETUI_COLOR_BLUE,  ZETUI_ATTR_NONE);
            gly_sty = mk_style(ZETUI_COLOR_BRIGHT_WHITE, ZETUI_COLOR_BLUE, ZETUI_ATTR_BOLD);

            zetui_draw_str(ctx, x, row, "glyph:", lbl);
            zetui_fill_rect(ctx, x + 8, row - 1, 5, 3, zetui_cell_make((zetui_u32)' ', box_sty));
            cp_to_utf8(ke->ch, glyph);
            zetui_draw_str(ctx, x + 10, row, glyph, gly_sty);
            row += 2;
        }

        /* zetui_cursor_move demo: park cursor on the glyph box */
        zetui_cursor_move(ctx, x + 10, row - 2);
    } else if (last->type == ZETUI_EVENT_RESIZE) {
        zetui_draw_str(ctx, x,      row, "event type  :", lbl);
        zetui_draw_str(ctx, x + 14, row, "RESIZE",        val);
        row++;
        pos = 0;
        sb_int(buf, &pos, last->data.resize.width);
        sb_ch(buf, &pos, 'x');
        sb_int(buf, &pos, last->data.resize.height);
        zetui_draw_str(ctx, x,      row, "new size    :", lbl);
        zetui_draw_str(ctx, x + 14, row, buf,             val);
    }

    /* Event counter at the bottom */
    pos = 0; sb_str(buf, &pos, "total events: "); sb_int(buf, &pos, total);
    zetui_draw_str(ctx, x, y + h - 2, buf, dim);

    (void)w;
}

/* ================================================================== */
/*  Panel 5: UTF-8 characters via zetui_set_cell                     */
/* ================================================================== */

static void panel_utf8(zetui_ctx_t *ctx, int x, int y, int w, int h)
{
    static const zetui_u32 blocks[] = {
        0x2588u, 0x2587u, 0x2586u, 0x2585u, 0x2584u, 0x2583u, 0x2582u, 0x2581u,
        0x258Fu, 0x258Eu, 0x258Du, 0x258Cu, 0x258Bu, 0x258Au, 0x2589u, 0x25A0u
    };
    static const zetui_u32 misc[] = {
        0x2665u, 0x2666u, 0x2663u, 0x2660u,
        0x263Au, 0x263Bu, 0x2605u, 0x2606u,
        0x2764u, 0x2713u, 0x2717u, 0x2714u,
        0x2190u, 0x2192u, 0x2191u, 0x2193u,
        0x21D0u, 0x21D2u, 0x21D1u, 0x21D3u,
        0x00B7u, 0x2022u, 0x25CFu, 0x25CBu
    };
    static const zetui_u32 braille[] = {
        0x2800u, 0x2801u, 0x2802u, 0x2803u, 0x2804u, 0x2805u, 0x2806u, 0x2807u,
        0x2840u, 0x2841u, 0x2842u, 0x2843u, 0x2844u, 0x2845u, 0x2846u, 0x2847u
    };

    zetui_style_t hdr, s;
    int           nb, col, row;

    hdr = mk_style(ZETUI_COLOR_BRIGHT_WHITE, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_BOLD);
    row = y;

    /* Box-drawing all three tables */
    zetui_draw_str(ctx, x, row, "All box-drawing codepoints (light / heavy / double):", hdr);
    row++;
    {
        int          ti, ci;
        const zetui_u32 *tables[3];
        zetui_i32    fgs[3];
        tables[0] = zetui_box_light;  fgs[0] = ZETUI_COLOR_CYAN;
        tables[1] = zetui_box_heavy;  fgs[1] = ZETUI_COLOR_YELLOW;
        tables[2] = zetui_box_double; fgs[2] = ZETUI_COLOR_GREEN;
        for (ti = 0; ti < 3; ti++) {
            for (ci = 0; ci < 11; ci++) {
                s = mk_style(fgs[ti], ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
                zetui_set_cell(ctx, x + ci * 2, row, zetui_cell_make(tables[ti][ci], s));
            }
            row++;
        }
    }

    /* Block elements */
    row++;
    zetui_draw_str(ctx, x, row, "Block elements:", hdr);
    row++;
    nb = (int)(sizeof(blocks) / sizeof(blocks[0]));
    for (col = 0; col < nb; col++) {
        s = mk_style(ZETUI_COLOR_BRIGHT_CYAN, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
        zetui_set_cell(ctx, x + col * 2, row, zetui_cell_make(blocks[col], s));
    }

    /* Misc symbols */
    row += 2;
    zetui_draw_str(ctx, x, row, "Misc symbols:", hdr);
    row++;
    nb = (int)(sizeof(misc) / sizeof(misc[0]));
    for (col = 0; col < nb; col++) {
        s = mk_style(ZETUI_COLOR_BRIGHT_MAGENTA, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
        zetui_set_cell(ctx, x + col * 3, row, zetui_cell_make(misc[col], s));
    }

    /* Braille */
    row += 2;
    zetui_draw_str(ctx, x, row, "Braille patterns (U+2800-284F sample):", hdr);
    row++;
    nb = (int)(sizeof(braille) / sizeof(braille[0]));
    for (col = 0; col < nb; col++) {
        s = mk_style(ZETUI_COLOR_BRIGHT_WHITE, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
        zetui_set_cell(ctx, x + col * 2, row, zetui_cell_make(braille[col], s));
    }

    /* zetui_draw_str with multi-byte UTF-8 inline */
    row += 2;
    zetui_draw_str(ctx, x, row, "zetui_draw_str with inline UTF-8:", hdr);
    row++;
    s = mk_style(ZETUI_COLOR_BRIGHT_YELLOW, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
    zetui_draw_str(ctx, x, row, "\xe2\x94\x8c\xe2\x94\x80\xe2\x94\x90 "
                                "\xe2\x96\x88\xe2\x96\x84\xe2\x96\x80 "
                                "\xe2\x99\xa5 \xe2\x98\x85 \xe2\x9c\x93", s);

    (void)w; (void)h;
}

/* ================================================================== */
/*  Panel 6: Patterns (get_cell, fill, geometry)                      */
/* ================================================================== */

static void panel_patterns(zetui_ctx_t *ctx, int x, int y, int w, int h)
{
    zetui_style_t hdr, orig_sty, inv_sty;
    int           col, row, ci;

    hdr = mk_style(ZETUI_COLOR_BRIGHT_WHITE, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_BOLD);
    row = y;

    /* -- fill_rect colour gradient --------------------------------- */
    zetui_draw_str(ctx, x, row, "fill_rect gradient (8 solid colour bands):", hdr);
    row++;
    for (ci = 0; ci < 8; ci++) {
        zetui_style_t fs;
        zetui_cell_t  fc;
        fs = mk_style(ci, ci, ZETUI_ATTR_NONE);
        fc = zetui_cell_make((zetui_u32)' ', fs);
        zetui_fill_rect(ctx, x + ci * 5, row, 5, 2, fc);
    }
    row += 3;

    /* -- hlines from all three tables ------------------------------ */
    zetui_draw_str(ctx, x, row, "draw_hline (light / heavy / double):", hdr);
    row++;
    for (ci = 0; ci < 3; ci++) {
        const zetui_u32 *t;
        zetui_style_t    hs;
        t  = (ci == 0) ? zetui_box_light
           : (ci == 1) ? zetui_box_heavy : zetui_box_double;
        hs = mk_style(ZETUI_COLOR_BRIGHT_CYAN + ci, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
        zetui_draw_hline(ctx, x, row + ci, w - 1, t[ZETUI_BOX_H], hs);
    }
    row += 4;

    /* -- zetui_get_cell -> invert fg/bg then zetui_set_cell -------- */
    zetui_draw_str(ctx, x, row, "get_cell -> swap fg/bg -> set_cell:", hdr);
    row++;

    orig_sty = mk_style(ZETUI_COLOR_GREEN,   ZETUI_COLOR_BLUE, ZETUI_ATTR_NONE);
    inv_sty  = mk_style(ZETUI_COLOR_YELLOW,  ZETUI_COLOR_DEFAULT, ZETUI_ATTR_DIM);

    zetui_draw_str(ctx, x, row, "label:", inv_sty);
    zetui_draw_str(ctx, x + 7, row, "original (green on blue)", orig_sty);
    row++;
    zetui_draw_str(ctx, x, row, "label:", inv_sty);

    /* Read the "original" row and write inverted version one row below */
    for (col = x + 7; col < x + 7 + 24; col++) {
        zetui_cell_t src, inv;
        zetui_i32    tmp;
        src    = zetui_get_cell(ctx, col, row - 1); /* reads back buffer */
        inv    = src;
        tmp    = inv.fg;
        inv.fg = inv.bg;
        inv.bg = tmp;
        zetui_set_cell(ctx, col, row, inv);
    }
    row += 2;

    /* -- Cross-hatch via set_cell ---------------------------------- */
    zetui_draw_str(ctx, x, row, "set_cell cross-hatch pattern:", hdr);
    row++;
    for (ci = 0; ci < 6; ci++) {
        int cx;
        for (cx = 0; cx < 32; cx++) {
            zetui_cell_t  cell;
            zetui_style_t ps;
            zetui_u32     ch;
            ch = ((ci + cx) % 2 == 0) ? 0x2588u : 0x2591u;
            ps = mk_style((ci + cx) % 8, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
            cell = zetui_cell_make(ch, ps);
            zetui_set_cell(ctx, x + cx, row + ci, cell);
        }
    }

    (void)h;
}

/* ================================================================== */
/*  Chrome: outer frame, title bar, sidebar, status bar               */
/* ================================================================== */

static const char *panel_titles[NUM_PANELS] = {
    "Colors", "Attributes", "Box Styles", "Input", "UTF-8", "Patterns"
};

static void draw_chrome(zetui_ctx_t *ctx, int sel, int total,
                         zetui_event_t *last)
{
    zetui_style_t frame, title_bg, title_txt, tab_sel, tab_norm, dim_sty;
    int           w, h, i, pos;
    char          buf[80];

    w = zetui_width(ctx);
    h = zetui_height(ctx);

    frame     = mk_style(ZETUI_COLOR_BRIGHT_BLACK, ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
    title_bg  = mk_style(ZETUI_COLOR_DEFAULT,      ZETUI_COLOR_BLUE,    ZETUI_ATTR_NONE);
    title_txt = mk_style(ZETUI_COLOR_BRIGHT_WHITE,  ZETUI_COLOR_BLUE,   ZETUI_ATTR_BOLD);
    tab_sel   = mk_style(ZETUI_COLOR_BRIGHT_WHITE,  ZETUI_COLOR_BLUE,   ZETUI_ATTR_BOLD);
    tab_norm  = mk_style(ZETUI_COLOR_WHITE,         ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);
    dim_sty   = mk_style(ZETUI_COLOR_BRIGHT_BLACK,  ZETUI_COLOR_DEFAULT, ZETUI_ATTR_NONE);

    /* Outer heavy box */
    draw_box_table(ctx, 0, 0, w, h, zetui_box_heavy, frame);

    /* Title bar (row 1) */
    zetui_fill_rect(ctx, 1, 1, w - 2, 1, zetui_cell_make((zetui_u32)' ', title_bg));
    zetui_draw_str(ctx, 2, 1, "zetui feature demo", title_txt);
    pos = 0;
    sb_str(buf, &pos, "size:");
    sb_int(buf, &pos, w); sb_ch(buf, &pos, 'x'); sb_int(buf, &pos, h);
    zetui_draw_str(ctx, w - pos - 2, 1, buf, title_txt);

    /* Row 2: ┣━━━━┳━━━━┫ separator */
    zetui_set_cell(ctx, 0,          2, zetui_cell_make(zetui_box_heavy[ZETUI_BOX_LT], frame));
    zetui_draw_hline(ctx, 1, 2, SIDEBAR_W - 1, zetui_box_heavy[ZETUI_BOX_H], frame);
    zetui_set_cell(ctx, SIDEBAR_W,  2, zetui_cell_make(zetui_box_heavy[ZETUI_BOX_TT], frame));
    zetui_draw_hline(ctx, SIDEBAR_W + 1, 2, w - SIDEBAR_W - 2, zetui_box_heavy[ZETUI_BOX_H], frame);
    zetui_set_cell(ctx, w - 1,      2, zetui_cell_make(zetui_box_heavy[ZETUI_BOX_RT], frame));

    /* Vertical divider between sidebar and content */
    zetui_draw_vline(ctx, SIDEBAR_W, 3, h - 5, zetui_box_heavy[ZETUI_BOX_V], frame);

    /* Row h-2: ┣━━━━┻━━━━┫ separator above status */
    zetui_set_cell(ctx, 0,          h - 2, zetui_cell_make(zetui_box_heavy[ZETUI_BOX_LT], frame));
    zetui_draw_hline(ctx, 1, h - 2, SIDEBAR_W - 1, zetui_box_heavy[ZETUI_BOX_H], frame);
    zetui_set_cell(ctx, SIDEBAR_W,  h - 2, zetui_cell_make(zetui_box_heavy[ZETUI_BOX_BT], frame));
    zetui_draw_hline(ctx, SIDEBAR_W + 1, h - 2, w - SIDEBAR_W - 2, zetui_box_heavy[ZETUI_BOX_H], frame);
    zetui_set_cell(ctx, w - 1,      h - 2, zetui_cell_make(zetui_box_heavy[ZETUI_BOX_RT], frame));

    /* Sidebar panel list */
    for (i = 0; i < NUM_PANELS; i++) {
        zetui_style_t ms;
        char          entry[20];
        int           ei;

        ms = (i == sel) ? tab_sel : tab_norm;

        entry[0] = ' ';
        entry[1] = (char)('1' + i);
        entry[2] = ' ';
        for (ei = 3; panel_titles[i][ei - 3] && ei < SIDEBAR_W - 2; ei++)
            entry[ei] = panel_titles[i][ei - 3];
        while (ei < SIDEBAR_W - 1) entry[ei++] = ' ';
        entry[ei] = '\0';

        if (i == sel)
            zetui_fill_rect(ctx, 1, 3 + i * 2, SIDEBAR_W - 1, 1,
                            zetui_cell_make((zetui_u32)' ', ms));
        zetui_draw_str(ctx, 1, 3 + i * 2, entry, ms);
    }

    /* Status bar */
    zetui_draw_str(ctx, 1, h - 1, " q:quit  Tab/1-6:switch panel", dim_sty);

    pos = 0;
    sb_str(buf, &pos, "events:");
    sb_int(buf, &pos, total);
    zetui_draw_str(ctx, w - pos - 2, h - 1, buf, dim_sty);

    (void)last;
}

/* ================================================================== */
/*  Main loop                                                         */
/* ================================================================== */

int main(void)
{
    zetui_ctx_t  *ctx;
    zetui_event_t ev, last_ev;
    int           sel, running, total;
    int           cx, cy, cw, ch;

    ctx = zetui_init(); /* opens terminal, enters raw + alt-screen */
    if (!ctx) return 1;

    zetui_cursor_hide(ctx); /* hide cursor during normal navigation */

    sel     = 0;
    running = 1;
    total   = 0;
    memset(&last_ev, 0, sizeof(last_ev));

    while (running) {
        int w, h;

        w  = zetui_width(ctx);
        h  = zetui_height(ctx);
        cx = SIDEBAR_W + 1;      /* content origin x */
        cy = 3;                   /* content origin y */
        cw = w - SIDEBAR_W - 2;  /* content width    */
        ch = h - 5;              /* content height   */

        zetui_clear(ctx); /* wipe back buffer to default cells */
        draw_chrome(ctx, sel, total, &last_ev);

        switch (sel) {
        case 0: panel_colors(ctx,   cx, cy, cw, ch);             break;
        case 1: panel_attrs(ctx,    cx, cy, cw, ch);             break;
        case 2: panel_boxes(ctx,    cx, cy, cw, ch);             break;
        case 3: panel_input(ctx,    cx, cy, cw, ch,
                            &last_ev, total);                     break;
        case 4: panel_utf8(ctx,     cx, cy, cw, ch);             break;
        case 5: panel_patterns(ctx, cx, cy, cw, ch);             break;
        default: break;
        }

        /*
         * Re-show cursor only on the Input panel where we park it
         * on the glyph preview box.  All other panels hide it.
         */
        if (sel == 3 && last_ev.type == ZETUI_EVENT_KEY
                     && last_ev.data.key.ch >= 32u
                     && last_ev.data.key.ch != 127u) {
            zetui_cursor_show(ctx);
        } else {
            zetui_cursor_hide(ctx);
        }

        zetui_present(ctx); /* diff-render back buffer to terminal */

        /*
         * Block until the next event.
         * Uses zetui_wait_event (blocking).  zetui_poll_event
         * (non-blocking) is demonstrated in the loop guard below.
         */
        ev = zetui_wait_event(ctx, -1);
        total++;
        last_ev = ev;

        if (ev.type == ZETUI_EVENT_KEY) {
            zetui_key_event_t *ke;
            ke = &ev.data.key;

            if (ke->key == ZETUI_KEY_CTRL_C || ke->ch == (zetui_u32)'q')
                running = 0;

            if (ke->key == ZETUI_KEY_TAB)
                sel = (sel + 1) % NUM_PANELS;

            if (ke->ch >= (zetui_u32)'1' && ke->ch <= (zetui_u32)'6')
                sel = (int)(ke->ch - (zetui_u32)'1');
        }
        /* ZETUI_EVENT_RESIZE: just loop — zetui_width/height updated */

        /*
         * Drain any further events that arrived during rendering
         * (demonstrates zetui_poll_event in non-blocking mode).
         */
        {
            zetui_event_t extra;
            extra = zetui_poll_event(ctx);
            if (extra.type != ZETUI_EVENT_NONE) {
                total++;
                last_ev = extra;
            }
        }
    }

    zetui_cursor_show(ctx); /* restore cursor before exit */
    zetui_shutdown(ctx);    /* restore terminal, free memory */
    return 0;
}
