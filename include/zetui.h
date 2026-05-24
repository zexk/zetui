/*
 * zetui.h -- single-header ANSI/C89 TUI library (STB style)
 *
 * USAGE
 *   In exactly one translation unit:
 *       #define ZETUI_IMPLEMENTATION
 *       #include "zetui.h"
 *
 *   In every other translation unit (declarations only):
 *       #include "zetui.h"
 *
 * LICENSE: Unlicense (public domain)
 */

#ifndef ZETUI_H
#define ZETUI_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* ================================================================== */
    /*  Portability typedefs (no <stdint.h> in C89)                       */
    /* ================================================================== */

    typedef unsigned char zetui_u8;
    typedef unsigned short zetui_u16;
    typedef int zetui_i32;
    typedef unsigned int zetui_u32;

    /* ================================================================== */
    /*  Error codes                                                        */
    /* ================================================================== */

    typedef enum zetui_error
    {
        ZETUI_OK = 0,
        ZETUI_ERR_NOMEM = -1,
        ZETUI_ERR_IO = -2,
        ZETUI_ERR_NOT_A_TTY = -3,
        ZETUI_ERR_UNSUPPORTED = -4,
        ZETUI_ERR_TIMEOUT = -5
    } zetui_error_t;

    /* ================================================================== */
    /*  Colors                                                             */
    /* ================================================================== */

    typedef enum zetui_color
    {
        ZETUI_COLOR_DEFAULT = -1,
        ZETUI_COLOR_BLACK = 0,
        ZETUI_COLOR_RED = 1,
        ZETUI_COLOR_GREEN = 2,
        ZETUI_COLOR_YELLOW = 3,
        ZETUI_COLOR_BLUE = 4,
        ZETUI_COLOR_MAGENTA = 5,
        ZETUI_COLOR_CYAN = 6,
        ZETUI_COLOR_WHITE = 7,
        ZETUI_COLOR_BRIGHT_BLACK = 8,
        ZETUI_COLOR_BRIGHT_RED = 9,
        ZETUI_COLOR_BRIGHT_GREEN = 10,
        ZETUI_COLOR_BRIGHT_YELLOW = 11,
        ZETUI_COLOR_BRIGHT_BLUE = 12,
        ZETUI_COLOR_BRIGHT_MAGENTA = 13,
        ZETUI_COLOR_BRIGHT_CYAN = 14,
        ZETUI_COLOR_BRIGHT_WHITE = 15
    } zetui_color_t;

    /* ================================================================== */
    /*  Cell attributes                                                    */
    /* ================================================================== */

#define ZETUI_ATTR_NONE 0u
#define ZETUI_ATTR_BOLD (1u << 0)
#define ZETUI_ATTR_DIM (1u << 1)
#define ZETUI_ATTR_ITALIC (1u << 2)
#define ZETUI_ATTR_UNDERLINE (1u << 3)
#define ZETUI_ATTR_BLINK (1u << 4)
#define ZETUI_ATTR_REVERSE (1u << 5)
#define ZETUI_ATTR_HIDDEN (1u << 6)
#define ZETUI_ATTR_STRIKE (1u << 7)

    /* ================================================================== */
    /*  Cell                                                               */
    /* ================================================================== */

    typedef struct zetui_cell
    {
        zetui_u32 ch; /* Unicode codepoint; 0 treated as space */
        zetui_i32 fg; /* zetui_color_t or ZETUI_COLOR_DEFAULT  */
        zetui_i32 bg;
        zetui_u32 attrs;
    } zetui_cell_t;

    /* ================================================================== */
    /*  Style                                                              */
    /* ================================================================== */

    typedef struct zetui_style
    {
        zetui_i32 fg;
        zetui_i32 bg;
        zetui_u32 attrs;
    } zetui_style_t;

    /* ================================================================== */
    /*  Key codes                                                          */
    /* ================================================================== */

    typedef enum zetui_key
    {
        ZETUI_KEY_NONE = 0,
        ZETUI_KEY_CTRL_A = 1,
        ZETUI_KEY_CTRL_B = 2,
        ZETUI_KEY_CTRL_C = 3,
        ZETUI_KEY_CTRL_D = 4,
        ZETUI_KEY_CTRL_E = 5,
        ZETUI_KEY_CTRL_F = 6,
        ZETUI_KEY_CTRL_G = 7,
        ZETUI_KEY_BACKSPACE = 8,
        ZETUI_KEY_TAB = 9,
        ZETUI_KEY_ENTER = 13,
        ZETUI_KEY_CTRL_K = 11,
        ZETUI_KEY_CTRL_L = 12,
        ZETUI_KEY_CTRL_N = 14,
        ZETUI_KEY_CTRL_P = 16,
        ZETUI_KEY_CTRL_Q = 17,
        ZETUI_KEY_CTRL_R = 18,
        ZETUI_KEY_CTRL_S = 19,
        ZETUI_KEY_CTRL_U = 21,
        ZETUI_KEY_CTRL_W = 23,
        ZETUI_KEY_CTRL_X = 24,
        ZETUI_KEY_CTRL_Y = 25,
        ZETUI_KEY_CTRL_Z = 26,
        ZETUI_KEY_ESC = 27,
        ZETUI_KEY_DEL = 127,
        ZETUI_KEY_ARROW_UP = 256,
        ZETUI_KEY_ARROW_DOWN = 257,
        ZETUI_KEY_ARROW_LEFT = 258,
        ZETUI_KEY_ARROW_RIGHT = 259,
        ZETUI_KEY_HOME = 260,
        ZETUI_KEY_END = 261,
        ZETUI_KEY_PAGE_UP = 262,
        ZETUI_KEY_PAGE_DOWN = 263,
        ZETUI_KEY_INSERT = 264,
        ZETUI_KEY_DELETE = 265,
        ZETUI_KEY_F1 = 266,
        ZETUI_KEY_F2 = 267,
        ZETUI_KEY_F3 = 268,
        ZETUI_KEY_F4 = 269,
        ZETUI_KEY_F5 = 270,
        ZETUI_KEY_F6 = 271,
        ZETUI_KEY_F7 = 272,
        ZETUI_KEY_F8 = 273,
        ZETUI_KEY_F9 = 274,
        ZETUI_KEY_F10 = 275,
        ZETUI_KEY_F11 = 276,
        ZETUI_KEY_F12 = 277
    } zetui_key_t;

