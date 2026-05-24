/*
 * hello.c — minimal C example
 * Build: zig build run-hello-c
 */

#include "zetui.h"
#include <string.h>

int
main (void)
{
    zetui_ctx_t *ctx;
    zetui_event_t ev;
    zetui_style_t normal;
    zetui_style_t title;
    zetui_style_t dim;
    char info[64];
    int w, h, running;

    ctx = zetui_init ();
    if (!ctx)
        return 1;

    normal = zetui_style_default ();

    title.fg = ZETUI_COLOR_CYAN;
    title.bg = ZETUI_COLOR_DEFAULT;
    title.attrs = ZETUI_ATTR_BOLD;

    dim.fg = ZETUI_COLOR_WHITE;
    dim.bg = ZETUI_COLOR_DEFAULT;
    dim.attrs = ZETUI_ATTR_DIM;

    zetui_cursor_hide (ctx);
    running = 1;

    while (running)
        {
            w = zetui_width (ctx);
            h = zetui_height (ctx);

            zetui_clear (ctx);

            /* Outer border */
            zetui_draw_box (ctx, 0, 0, w, h, normal);

            /* Title bar */
            {
                zetui_cell_t bar_cell;
                zetui_style_t bar_style;
                int i;
                bar_style.fg = ZETUI_COLOR_DEFAULT;
                bar_style.bg = ZETUI_COLOR_BLUE;
                bar_style.attrs = ZETUI_ATTR_NONE;
                bar_cell = zetui_cell_make ((zetui_u32)' ', bar_style);
                for (i = 1; i < w - 1; i++)
                    zetui_set_cell (ctx, i, 1, bar_cell);
                zetui_draw_str (ctx, 2, 1, " zetui demo ", title);
            }

            /* Content */
            zetui_draw_str (ctx, 2, 3, "Hello from zetui!", title);
            zetui_draw_str (ctx, 2, 5, "Press q or Ctrl-C to quit.", normal);

            /* Terminal size info */
            {
                int pos;
                const char *label;
                char *p;
                int n;

                label = "Size: ";
                pos = 0;
                memcpy (info, label, 6);
                pos = 6;

                /* itoa w */
                n = w;
                if (n == 0)
                    {
                        info[pos++] = '0';
                    }
                else
                    {
                        char tmp[12];
                        int ti = 0;
                        while (n > 0)
                            {
                                tmp[ti++] = (char)('0' + n % 10);
                                n /= 10;
                            }
                        while (ti > 0)
                            info[pos++] = tmp[--ti];
                    }
                info[pos++] = 'x';
                /* itoa h */
                n = h;
                if (n == 0)
                    {
                        info[pos++] = '0';
                    }
                else
                    {
                        char tmp[12];
                        int ti = 0;
                        while (n > 0)
                            {
                                tmp[ti++] = (char)('0' + n % 10);
                                n /= 10;
                            }
                        while (ti > 0)
                            info[pos++] = tmp[--ti];
                    }
                info[pos] = '\0';

                zetui_draw_str (ctx, 2, 7, info, dim);
            }

            zetui_present (ctx);

            ev = zetui_wait_event (ctx, -1);

            if (ev.type == ZETUI_EVENT_KEY)
                {
                    if (ev.data.key.key == ZETUI_KEY_CTRL_C
                        || ev.data.key.ch == (zetui_u32)'q')
                        {
                            running = 0;
                        }
                }
            /* On resize the next loop iteration redraws at the new size */
        }

    zetui_cursor_show (ctx);
    zetui_shutdown (ctx);
    return 0;
}