#define ZETUI_MOD_NONE 0u
#define ZETUI_MOD_SHIFT (1u << 0)
#define ZETUI_MOD_ALT (1u << 1)
#define ZETUI_MOD_CTRL (1u << 2)

    /* ================================================================== */
    /*  Events                                                             */
    /* ================================================================== */

    typedef enum zetui_event_type
    {
        ZETUI_EVENT_NONE = 0,
        ZETUI_EVENT_KEY = 1,
        ZETUI_EVENT_RESIZE = 2
    } zetui_event_type_t;

    typedef struct zetui_key_event
    {
        zetui_key_t key;
        zetui_u32 ch; /* Unicode codepoint for printable characters */
        zetui_u32 mods;
    } zetui_key_event_t;

    typedef struct zetui_resize_event
    {
        int width;
        int height;
    } zetui_resize_event_t;

    typedef union zetui_event_data
    {
        zetui_key_event_t key;
        zetui_resize_event_t resize;
    } zetui_event_data_t;

    typedef struct zetui_event
    {
        zetui_event_type_t type;
        zetui_event_data_t data;
    } zetui_event_t;

    /* ================================================================== */
    /*  Opaque context                                                     */
    /* ================================================================== */

    typedef struct zetui_ctx zetui_ctx_t;

    /* ================================================================== */
    /*  Box-drawing codepoint tables                                       */
    /* ================================================================== */

    extern const zetui_u32 zetui_box_light[11];
    extern const zetui_u32 zetui_box_heavy[11];
    extern const zetui_u32 zetui_box_double[11];

#define ZETUI_BOX_TL 0 /* top-left corner    ┌ */
#define ZETUI_BOX_TR 1 /* top-right corner   ┐ */
#define ZETUI_BOX_BL 2 /* bottom-left corner └ */
#define ZETUI_BOX_BR 3 /* bottom-right corner┘ */
#define ZETUI_BOX_H 4  /* horizontal line    ─ */
#define ZETUI_BOX_V 5  /* vertical line      │ */
#define ZETUI_BOX_LT 6 /* left T-junction    ├ */
#define ZETUI_BOX_RT 7 /* right T-junction   ┤ */
#define ZETUI_BOX_TT 8 /* top T-junction     ┬ */
#define ZETUI_BOX_BT 9 /* bottom T-junction  ┴ */
#define ZETUI_BOX_X 10 /* cross              ┼ */

    /* ================================================================== */
    /*  Public API                                                         */
    /* ================================================================== */

    /* --- Lifecycle ---------------------------------------------------- */

    zetui_ctx_t *zetui_init (void);
    void zetui_shutdown (zetui_ctx_t *ctx);

    /* --- Terminal info ------------------------------------------------ */

    int zetui_width (const zetui_ctx_t *ctx);
    int zetui_height (const zetui_ctx_t *ctx);

    /* --- Back-buffer drawing ------------------------------------------ */

    void zetui_clear (zetui_ctx_t *ctx);
    void zetui_set_cell (zetui_ctx_t *ctx, int x, int y, zetui_cell_t cell);
    zetui_cell_t zetui_get_cell (const zetui_ctx_t *ctx, int x, int y);
    zetui_error_t zetui_present (zetui_ctx_t *ctx);

    /* --- Input -------------------------------------------------------- */

    zetui_event_t zetui_poll_event (zetui_ctx_t *ctx);
    zetui_event_t zetui_wait_event (zetui_ctx_t *ctx, int timeout_ms);

    /* --- Drawing helpers ---------------------------------------------- */

    zetui_style_t zetui_style_default (void);
    zetui_cell_t zetui_cell_make (zetui_u32 ch, zetui_style_t style);

    void zetui_draw_str (zetui_ctx_t *ctx, int x, int y, const char *str,
                         zetui_style_t style);
    void zetui_draw_box (zetui_ctx_t *ctx, int x, int y, int w, int h,
                         zetui_style_t style);
    void zetui_fill_rect (zetui_ctx_t *ctx, int x, int y, int w, int h,
                          zetui_cell_t cell);
    void zetui_draw_hline (zetui_ctx_t *ctx, int x, int y, int len,
                           zetui_u32 ch, zetui_style_t style);
    void zetui_draw_vline (zetui_ctx_t *ctx, int x, int y, int len,
                           zetui_u32 ch, zetui_style_t style);

    /* --- Cursor ------------------------------------------------------- */

    void zetui_cursor_hide (zetui_ctx_t *ctx);
    void zetui_cursor_show (zetui_ctx_t *ctx);
    void zetui_cursor_move (zetui_ctx_t *ctx, int x, int y);

    /* ================================================================== */
    /*  IMPLEMENTATION                                                     */
    /* ================================================================== */

#ifdef ZETUI_IMPLEMENTATION

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

    /* ------------------------------------------------------------------ */
    /*  Internal: growable output buffer                                   */
    /* ------------------------------------------------------------------ */

    typedef struct zetui__obuf
    {
        char *data;
        size_t len;
        size_t cap;
    } zetui__obuf_t;

    static int
    zetui__obuf_init (zetui__obuf_t *ob, size_t cap)
    {
        ob->data = (char *)malloc (cap);
        if (!ob->data)
            return -1;
        ob->len = 0;
        ob->cap = cap;
        return 0;
    }

    static void
    zetui__obuf_free (zetui__obuf_t *ob)
    {
        free (ob->data);
        ob->data = NULL;
        ob->len = 0;
        ob->cap = 0;
    }

    static int
    zetui__obuf_append (zetui__obuf_t *ob, const char *data, size_t len)
    {
        char *nd;
        size_t nc;

        if (ob->len + len > ob->cap)
            {
                nc = ob->cap * 2;
                if (nc < ob->len + len)
                    nc = ob->len + len + 4096u;
                nd = (char *)realloc (ob->data, nc);
                if (!nd)
                    return -1;
                ob->data = nd;
                ob->cap = nc;
            }
        memcpy (ob->data + ob->len, data, len);
        ob->len += len;
        return 0;
    }

    static int
    zetui__obuf_append_str (zetui__obuf_t *ob, const char *s)
    {
        return zetui__obuf_append (ob, s, strlen (s));
    }

    static int
    zetui__obuf_append_int (zetui__obuf_t *ob, int n)
    {
        char tmp[12];
        char buf[13];
        int i, len;

        if (n == 0)
            return zetui__obuf_append (ob, "0", 1u);

        i = 0;
        while (n > 0)
            {
                tmp[i++] = (char)('0' + (n % 10));
                n /= 10;
            }
        len = 0;
        while (i > 0)
            buf[len++] = tmp[--i];
        return zetui__obuf_append (ob, buf, (size_t)len);
    }

    static int
    zetui__obuf_flush (zetui__obuf_t *ob, int fd)
    {
        size_t written;
        ssize_t n;

        written = 0u;
        while (written < ob->len)
            {
                n = write (fd, ob->data + written, ob->len - written);
                if (n < 0)
                    {
                        if (errno == EINTR)
                            continue;
                        return -1;
                    }
                written += (size_t)n;
            }
        ob->len = 0u;
        return 0;
    }

    /* ------------------------------------------------------------------ */
    /*  Internal: full context definition                                  */
    /* ------------------------------------------------------------------ */

    struct zetui_ctx
    {
        int fd_in;
        int fd_out;

        struct termios saved_termios;

        int width;
        int height;

        zetui_cell_t *front; /* currently visible on the terminal */
        zetui_cell_t *back;  /* written to by draw calls          */
        size_t ncells;       /* width * height                    */

        zetui__obuf_t out;

        /* Rendering state carried across present() calls */
        zetui_i32 ren_fg;
        zetui_i32 ren_bg;
        zetui_u32 ren_attrs;
        int ren_x;
        int ren_y;

        /* Logical cursor state */
        int cursor_visible;
        int cursor_x;
        int cursor_y;

        /* Set by SIGWINCH handler */
        int resize_pending;
    };

    /* ------------------------------------------------------------------ */
    /*  SIGWINCH                                                           */
    /* ------------------------------------------------------------------ */

    static volatile sig_atomic_t zetui__resize_flag = 0;

    static void
    zetui__sigwinch (int sig)
    {
        (void)sig;
        zetui__resize_flag = 1;
    }

    /* ------------------------------------------------------------------ */
    /*  Box-drawing codepoint tables                                       */
    /* ------------------------------------------------------------------ */

    const zetui_u32 zetui_box_light[11] = {
        0x250Cu, 0x2510u, 0x2514u, 0x2518u, /* TL TR BL BR */
        0x2500u, 0x2502u,                   /* H  V        */
        0x251Cu, 0x2524u, 0x252Cu, 0x2534u, /* LT RT TT BT */
        0x253Cu                             /* X           */
    };

    const zetui_u32 zetui_box_heavy[11]
        = { 0x250Fu, 0x2513u, 0x2517u, 0x251Bu, 0x2501u, 0x2503u,
            0x2523u, 0x252Bu, 0x2533u, 0x253Bu, 0x254Bu };

    const zetui_u32 zetui_box_double[11]
        = { 0x2554u, 0x2557u, 0x255Au, 0x255Du, 0x2550u, 0x2551u,
            0x2560u, 0x2563u, 0x2566u, 0x2569u, 0x256Cu };

    /* ------------------------------------------------------------------ */
    /*  UTF-8 encode/decode                                                */
    /* ------------------------------------------------------------------ */

    static size_t
    zetui__utf8_enc (zetui_u32 cp, char *buf)
    {
        if (cp < 0x80u)
            {
                buf[0] = (char)cp;
                return 1u;
            }
        else if (cp < 0x800u)
            {
                buf[0] = (char)(0xC0u | (cp >> 6));
                buf[1] = (char)(0x80u | (cp & 0x3Fu));
                return 2u;
            }
        else if (cp < 0x10000u)
            {
                buf[0] = (char)(0xE0u | (cp >> 12));
                buf[1] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
                buf[2] = (char)(0x80u | (cp & 0x3Fu));
                return 3u;
            }
        else
            {
                buf[0] = (char)(0xF0u | (cp >> 18));
                buf[1] = (char)(0x80u | ((cp >> 12) & 0x3Fu));
                buf[2] = (char)(0x80u | ((cp >> 6) & 0x3Fu));
                buf[3] = (char)(0x80u | (cp & 0x3Fu));
                return 4u;
            }
    }

    static int
    zetui__utf8_dec (const unsigned char *buf, size_t len, zetui_u32 *out)
    {
        if (len == 0u)
            {
                *out = 0u;
                return 0;
            }

        if ((buf[0] & 0x80u) == 0u)
            {
                *out = buf[0];
                return 1;
            }
        if ((buf[0] & 0xE0u) == 0xC0u && len >= 2u)
            {
                *out = ((zetui_u32)(buf[0] & 0x1Fu) << 6)
                       | (zetui_u32)(buf[1] & 0x3Fu);
                return 2;
            }
        if ((buf[0] & 0xF0u) == 0xE0u && len >= 3u)
            {
                *out = ((zetui_u32)(buf[0] & 0x0Fu) << 12)
                       | ((zetui_u32)(buf[1] & 0x3Fu) << 6)
                       | (zetui_u32)(buf[2] & 0x3Fu);
                return 3;
            }
        if ((buf[0] & 0xF8u) == 0xF0u && len >= 4u)
            {
                *out = ((zetui_u32)(buf[0] & 0x07u) << 18)
                       | ((zetui_u32)(buf[1] & 0x3Fu) << 12)
                       | ((zetui_u32)(buf[2] & 0x3Fu) << 6)
                       | (zetui_u32)(buf[3] & 0x3Fu);
                return 4;
            }
        *out = buf[0]; /* malformed: pass through */
        return 1;
    }

    /* ------------------------------------------------------------------ */
    /*  ANSI escape helpers                                                */
    /* ------------------------------------------------------------------ */

    static int
    zetui__ansi_move (zetui__obuf_t *ob, int x, int y)
    {
        zetui__obuf_append_str (ob, "\033[");
        zetui__obuf_append_int (ob, y + 1);
        zetui__obuf_append (ob, ";", 1u);
        zetui__obuf_append_int (ob, x + 1);
        return zetui__obuf_append (ob, "H", 1u);
    }

    static int
    zetui__ansi_style (zetui__obuf_t *ob, zetui_i32 fg, zetui_i32 bg,
                       zetui_u32 attrs, zetui_i32 *c_fg, zetui_i32 *c_bg,
                       zetui_u32 *c_attrs)
    {
        zetui_u32 turned_off;
        int need_reset;

        turned_off = (*c_attrs) & ~attrs;
        need_reset = 0;

        if (turned_off)
            need_reset = 1;
        if (fg == ZETUI_COLOR_DEFAULT && *c_fg != ZETUI_COLOR_DEFAULT)
            need_reset = 1;
        if (bg == ZETUI_COLOR_DEFAULT && *c_bg != ZETUI_COLOR_DEFAULT)
            need_reset = 1;

        if (need_reset)
            {
                zetui__obuf_append_str (ob, "\033[0m");
                *c_fg = ZETUI_COLOR_DEFAULT;
                *c_bg = ZETUI_COLOR_DEFAULT;
                *c_attrs = ZETUI_ATTR_NONE;
            }

#define ZETUI__ATTR_ON(flag, code)                                             \
    if ((attrs & (flag)) && !(*c_attrs & (flag)))                              \
    zetui__obuf_append_str (ob, "\033[" code "m")

        ZETUI__ATTR_ON (ZETUI_ATTR_BOLD, "1");
        ZETUI__ATTR_ON (ZETUI_ATTR_DIM, "2");
        ZETUI__ATTR_ON (ZETUI_ATTR_ITALIC, "3");
        ZETUI__ATTR_ON (ZETUI_ATTR_UNDERLINE, "4");
        ZETUI__ATTR_ON (ZETUI_ATTR_BLINK, "5");
        ZETUI__ATTR_ON (ZETUI_ATTR_REVERSE, "7");
        ZETUI__ATTR_ON (ZETUI_ATTR_HIDDEN, "8");
        ZETUI__ATTR_ON (ZETUI_ATTR_STRIKE, "9");

#undef ZETUI__ATTR_ON

        *c_attrs = attrs;

        if (fg != *c_fg)
            {
                if (fg == ZETUI_COLOR_DEFAULT)
                    {
                        zetui__obuf_append_str (ob, "\033[39m");
                    }
                else if (fg < 8)
                    {
                        zetui__obuf_append_str (ob, "\033[");
                        zetui__obuf_append_int (ob, 30 + fg);
                        zetui__obuf_append (ob, "m", 1u);
                    }
                else
                    {
                        zetui__obuf_append_str (ob, "\033[");
                        zetui__obuf_append_int (ob, 90 + (fg - 8));
                        zetui__obuf_append (ob, "m", 1u);
                    }
                *c_fg = fg;
            }

        if (bg != *c_bg)
            {
                if (bg == ZETUI_COLOR_DEFAULT)
                    {
                        zetui__obuf_append_str (ob, "\033[49m");
                    }
                else if (bg < 8)
                    {
                        zetui__obuf_append_str (ob, "\033[");
                        zetui__obuf_append_int (ob, 40 + bg);
                        zetui__obuf_append (ob, "m", 1u);
                    }
                else
                    {
                        zetui__obuf_append_str (ob, "\033[");
                        zetui__obuf_append_int (ob, 100 + (bg - 8));
                        zetui__obuf_append (ob, "m", 1u);
                    }
                *c_bg = bg;
            }

        return 0;
    }

    /* ------------------------------------------------------------------ */
    /*  Screen buffer                                                      */
    /* ------------------------------------------------------------------ */

    static zetui_cell_t
    zetui__blank_cell (void)
    {
        zetui_cell_t c;
        c.ch = (zetui_u32)' ';
        c.fg = ZETUI_COLOR_DEFAULT;
        c.bg = ZETUI_COLOR_DEFAULT;
        c.attrs = ZETUI_ATTR_NONE;
        return c;
    }

    static int
    zetui__cells_eq (zetui_cell_t a, zetui_cell_t b)
    {
        return a.ch == b.ch && a.fg == b.fg && a.bg == b.bg
               && a.attrs == b.attrs;
    }

    static int
    zetui__screen_resize (zetui_ctx_t *ctx, int w, int h)
    {
        zetui_cell_t *nf, *nb;
        zetui_cell_t blank;
        size_t n, i;

        n = (size_t)(w * h);
        nf = (zetui_cell_t *)malloc (n * sizeof (zetui_cell_t));
        nb = (zetui_cell_t *)malloc (n * sizeof (zetui_cell_t));

        if (!nf || !nb)
            {
                free (nf);
                free (nb);
                return -1;
            }

        blank = zetui__blank_cell ();
        for (i = 0u; i < n; i++)
            {
                nf[i] = blank;
                nb[i] = blank;
            }

        free (ctx->front);
        free (ctx->back);

        ctx->front = nf;
        ctx->back = nb;
        ctx->ncells = n;
        ctx->width = w;
        ctx->height = h;
        return 0;
    }

    void
    zetui_clear (zetui_ctx_t *ctx)
    {
        zetui_cell_t blank;
        size_t i;

        blank = zetui__blank_cell ();
        for (i = 0u; i < ctx->ncells; i++)
            ctx->back[i] = blank;
    }

    void
    zetui_set_cell (zetui_ctx_t *ctx, int x, int y, zetui_cell_t cell)
    {
        if (x < 0 || y < 0 || x >= ctx->width || y >= ctx->height)
            return;
        ctx->back[y * ctx->width + x] = cell;
    }

    zetui_cell_t
    zetui_get_cell (const zetui_ctx_t *ctx, int x, int y)
    {
        if (x < 0 || y < 0 || x >= ctx->width || y >= ctx->height)
            return zetui__blank_cell ();
        return ctx->back[y * ctx->width + x];
    }

    zetui_error_t
    zetui_present (zetui_ctx_t *ctx)
    {
        int x, y;
        zetui_cell_t *f, *b;
        char utf8[4];
        size_t ulen;

        ctx->ren_fg = ZETUI_COLOR_DEFAULT;
        ctx->ren_bg = ZETUI_COLOR_DEFAULT;
        ctx->ren_attrs = ZETUI_ATTR_NONE;
        ctx->ren_x = -1;
        ctx->ren_y = -1;

        ctx->out.len = 0u;
        zetui__obuf_append_str (&ctx->out, "\033[?25l");

        for (y = 0; y < ctx->height; y++)
            {
                for (x = 0; x < ctx->width; x++)
                    {
                        b = &ctx->back[y * ctx->width + x];
                        f = &ctx->front[y * ctx->width + x];

                        if (zetui__cells_eq (*b, *f))
                            continue;

                        if (ctx->ren_x != x || ctx->ren_y != y)
                            {
                                zetui__ansi_move (&ctx->out, x, y);
                                ctx->ren_x = x;
                                ctx->ren_y = y;
                            }

                        zetui__ansi_style (&ctx->out, b->fg, b->bg, b->attrs,
                                           &ctx->ren_fg, &ctx->ren_bg,
                                           &ctx->ren_attrs);

                        ulen = zetui__utf8_enc (
                            b->ch == 0u ? (zetui_u32)' ' : b->ch, utf8);
                        zetui__obuf_append (&ctx->out, utf8, ulen);

                        ctx->ren_x++;
                        *f = *b;
                    }
            }

        if (ctx->cursor_visible)
            {
                zetui__ansi_move (&ctx->out, ctx->cursor_x, ctx->cursor_y);
                zetui__obuf_append_str (&ctx->out, "\033[?25h");
            }

        return zetui__obuf_flush (&ctx->out, ctx->fd_out) == 0 ? ZETUI_OK
                                                               : ZETUI_ERR_IO;
    }

    /* ------------------------------------------------------------------ */
    /*  Terminal lifecycle                                                 */
    /* ------------------------------------------------------------------ */

    static int
    zetui__enter_raw (int fd, struct termios *saved)
    {
        struct termios raw;

        if (tcgetattr (fd, saved) == -1)
            return -1;

        raw = *saved;
        raw.c_iflag &= (tcflag_t) ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag &= (tcflag_t)~OPOST;
        raw.c_cflag |= CS8;
        raw.c_lflag &= (tcflag_t) ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;

        return tcsetattr (fd, TCSAFLUSH, &raw);
    }

    static void
    zetui__leave_raw (int fd, const struct termios *saved)
    {
        tcsetattr (fd, TCSAFLUSH, saved);
    }

    static int
    zetui__query_size (int fd, int *w, int *h)
    {
        struct winsize ws;
        if (ioctl (fd, TIOCGWINSZ, &ws) == -1)
            return -1;
        *w = (int)ws.ws_col;
        *h = (int)ws.ws_row;
        return 0;
    }

    zetui_ctx_t *
    zetui_init (void)
    {
        zetui_ctx_t *ctx;
        int w, h;

        ctx = (zetui_ctx_t *)calloc (1u, sizeof (zetui_ctx_t));
        if (!ctx)
            return NULL;

        ctx->fd_in = STDIN_FILENO;
        ctx->fd_out = STDOUT_FILENO;

        if (!isatty (ctx->fd_out))
            {
                free (ctx);
                return NULL;
            }

        if (zetui__enter_raw (ctx->fd_in, &ctx->saved_termios) == -1)
            {
                free (ctx);
                return NULL;
            }

        if (zetui__query_size (ctx->fd_out, &w, &h) == -1)
            {
                zetui__leave_raw (ctx->fd_in, &ctx->saved_termios);
                free (ctx);
                return NULL;
            }

        if (zetui__obuf_init (&ctx->out, 65536u) == -1)
            {
                zetui__leave_raw (ctx->fd_in, &ctx->saved_termios);
                free (ctx);
                return NULL;
            }

        if (zetui__screen_resize (ctx, w, h) == -1)
            {
                zetui__obuf_free (&ctx->out);
                zetui__leave_raw (ctx->fd_in, &ctx->saved_termios);
                free (ctx);
                return NULL;
            }

        ctx->cursor_visible = 1;
        ctx->cursor_x = 0;
        ctx->cursor_y = 0;

        signal (SIGWINCH, zetui__sigwinch);

        write (ctx->fd_out, "\033[?1049h\033[?25l", 14);

        return ctx;
    }

    void
    zetui_shutdown (zetui_ctx_t *ctx)
    {
        if (!ctx)
            return;

        signal (SIGWINCH, SIG_DFL);
        write (ctx->fd_out, "\033[?25h\033[?1049l", 14);
        zetui__leave_raw (ctx->fd_in, &ctx->saved_termios);

        free (ctx->front);
        free (ctx->back);
        zetui__obuf_free (&ctx->out);
        free (ctx);
    }

    int
    zetui_width (const zetui_ctx_t *ctx)
    {
        return ctx->width;
    }
    int
    zetui_height (const zetui_ctx_t *ctx)
    {
        return ctx->height;
    }

    /* ------------------------------------------------------------------ */
    /*  Cursor                                                             */
    /* ------------------------------------------------------------------ */

    void
    zetui_cursor_hide (zetui_ctx_t *ctx)
    {
        ctx->cursor_visible = 0;
    }
    void
    zetui_cursor_show (zetui_ctx_t *ctx)
    {
        ctx->cursor_visible = 1;
    }
    void
    zetui_cursor_move (zetui_ctx_t *ctx, int x, int y)
    {
        ctx->cursor_x = x;
        ctx->cursor_y = y;
    }

    /* ------------------------------------------------------------------ */
    /*  Input                                                              */
    /* ------------------------------------------------------------------ */

    typedef struct
    {
        const char *seq;
        zetui_key_t key;
    } zetui__esc_entry_t;

    static const zetui__esc_entry_t zetui__esc_table[]
        = { { "\033[A", ZETUI_KEY_ARROW_UP },
            { "\033[B", ZETUI_KEY_ARROW_DOWN },
            { "\033[C", ZETUI_KEY_ARROW_RIGHT },
            { "\033[D", ZETUI_KEY_ARROW_LEFT },
            { "\033[H", ZETUI_KEY_HOME },
            { "\033[F", ZETUI_KEY_END },
            { "\033[1~", ZETUI_KEY_HOME },
            { "\033[4~", ZETUI_KEY_END },
            { "\033[5~", ZETUI_KEY_PAGE_UP },
            { "\033[6~", ZETUI_KEY_PAGE_DOWN },
            { "\033[2~", ZETUI_KEY_INSERT },
            { "\033[3~", ZETUI_KEY_DELETE },
            { "\033OP", ZETUI_KEY_F1 },
            { "\033OQ", ZETUI_KEY_F2 },
            { "\033OR", ZETUI_KEY_F3 },
            { "\033OS", ZETUI_KEY_F4 },
            { "\033[15~", ZETUI_KEY_F5 },
            { "\033[17~", ZETUI_KEY_F6 },
            { "\033[18~", ZETUI_KEY_F7 },
            { "\033[19~", ZETUI_KEY_F8 },
            { "\033[20~", ZETUI_KEY_F9 },
            { "\033[21~", ZETUI_KEY_F10 },
            { "\033[23~", ZETUI_KEY_F11 },
            { "\033[24~", ZETUI_KEY_F12 },
            { "\033OA", ZETUI_KEY_ARROW_UP },
            { "\033OB", ZETUI_KEY_ARROW_DOWN },
            { "\033OC", ZETUI_KEY_ARROW_RIGHT },
            { "\033OD", ZETUI_KEY_ARROW_LEFT },
            { NULL, ZETUI_KEY_NONE } };

#define ZETUI__IBUF 64

    typedef struct
    {
        unsigned char data[ZETUI__IBUF];
        int len;
        int pos;
    } zetui__ibuf_t;

    static int
    zetui__ibuf_fill (zetui__ibuf_t *ib, int fd, int timeout_ms)
    {
        fd_set rfds;
        struct timeval tv;
        struct timeval *tvp;
        int ret;

        FD_ZERO (&rfds);
        FD_SET (fd, &rfds);

        if (timeout_ms < 0)
            {
                tvp = NULL;
            }
        else
            {
                tv.tv_sec = timeout_ms / 1000;
                tv.tv_usec = (timeout_ms % 1000) * 1000;
                tvp = &tv;
            }

        ret = select (fd + 1, &rfds, NULL, NULL, tvp);
        if (ret <= 0)
            return ret;

        ib->len = (int)read (fd, ib->data, ZETUI__IBUF);
        ib->pos = 0;
        return ib->len > 0 ? ib->len : -1;
    }

    static zetui_event_t
    zetui__parse (zetui__ibuf_t *ib)
    {
        zetui_event_t ev;
        unsigned char c;
        const zetui__esc_entry_t *e;
        zetui_u32 cp;
        int bytes;

        memset (&ev, 0, sizeof (ev));
        ev.type = ZETUI_EVENT_NONE;

        if (ib->pos >= ib->len)
            return ev;

        c = ib->data[ib->pos];

        if (c == 0x1Bu && ib->len - ib->pos > 1)
            {
                int rem;
                const char *buf;

                rem = ib->len - ib->pos;
                buf = (const char *)(ib->data + ib->pos);

                for (e = zetui__esc_table; e->seq != NULL; e++)
                    {
                        int slen;
                        slen = (int)strlen (e->seq);
                        if (slen <= rem
                            && memcmp (buf, e->seq, (size_t)slen) == 0)
                            {
                                ev.type = ZETUI_EVENT_KEY;
                                ev.data.key.key = e->key;
                                ev.data.key.ch = 0u;
                                ev.data.key.mods = ZETUI_MOD_NONE;
                                ib->pos += slen;
                                return ev;
                            }
                    }

                ev.type = ZETUI_EVENT_KEY;
                ev.data.key.key = ZETUI_KEY_ESC;
                ev.data.key.ch = 0x1Bu;
                ev.data.key.mods = ZETUI_MOD_NONE;
                ib->pos++;
                return ev;
            }

        bytes = zetui__utf8_dec (ib->data + ib->pos,
                                 (size_t)(ib->len - ib->pos), &cp);

        if (bytes > 0 && cp >= 32u && cp != 127u)
            {
                ev.type = ZETUI_EVENT_KEY;
                ev.data.key.key = ZETUI_KEY_NONE;
                ev.data.key.ch = cp;
                ev.data.key.mods = ZETUI_MOD_NONE;
                ib->pos += bytes;
                return ev;
            }

        ev.type = ZETUI_EVENT_KEY;
        ev.data.key.key = (zetui_key_t)c;
        ev.data.key.ch = c;
        ev.data.key.mods = (c < 32u) ? ZETUI_MOD_CTRL : ZETUI_MOD_NONE;
        ib->pos++;
        return ev;
    }

    static zetui_event_t
    zetui__resize_ev (zetui_ctx_t *ctx)
    {
        zetui_event_t ev;
        int w, h;

        zetui__resize_flag = 0;
        if (zetui__query_size (ctx->fd_out, &w, &h) == 0)
            zetui__screen_resize (ctx, w, h);

        ev.type = ZETUI_EVENT_RESIZE;
        ev.data.resize.width = ctx->width;
        ev.data.resize.height = ctx->height;
        return ev;
    }

    zetui_event_t
    zetui_poll_event (zetui_ctx_t *ctx)
    {
        zetui__ibuf_t ib;
        zetui_event_t none;

        if (zetui__resize_flag)
            return zetui__resize_ev (ctx);

        memset (&ib, 0, sizeof (ib));
        if (zetui__ibuf_fill (&ib, ctx->fd_in, 0) <= 0)
            {
                memset (&none, 0, sizeof (none));
                none.type = ZETUI_EVENT_NONE;
                return none;
            }
        return zetui__parse (&ib);
    }

    zetui_event_t
    zetui_wait_event (zetui_ctx_t *ctx, int timeout_ms)
    {
        zetui__ibuf_t ib;
        zetui_event_t none;
        int ret;

        if (zetui__resize_flag)
            return zetui__resize_ev (ctx);

        memset (&ib, 0, sizeof (ib));
        ret = zetui__ibuf_fill (&ib, ctx->fd_in, timeout_ms);

        if (ret <= 0)
            {
                if (zetui__resize_flag)
                    return zetui__resize_ev (ctx);
                memset (&none, 0, sizeof (none));
                none.type = ZETUI_EVENT_NONE;
                return none;
            }
        return zetui__parse (&ib);
    }

    /* ------------------------------------------------------------------ */
    /*  Drawing                                                            */
    /* ------------------------------------------------------------------ */

    zetui_style_t
    zetui_style_default (void)
    {
        zetui_style_t s;
        s.fg = ZETUI_COLOR_DEFAULT;
        s.bg = ZETUI_COLOR_DEFAULT;
        s.attrs = ZETUI_ATTR_NONE;
        return s;
    }

    zetui_cell_t
    zetui_cell_make (zetui_u32 ch, zetui_style_t style)
    {
        zetui_cell_t c;
        c.ch = ch;
        c.fg = style.fg;
        c.bg = style.bg;
        c.attrs = style.attrs;
        return c;
    }

    void
    zetui_draw_str (zetui_ctx_t *ctx, int x, int y, const char *str,
                    zetui_style_t style)
    {
        const unsigned char *p;
        zetui_u32 cp;
        int bytes, cx;

        if (!str)
            return;
        p = (const unsigned char *)str;
        cx = x;

        while (*p && cx < ctx->width)
            {
                bytes = zetui__utf8_dec (p, 4u, &cp);
                if (bytes <= 0)
                    break;
                if (cp == (zetui_u32)'\n')
                    break;
                zetui_set_cell (ctx, cx, y, zetui_cell_make (cp, style));
                cx++;
                p += bytes;
            }
    }

    void
    zetui_draw_hline (zetui_ctx_t *ctx, int x, int y, int len, zetui_u32 ch,
                      zetui_style_t style)
    {
        int i;
        for (i = 0; i < len; i++)
            zetui_set_cell (ctx, x + i, y, zetui_cell_make (ch, style));
    }

    void
    zetui_draw_vline (zetui_ctx_t *ctx, int x, int y, int len, zetui_u32 ch,
                      zetui_style_t style)
    {
        int i;
        for (i = 0; i < len; i++)
            zetui_set_cell (ctx, x, y + i, zetui_cell_make (ch, style));
    }

    void
    zetui_draw_box (zetui_ctx_t *ctx, int x, int y, int w, int h,
                    zetui_style_t style)
    {
        zetui_cell_t c;
        int i;

        if (w < 2 || h < 2)
            return;

        c = zetui_cell_make (zetui_box_light[ZETUI_BOX_TL], style);
        zetui_set_cell (ctx, x, y, c);
        c = zetui_cell_make (zetui_box_light[ZETUI_BOX_TR], style);
        zetui_set_cell (ctx, x + w - 1, y, c);
        c = zetui_cell_make (zetui_box_light[ZETUI_BOX_BL], style);
        zetui_set_cell (ctx, x, y + h - 1, c);
        c = zetui_cell_make (zetui_box_light[ZETUI_BOX_BR], style);
        zetui_set_cell (ctx, x + w - 1, y + h - 1, c);

        c = zetui_cell_make (zetui_box_light[ZETUI_BOX_H], style);
        for (i = 1; i < w - 1; i++)
            {
                zetui_set_cell (ctx, x + i, y, c);
                zetui_set_cell (ctx, x + i, y + h - 1, c);
            }

        c = zetui_cell_make (zetui_box_light[ZETUI_BOX_V], style);
        for (i = 1; i < h - 1; i++)
            {
                zetui_set_cell (ctx, x, y + i, c);
                zetui_set_cell (ctx, x + w - 1, y + i, c);
            }
    }

    void
    zetui_fill_rect (zetui_ctx_t *ctx, int x, int y, int w, int h,
                     zetui_cell_t cell)
    {
        int row, col;
        for (row = 0; row < h; row++)
            for (col = 0; col < w; col++)
                zetui_set_cell (ctx, x + col, y + row, cell);
    }

#endif /* ZETUI_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* ZETUI_H */
