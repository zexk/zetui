/**
 * @file zetui.h
 * @brief Single-header ANSI/C89 terminal UI library (STB style).
 *
 * In exactly one translation unit define the implementation before including:
 * @code
 *   #define ZETUI_IMPLEMENTATION
 *   #include "zetui.h"
 * @endcode
 * Every other translation unit includes the header without the define.
 *
 * @author zexk
 * @copyright Unlicense (public domain)
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

    /** @brief Unsigned 8-bit integer (no <stdint.h> in C89). */
    typedef unsigned char zetui_u8;
    /** @brief Unsigned 16-bit integer. */
    typedef unsigned short zetui_u16;
    /** @brief Signed 32-bit integer. */
    typedef int zetui_i32;
    /** @brief Unsigned 32-bit integer (used for Unicode codepoints). */
    typedef unsigned int zetui_u32;

    /* ================================================================== */
    /*  Error codes                                                        */
    /* ================================================================== */

    /**
     * @brief Return codes used by functions that can fail.
     */
    typedef enum zetui_error
    {
        ZETUI_OK = 0,             /**< Success. */
        ZETUI_ERR_NOMEM = -1,     /**< Memory allocation failed. */
        ZETUI_ERR_IO = -2,        /**< Terminal I/O error. */
        ZETUI_ERR_NOT_A_TTY = -3, /**< stdout is not a TTY. */
        ZETUI_ERR_UNSUPPORTED = -4, /**< Feature not supported by terminal. */
        ZETUI_ERR_TIMEOUT = -5    /**< Wait timed out with no event. */
    } zetui_error_t;

    /* ================================================================== */
    /*  Colors                                                             */
    /* ================================================================== */

    /**
     * @brief ANSI terminal colors (16 named + default).
     *
     * Use @c ZETUI_COLOR_DEFAULT to leave the terminal's default color in
     * place for the foreground or background.
     */
    typedef enum zetui_color
    {
        ZETUI_COLOR_DEFAULT = -1,       /**< Terminal default color. */
        ZETUI_COLOR_BLACK = 0,          /**< Black (ANSI 0). */
        ZETUI_COLOR_RED = 1,            /**< Red. */
        ZETUI_COLOR_GREEN = 2,          /**< Green. */
        ZETUI_COLOR_YELLOW = 3,         /**< Yellow. */
        ZETUI_COLOR_BLUE = 4,           /**< Blue. */
        ZETUI_COLOR_MAGENTA = 5,        /**< Magenta. */
        ZETUI_COLOR_CYAN = 6,           /**< Cyan. */
        ZETUI_COLOR_WHITE = 7,          /**< White. */
        ZETUI_COLOR_BRIGHT_BLACK = 8,   /**< Bright black (dark grey). */
        ZETUI_COLOR_BRIGHT_RED = 9,     /**< Bright red. */
        ZETUI_COLOR_BRIGHT_GREEN = 10,  /**< Bright green. */
        ZETUI_COLOR_BRIGHT_YELLOW = 11, /**< Bright yellow. */
        ZETUI_COLOR_BRIGHT_BLUE = 12,   /**< Bright blue. */
        ZETUI_COLOR_BRIGHT_MAGENTA = 13,/**< Bright magenta. */
        ZETUI_COLOR_BRIGHT_CYAN = 14,   /**< Bright cyan. */
        ZETUI_COLOR_BRIGHT_WHITE = 15   /**< Bright white. */
    } zetui_color_t;

    /** @brief Encode R, G, B into a 24-bit "color" value.
     *
     *  The result can be used anywhere a @c zetui_color_t value is expected
     *  (both fields of @c zetui_style_t and @c zetui_cell_t).
     *
     *  @code
     *      zetui_style_t s;
     *      s.fg = ZETUI_COLOR_RGB(255, 128, 0);   // orange foreground
     *      s.bg = ZETUI_COLOR_RGB(0, 0, 0);        // black background
     *  @endcode
     *
     *  Terminals that do not support 24-bit colour ignore the escape sequence
     *  and keep the previous colour.
     */
#define ZETUI_COLOR_RGB(r, g, b)                                              \
    ((zetui_i32)((1u << 24) | ((zetui_u32)(r) << 16) | ((zetui_u32)(g) << 8)  \
                 | (zetui_u32)(b)))

/** @brief True if @p c was created with @c ZETUI_COLOR_RGB (vs a palette color). */
#define ZETUI_COLOR_IS_RGB(c) (((zetui_u32)(c) >> 24) == 1u)

    /* ================================================================== */
    /*  Cell attributes                                                    */
    /* ================================================================== */

    /** @defgroup attrs Cell Attributes
     *  @brief Bitfield flags OR-combined into @c zetui_style_t.attrs.
     *  @{
     */
#define ZETUI_ATTR_NONE      0u         /**< No attributes. */
#define ZETUI_ATTR_BOLD      (1u << 0)  /**< Bold / increased intensity. */
#define ZETUI_ATTR_DIM       (1u << 1)  /**< Dim / decreased intensity. */
#define ZETUI_ATTR_ITALIC    (1u << 2)  /**< Italic (terminal support varies). */
#define ZETUI_ATTR_UNDERLINE (1u << 3)  /**< Single underline. */
#define ZETUI_ATTR_BLINK     (1u << 4)  /**< Slow blink (terminal support varies). */
#define ZETUI_ATTR_REVERSE   (1u << 5)  /**< Swap foreground and background. */
#define ZETUI_ATTR_HIDDEN    (1u << 6)  /**< Invisible text. */
#define ZETUI_ATTR_STRIKE    (1u << 7)  /**< Strikethrough. */
    /** @} */

    /* ================================================================== */
    /*  Cell                                                               */
    /* ================================================================== */

    /**
     * @brief Marker codepoint occupying the cell right of a wide character.
     *
     * Wide (two-column) characters such as CJK and most emoji cover two
     * cells: the glyph lives in the left cell and the right cell holds
     * this marker. zetui_set_cell() maintains the pairing automatically;
     * cells read back with zetui_get_cell() may contain it.
     */
#define ZETUI_WIDE_PAD 0xFFFFFFFFu

    /**
     * @brief A single terminal cell (codepoint + style).
     */
    typedef struct zetui_cell
    {
        zetui_u32 ch;    /**< Unicode codepoint; 0 is treated as a space,
                              @c ZETUI_WIDE_PAD marks a wide-char tail. */
        zetui_i32 fg;    /**< Foreground color (@c zetui_color_t or @c ZETUI_COLOR_DEFAULT). */
        zetui_i32 bg;    /**< Background color (@c zetui_color_t or @c ZETUI_COLOR_DEFAULT). */
        zetui_u32 attrs; /**< Attribute flags (@c ZETUI_ATTR_* OR-combined). */
    } zetui_cell_t;

    /* ================================================================== */
    /*  Style                                                              */
    /* ================================================================== */

    /**
     * @brief Foreground color, background color, and attribute flags.
     *
     * Zero-initialize for terminal defaults with no attributes.
     */
    typedef struct zetui_style
    {
        zetui_i32 fg;    /**< Foreground color (@c zetui_color_t or @c ZETUI_COLOR_DEFAULT). */
        zetui_i32 bg;    /**< Background color (@c zetui_color_t or @c ZETUI_COLOR_DEFAULT). */
        zetui_u32 attrs; /**< Attribute flags (@c ZETUI_ATTR_* OR-combined). */
    } zetui_style_t;

    /* ================================================================== */
    /*  Key codes                                                          */
    /* ================================================================== */

    /**
     * @brief Special key identifiers.
     *
     * Printable characters are reported via @c zetui_key_event_t.ch (Unicode
     * codepoint) and @c key is set to @c ZETUI_KEY_NONE.
     */
    typedef enum zetui_key
    {
        ZETUI_KEY_NONE = 0,          /**< No special key; check @c ch. */
        ZETUI_KEY_CTRL_A = 1,
        ZETUI_KEY_CTRL_B = 2,
        ZETUI_KEY_CTRL_C = 3,        /**< Interrupt (Ctrl-C). */
        ZETUI_KEY_CTRL_D = 4,
        ZETUI_KEY_CTRL_E = 5,
        ZETUI_KEY_CTRL_F = 6,
        ZETUI_KEY_CTRL_G = 7,
        ZETUI_KEY_BACKSPACE = 8,
        ZETUI_KEY_TAB = 9,
        ZETUI_KEY_CTRL_J = 10,
        ZETUI_KEY_ENTER = 13,
        ZETUI_KEY_CTRL_K = 11,
        ZETUI_KEY_CTRL_L = 12,
        ZETUI_KEY_CTRL_N = 14,
        ZETUI_KEY_CTRL_O = 15,
        ZETUI_KEY_CTRL_P = 16,
        ZETUI_KEY_CTRL_Q = 17,
        ZETUI_KEY_CTRL_R = 18,
        ZETUI_KEY_CTRL_S = 19,
        ZETUI_KEY_CTRL_T = 20,
        ZETUI_KEY_CTRL_U = 21,
        ZETUI_KEY_CTRL_V = 22,
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

    /** @defgroup mods Key Modifiers
     *  @brief Bitfield flags for modifier keys, set in @c zetui_key_event_t.mods.
     *  @{
     */
#define ZETUI_MOD_NONE  0u        /**< No modifier. */
#define ZETUI_MOD_SHIFT (1u << 0) /**< Shift held. */
#define ZETUI_MOD_ALT   (1u << 1) /**< Alt / Meta held. */
#define ZETUI_MOD_CTRL  (1u << 2) /**< Ctrl held. */
    /** @} */

    /* ================================================================== */
    /*  Events                                                             */
    /* ================================================================== */

    /** @brief Discriminator for @c zetui_event_t. */
    typedef enum zetui_event_type
    {
        ZETUI_EVENT_NONE = 0,   /**< No event (poll returned immediately). */
        ZETUI_EVENT_KEY = 1,    /**< Keyboard event; see @c data.key. */
        ZETUI_EVENT_RESIZE = 2, /**< Terminal was resized; see @c data.resize. */
        ZETUI_EVENT_MOUSE = 3,  /**< Mouse event; see @c data.mouse. */
        ZETUI_EVENT_FOCUS = 4   /**< Focus gained/lost; see @c data.focus. */
    } zetui_event_type_t;

    /** @brief Keyboard event payload. */
    typedef struct zetui_key_event
    {
        zetui_key_t key;  /**< Special key code, or @c ZETUI_KEY_NONE for printable. */
        zetui_u32   ch;   /**< Unicode codepoint (printable chars). */
        zetui_u32   mods; /**< Active modifier flags (@c ZETUI_MOD_*). */
    } zetui_key_event_t;

    /** @brief Terminal resize event payload. */
    typedef struct zetui_resize_event
    {
        int width;  /**< New terminal width in columns. */
        int height; /**< New terminal height in rows. */
    } zetui_resize_event_t;

    /** @brief What a mouse event reports. */
    typedef enum zetui_mouse_action
    {
        ZETUI_MOUSE_PRESS = 0,     /**< Button pressed. */
        ZETUI_MOUSE_RELEASE = 1,   /**< Button released. */
        ZETUI_MOUSE_MOTION = 2,    /**< Movement with a button held (drag). */
        ZETUI_MOUSE_WHEEL_UP = 3,  /**< Wheel scrolled up. */
        ZETUI_MOUSE_WHEEL_DOWN = 4 /**< Wheel scrolled down. */
    } zetui_mouse_action_t;

    /** @brief Which button a mouse event refers to. */
    typedef enum zetui_mouse_button
    {
        ZETUI_MOUSE_BUTTON_NONE = 0,  /**< No button (wheel events). */
        ZETUI_MOUSE_BUTTON_LEFT = 1,
        ZETUI_MOUSE_BUTTON_MIDDLE = 2,
        ZETUI_MOUSE_BUTTON_RIGHT = 3
    } zetui_mouse_button_t;

    /** @brief Mouse event payload (requires zetui_mouse_enable()). */
    typedef struct zetui_mouse_event
    {
        zetui_mouse_action_t action; /**< Press, release, motion or wheel. */
        zetui_mouse_button_t button; /**< Button for press/release/motion. */
        int x;            /**< Column (0-based). */
        int y;            /**< Row (0-based). */
        zetui_u32 mods;   /**< Active modifier flags (@c ZETUI_MOD_*). */
    } zetui_mouse_event_t;

    /** @brief Focus event payload (requires zetui_focus_enable()). */
    typedef struct zetui_focus_event
    {
        int focused; /**< Non-zero when the terminal window gained focus. */
    } zetui_focus_event_t;

    /** @brief Union of all event payloads. */
    typedef union zetui_event_data
    {
        zetui_key_event_t    key;    /**< Valid when @c zetui_event_t.type == ZETUI_EVENT_KEY. */
        zetui_resize_event_t resize; /**< Valid when @c zetui_event_t.type == ZETUI_EVENT_RESIZE. */
        zetui_mouse_event_t  mouse;  /**< Valid when @c zetui_event_t.type == ZETUI_EVENT_MOUSE. */
        zetui_focus_event_t  focus;  /**< Valid when @c zetui_event_t.type == ZETUI_EVENT_FOCUS. */
    } zetui_event_data_t;

    /**
     * @brief A terminal event (key press, resize, or mouse).
     *
     * Check @c type first, then access the appropriate field of @c data.
     */
    typedef struct zetui_event
    {
        zetui_event_type_t type; /**< Event discriminator. */
        zetui_event_data_t data; /**< Event payload. */
    } zetui_event_t;

    /* ================================================================== */
    /*  Opaque context                                                     */
    /* ================================================================== */

    /**
     * @brief Opaque handle returned by zetui_init().
     *
     * All API functions take a pointer to this type. The internal layout is
     * private; allocate only via zetui_init() and free via zetui_shutdown().
     */
    typedef struct zetui_ctx zetui_ctx_t;

    /* ================================================================== */
    /*  Box-drawing codepoint tables                                       */
    /* ================================================================== */

    /**
     * @brief Unicode codepoints for light box-drawing characters.
     * @see ZETUI_BOX_TL and friends for index constants.
     */
    extern const zetui_u32 zetui_box_light[11];

    /** @brief Unicode codepoints for heavy box-drawing characters. */
    extern const zetui_u32 zetui_box_heavy[11];

    /** @brief Unicode codepoints for double box-drawing characters. */
    extern const zetui_u32 zetui_box_double[11];

    /** @defgroup box_idx Box Index Constants
     *  @brief Indices into @c zetui_box_light / @c zetui_box_heavy / @c zetui_box_double.
     *  @{
     */
#define ZETUI_BOX_TL 0  /**< Top-left corner     ┌ */
#define ZETUI_BOX_TR 1  /**< Top-right corner    ┐ */
#define ZETUI_BOX_BL 2  /**< Bottom-left corner  └ */
#define ZETUI_BOX_BR 3  /**< Bottom-right corner ┘ */
#define ZETUI_BOX_H  4  /**< Horizontal line     ─ */
#define ZETUI_BOX_V  5  /**< Vertical line       │ */
#define ZETUI_BOX_LT 6  /**< Left T-junction     ├ */
#define ZETUI_BOX_RT 7  /**< Right T-junction    ┤ */
#define ZETUI_BOX_TT 8  /**< Top T-junction      ┬ */
#define ZETUI_BOX_BT 9  /**< Bottom T-junction   ┴ */
#define ZETUI_BOX_X  10 /**< Cross               ┼ */
    /** @} */

    /**
     * @brief Which character set to use for box-drawing.
     *
     * Passed to @c zetui_draw_box() to select the visual appearance.
     */
    typedef enum zetui_box_style
    {
        ZETUI_BOX_STYLE_LIGHT  = 0, /**< Thin single lines  ┌─┐. */
        ZETUI_BOX_STYLE_HEAVY  = 1, /**< Bold single lines  ┏━┓. */
        ZETUI_BOX_STYLE_DOUBLE = 2  /**< Double lines       ╔═╗. */
    } zetui_box_style_t;

    /* ================================================================== */
    /*  Public API                                                         */
    /* ================================================================== */

    /* --- Lifecycle ---------------------------------------------------- */

    /**
     * @brief Initialise zetui: enter raw mode and alternate screen.
     * @return Allocated context, or NULL if stdout is not a TTY or on error.
     */
    zetui_ctx_t *zetui_init (void);

    /**
     * @brief Initialise zetui, reporting the failure reason.
     *
     * Identical to zetui_init() but stores the cause in @p err when NULL
     * is returned: @c ZETUI_ERR_NOMEM, @c ZETUI_ERR_NOT_A_TTY or
     * @c ZETUI_ERR_IO.
     * @param err Receives the error code on failure (may be NULL).
     * @return Allocated context, or NULL on error.
     */
    zetui_ctx_t *zetui_init_ex (zetui_error_t *err);

    /**
     * @brief Restore terminal state and free the context.
     * @param ctx Context previously returned by zetui_init().
     */
    void zetui_shutdown (zetui_ctx_t *ctx);

    /* --- Terminal info ------------------------------------------------ */

    /**
     * @brief Current terminal width in columns.
     * @param ctx Initialised context.
     * @return Column count; updated after each @c ZETUI_EVENT_RESIZE.
     */
    int zetui_width (const zetui_ctx_t *ctx);

    /**
     * @brief Current terminal height in rows.
     * @param ctx Initialised context.
     * @return Row count; updated after each @c ZETUI_EVENT_RESIZE.
     */
    int zetui_height (const zetui_ctx_t *ctx);

    /* --- Back-buffer drawing ------------------------------------------ */

    /**
     * @brief Fill the back buffer with blank cells (terminal default style).
     * @param ctx Initialised context.
     */
    void zetui_clear (zetui_ctx_t *ctx);

    /**
     * @brief Write a single cell to the back buffer.
     *
     * Out-of-bounds coordinates are silently ignored. Wide codepoints
     * (zetui_char_width() == 2) also claim the cell to their right with
     * @c ZETUI_WIDE_PAD; overwriting either half of an existing wide
     * character truncates it to a space. A wide codepoint written to the
     * last column, where its second half cannot fit, becomes a space.
     * @param ctx  Initialised context.
     * @param x    Column (0-based).
     * @param y    Row (0-based).
     * @param cell Cell value to write.
     */
    void zetui_set_cell (zetui_ctx_t *ctx, int x, int y, zetui_cell_t cell);

    /**
     * @brief Read a cell from the back buffer.
     *
     * Returns a blank cell for out-of-bounds coordinates.
     * @param ctx Initialised context.
     * @param x   Column (0-based).
     * @param y   Row (0-based).
     * @return    Copy of the cell currently in the buffer.
     */
    zetui_cell_t zetui_get_cell (const zetui_ctx_t *ctx, int x, int y);

    /**
     * @brief Diff the back buffer against the front buffer and flush to the terminal.
     *
     * Only changed cells are emitted. Swaps front and back buffers on success.
     * @param ctx Initialised context.
     * @return @c ZETUI_OK on success, @c ZETUI_ERR_IO on write failure.
     */
    zetui_error_t zetui_present (zetui_ctx_t *ctx);

    /* --- Input -------------------------------------------------------- */

    /**
     * @brief Non-blocking event poll.
     * @param ctx Initialised context.
     * @return The next event, or an event with @c type == @c ZETUI_EVENT_NONE
     *         if no input is available.
     */
    zetui_event_t zetui_poll_event (zetui_ctx_t *ctx);

    /**
     * @brief Blocking event wait with optional timeout.
     * @param ctx        Initialised context.
     * @param timeout_ms Milliseconds to wait; -1 blocks indefinitely.
     * @return The next event. @c type == @c ZETUI_EVENT_NONE on timeout.
     */
    zetui_event_t zetui_wait_event (zetui_ctx_t *ctx, int timeout_ms);

    /**
     * @brief Enable mouse reporting (SGR protocol).
     *
     * Button presses, releases, wheel scrolls and drag motion are then
     * delivered as @c ZETUI_EVENT_MOUSE events. Off by default because
     * mouse reporting takes over the terminal's native text selection.
     * Automatically disabled by zetui_shutdown().
     * @param ctx Initialised context.
     */
    void zetui_mouse_enable (zetui_ctx_t *ctx);

    /**
     * @brief Disable mouse reporting.
     * @param ctx Initialised context.
     */
    void zetui_mouse_disable (zetui_ctx_t *ctx);

    /**
     * @brief Enable focus-change reporting.
     *
     * The terminal sends @c ZETUI_EVENT_FOCUS when its window gains or loses
     * focus. Off by default. Automatically disabled by zetui_shutdown().
     * @param ctx Initialised context.
     */
    void zetui_focus_enable (zetui_ctx_t *ctx);

    /**
     * @brief Disable focus-change reporting.
     * @param ctx Initialised context.
     */
    void zetui_focus_disable (zetui_ctx_t *ctx);

    /* --- Drawing helpers ---------------------------------------------- */

    /**
     * @brief Return a style with terminal-default foreground, background,
     *        and no attributes.
     */
    zetui_style_t zetui_style_default (void);

    /**
     * @brief Construct a @c zetui_cell_t from a codepoint and style.
     * @param ch    Unicode codepoint.
     * @param style Visual style.
     */
    zetui_cell_t zetui_cell_make (zetui_u32 ch, zetui_style_t style);

    /**
     * @brief Display width of a Unicode codepoint in terminal cells.
     * @param cp Unicode codepoint.
     * @return 2 for wide codepoints (CJK, Hangul, fullwidth forms, most
     *         emoji), 0 for zero-width codepoints (combining marks,
     *         joiners, variation selectors), 1 otherwise.
     */
    int zetui_char_width (zetui_u32 cp);

    /**
     * @brief Draw a NUL-terminated UTF-8 string to the back buffer.
     *
     * The cursor advances by zetui_char_width() per codepoint, so wide
     * characters occupy two columns. Zero-width codepoints are skipped
     * (cells hold exactly one codepoint each).
     * @param ctx   Initialised context.
     * @param x     Starting column.
     * @param y     Row.
     * @param str   NUL-terminated UTF-8 string.
     * @param style Visual style applied to every cell.
     */
    void zetui_draw_str (zetui_ctx_t *ctx, int x, int y, const char *str,
                         zetui_style_t style);

    /**
     * @brief Draw a box-drawing rectangle.
     * @param ctx      Initialised context.
     * @param x        Left column.
     * @param y        Top row.
     * @param w        Width in columns (must be >= 2).
     * @param h        Height in rows (must be >= 2).
     * @param box      Character set: light, heavy, or double.
     * @param style    Visual style for all border cells.
     */
    void zetui_draw_box (zetui_ctx_t *ctx, int x, int y, int w, int h,
                         zetui_box_style_t box, zetui_style_t style);

    /**
     * @brief Fill a rectangular region with a single cell.
     * @param ctx  Initialised context.
     * @param x    Left column.
     * @param y    Top row.
     * @param w    Width in columns.
     * @param h    Height in rows.
     * @param cell Cell written to every position in the rectangle.
     */
    void zetui_fill_rect (zetui_ctx_t *ctx, int x, int y, int w, int h,
                          zetui_cell_t cell);

    /**
     * @brief Draw a horizontal line of a single codepoint.
     * @param ctx   Initialised context.
     * @param x     Starting column.
     * @param y     Row.
     * @param len   Number of cells to fill.
     * @param ch    Unicode codepoint to repeat.
     * @param style Visual style.
     */
    void zetui_draw_hline (zetui_ctx_t *ctx, int x, int y, int len,
                           zetui_u32 ch, zetui_style_t style);

    /**
     * @brief Draw a vertical line of a single codepoint.
     * @param ctx   Initialised context.
     * @param x     Column.
     * @param y     Starting row.
     * @param len   Number of cells to fill.
     * @param ch    Unicode codepoint to repeat.
     * @param style Visual style.
     */
    void zetui_draw_vline (zetui_ctx_t *ctx, int x, int y, int len,
                           zetui_u32 ch, zetui_style_t style);

    /* --- Cursor ------------------------------------------------------- */

    /** @brief Hide the hardware cursor. @param ctx Initialised context. */
    void zetui_cursor_hide (zetui_ctx_t *ctx);

    /** @brief Show the hardware cursor. @param ctx Initialised context. */
    void zetui_cursor_show (zetui_ctx_t *ctx);

    /**
     * @brief Move the hardware cursor to (x, y).
     * @param ctx Initialised context.
     * @param x   Column (0-based).
     * @param y   Row (0-based).
     */
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
#include <time.h>
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
        unsigned int u;
        int i, len;

        if (n == 0)
            return zetui__obuf_append (ob, "0", 1u);

        if (n < 0)
            {
                zetui__obuf_append (ob, "-", 1u);
                u = (unsigned int)-(n + 1) + 1u; /* INT_MIN-safe negate */
            }
        else
            {
                u = (unsigned int)n;
            }

        i = 0;
        while (u > 0u)
            {
                tmp[i++] = (char)('0' + (u % 10u));
                u /= 10u;
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

    /* write() the whole buffer, retrying on EINTR and partial writes. */
    static void
    zetui__write_all (int fd, const char *buf, size_t len)
    {
        size_t off;
        ssize_t n;

        off = 0u;
        while (off < len)
            {
                n = write (fd, buf + off, len - off);
                if (n < 0)
                    {
                        if (errno == EINTR)
                            continue;
                        return;
                    }
                off += (size_t)n;
            }
    }

    /* ------------------------------------------------------------------ */
    /*  Internal: input buffer                                             */
    /* ------------------------------------------------------------------ */

#define ZETUI__IBUF 64

    typedef struct
    {
        unsigned char data[ZETUI__IBUF];
        int len;
        int pos;
    } zetui__ibuf_t;

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

        /* Pending input bytes carried across poll/wait calls */
        zetui__ibuf_t in;

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

        /* Whether the terminal itself currently shows the cursor */
        int term_cursor_on;

        /* Whether SGR mouse reporting is enabled */
        int mouse_on;

        /* Whether focus-change reporting is enabled */
        int focus_on;

        /* Set by SIGWINCH handler */
        int resize_pending;

        /* Signal masks for race-free SIGWINCH handling via pselect() */
        sigset_t orig_mask;   /* process mask to restore at shutdown */
        sigset_t select_mask; /* orig_mask with SIGWINCH unblocked   */

        /* SIGWINCH disposition that was in place before zetui_init() */
        struct sigaction old_winch;
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
    /*  Codepoint display width                                            */
    /* ------------------------------------------------------------------ */

    typedef struct
    {
        zetui_u32 first;
        zetui_u32 last;
    } zetui__cp_range_t;

    /* Zero-width codepoints: the principal combining-mark ranges plus
       joiners, bidi controls and variation selectors. Sorted. */
    static const zetui__cp_range_t zetui__zero_width[] = {
        { 0x0300u, 0x036Fu },   /* combining diacritical marks      */
        { 0x0483u, 0x0489u },   /* cyrillic combining               */
        { 0x0591u, 0x05BDu },   /* hebrew points                    */
        { 0x05BFu, 0x05BFu },   { 0x05C1u, 0x05C2u },
        { 0x05C4u, 0x05C5u },   { 0x05C7u, 0x05C7u },
        { 0x0610u, 0x061Au },   /* arabic marks                     */
        { 0x064Bu, 0x065Fu },   { 0x0670u, 0x0670u },
        { 0x06D6u, 0x06DCu },   { 0x06DFu, 0x06E4u },
        { 0x06E7u, 0x06E8u },   { 0x06EAu, 0x06EDu },
        { 0x0711u, 0x0711u },   /* syriac                           */
        { 0x0730u, 0x074Au },   { 0x07A6u, 0x07B0u },
        { 0x0E31u, 0x0E31u },   /* thai                             */
        { 0x0E34u, 0x0E3Au },   { 0x0E47u, 0x0E4Eu },
        { 0x0EB1u, 0x0EB1u },   /* lao                              */
        { 0x0EB4u, 0x0EBCu },   { 0x0EC8u, 0x0ECDu },
        { 0x135Du, 0x135Fu },   /* ethiopic combining               */
        { 0x1AB0u, 0x1AFFu },   /* combining marks extended         */
        { 0x1DC0u, 0x1DFFu },   /* combining marks supplement       */
        { 0x200Bu, 0x200Fu },   /* ZWSP, ZWNJ, ZWJ, LRM, RLM        */
        { 0x202Au, 0x202Eu },   /* bidi embedding controls          */
        { 0x2060u, 0x2064u },   /* word joiner, invisible operators */
        { 0x20D0u, 0x20FFu },   /* combining marks for symbols      */
        { 0xFE00u, 0xFE0Fu },   /* variation selectors              */
        { 0xFE20u, 0xFE2Fu },   /* combining half marks             */
        { 0xFEFFu, 0xFEFFu },   /* zero width no-break space        */
        { 0xE0100u, 0xE01EFu }  /* variation selectors supplement   */
    };

    /* Wide (two-column) codepoints: East Asian Wide/Fullwidth ranges
       plus the emoji blocks terminals render double-width. Sorted. */
    static const zetui__cp_range_t zetui__double_width[] = {
        { 0x1100u, 0x115Fu },   /* hangul jamo                      */
        { 0x231Au, 0x231Bu },   /* watch, hourglass                 */
        { 0x2329u, 0x232Au },   /* angle brackets                   */
        { 0x23E9u, 0x23ECu },   { 0x23F0u, 0x23F0u },
        { 0x23F3u, 0x23F3u },   { 0x25FDu, 0x25FEu },
        { 0x2614u, 0x2615u },   { 0x2648u, 0x2653u },
        { 0x267Fu, 0x267Fu },   { 0x2693u, 0x2693u },
        { 0x26A1u, 0x26A1u },   { 0x26AAu, 0x26ABu },
        { 0x26BDu, 0x26BEu },   { 0x26C4u, 0x26C5u },
        { 0x26CEu, 0x26CEu },   { 0x26D4u, 0x26D4u },
        { 0x26EAu, 0x26EAu },   { 0x26F2u, 0x26F3u },
        { 0x26F5u, 0x26F5u },   { 0x26FAu, 0x26FAu },
        { 0x26FDu, 0x26FDu },   { 0x2705u, 0x2705u },
        { 0x270Au, 0x270Bu },   { 0x2728u, 0x2728u },
        { 0x274Cu, 0x274Cu },   { 0x274Eu, 0x274Eu },
        { 0x2753u, 0x2755u },   { 0x2757u, 0x2757u },
        { 0x2795u, 0x2797u },   { 0x27B0u, 0x27B0u },
        { 0x27BFu, 0x27BFu },   { 0x2B1Bu, 0x2B1Cu },
        { 0x2B50u, 0x2B50u },   { 0x2B55u, 0x2B55u },
        { 0x2E80u, 0x303Eu },   /* CJK radicals .. CJK punctuation  */
        { 0x3041u, 0xA4CFu },   /* kana .. CJK .. Yi                */
        { 0xA960u, 0xA97Fu },   /* hangul jamo extended-A           */
        { 0xAC00u, 0xD7A3u },   /* hangul syllables                 */
        { 0xF900u, 0xFAFFu },   /* CJK compatibility ideographs     */
        { 0xFE10u, 0xFE19u },   /* vertical forms                   */
        { 0xFE30u, 0xFE6Fu },   /* CJK compat forms, small forms    */
        { 0xFF00u, 0xFF60u },   /* fullwidth forms                  */
        { 0xFFE0u, 0xFFE6u },   /* fullwidth signs                  */
        { 0x16FE0u, 0x16FE4u }, /* tangut marks                     */
        { 0x17000u, 0x18AFFu }, /* tangut                           */
        { 0x1B000u, 0x1B2FFu }, /* kana extensions                  */
        { 0x1F004u, 0x1F004u }, { 0x1F0CFu, 0x1F0CFu },
        { 0x1F18Eu, 0x1F18Eu }, { 0x1F191u, 0x1F19Au },
        { 0x1F200u, 0x1F2FFu }, /* enclosed ideographic supplement  */
        { 0x1F300u, 0x1F64Fu }, /* pictographs, emoticons           */
        { 0x1F680u, 0x1F6FFu }, /* transport symbols                */
        { 0x1F7E0u, 0x1F7FFu }, /* geometric shapes extended        */
        { 0x1F900u, 0x1F9FFu }, /* supplemental symbols             */
        { 0x1FA70u, 0x1FAFFu }, /* symbols extended-A               */
        { 0x20000u, 0x2FFFDu }, /* CJK extension B and beyond       */
        { 0x30000u, 0x3FFFDu }
    };

    static int
    zetui__cp_in_table (zetui_u32 cp, const zetui__cp_range_t *t, int n)
    {
        int lo, hi, mid;

        if (cp < t[0].first || cp > t[n - 1].last)
            return 0;
        lo = 0;
        hi = n - 1;
        while (lo <= hi)
            {
                mid = (lo + hi) / 2;
                if (cp < t[mid].first)
                    hi = mid - 1;
                else if (cp > t[mid].last)
                    lo = mid + 1;
                else
                    return 1;
            }
        return 0;
    }

    int
    zetui_char_width (zetui_u32 cp)
    {
        if (cp < 0x20u || (cp >= 0x7Fu && cp < 0xA0u))
            return 1; /* NUL and control bytes still occupy their cell */
        if (zetui__cp_in_table (cp, zetui__zero_width,
                                (int)(sizeof (zetui__zero_width)
                                      / sizeof (zetui__zero_width[0]))))
            return 0;
        if (zetui__cp_in_table (cp, zetui__double_width,
                                (int)(sizeof (zetui__double_width)
                                      / sizeof (zetui__double_width[0]))))
            return 2;
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
                else if (ZETUI_COLOR_IS_RGB (fg))
                    {
                        zetui__obuf_append_str (ob, "\033[38;2;");
                        zetui__obuf_append_int (ob, (fg >> 16) & 0xFF);
                        zetui__obuf_append (ob, ";", 1u);
                        zetui__obuf_append_int (ob, (fg >> 8) & 0xFF);
                        zetui__obuf_append (ob, ";", 1u);
                        zetui__obuf_append_int (ob, fg & 0xFF);
                        zetui__obuf_append (ob, "m", 1u);
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
                else if (ZETUI_COLOR_IS_RGB (bg))
                    {
                        zetui__obuf_append_str (ob, "\033[48;2;");
                        zetui__obuf_append_int (ob, (bg >> 16) & 0xFF);
                        zetui__obuf_append (ob, ";", 1u);
                        zetui__obuf_append_int (ob, (bg >> 8) & 0xFF);
                        zetui__obuf_append (ob, ";", 1u);
                        zetui__obuf_append_int (ob, bg & 0xFF);
                        zetui__obuf_append (ob, "m", 1u);
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
        zetui_cell_t *bk;
        size_t idx;

        if (x < 0 || y < 0 || x >= ctx->width || y >= ctx->height)
            return;

        bk = ctx->back;
        idx = (size_t)(y * ctx->width + x);

        /* Overwriting the tail of a wide character truncates it. */
        if (bk[idx].ch == ZETUI_WIDE_PAD && cell.ch != ZETUI_WIDE_PAD
            && x > 0 && zetui_char_width (bk[idx - 1].ch) == 2)
            bk[idx - 1].ch = (zetui_u32)' ';

        /* Overwriting the head of a wide character orphans its tail. */
        if (zetui_char_width (bk[idx].ch) == 2 && x + 1 < ctx->width
            && bk[idx + 1].ch == ZETUI_WIDE_PAD)
            bk[idx + 1].ch = (zetui_u32)' ';

        bk[idx] = cell;

        if (zetui_char_width (cell.ch) == 2)
            {
                if (x + 1 >= ctx->width)
                    {
                        /* No room for the second column. */
                        bk[idx].ch = (zetui_u32)' ';
                    }
                else
                    {
                        /* The tail may in turn cover another wide char. */
                        if (zetui_char_width (bk[idx + 1].ch) == 2
                            && x + 2 < ctx->width
                            && bk[idx + 2].ch == ZETUI_WIDE_PAD)
                            bk[idx + 2].ch = (zetui_u32)' ';
                        bk[idx + 1] = cell;
                        bk[idx + 1].ch = ZETUI_WIDE_PAD;
                    }
            }
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
        int x, y, cw;
        zetui_cell_t *f, *b;
        zetui_u32 cp;
        char utf8[4];
        size_t ulen;

        ctx->ren_fg = ZETUI_COLOR_DEFAULT;
        ctx->ren_bg = ZETUI_COLOR_DEFAULT;
        ctx->ren_attrs = ZETUI_ATTR_NONE;
        ctx->ren_x = -1;
        ctx->ren_y = -1;

        ctx->out.len = 0u;

        for (y = 0; y < ctx->height; y++)
            {
                for (x = 0; x < ctx->width; x++)
                    {
                        b = &ctx->back[y * ctx->width + x];
                        f = &ctx->front[y * ctx->width + x];

                        if (zetui__cells_eq (*b, *f))
                            continue;

                        /* Wide-char tails are covered by the glyph in
                           the cell to their left; emit nothing. */
                        if (b->ch == ZETUI_WIDE_PAD)
                            {
                                *f = *b;
                                continue;
                            }

                        /* Hide the cursor while painting, but only if it
                           is actually shown: unconditional hide/show per
                           frame makes a visible cursor flicker. */
                        if (ctx->term_cursor_on)
                            {
                                zetui__obuf_append_str (&ctx->out,
                                                        "\033[?25l");
                                ctx->term_cursor_on = 0;
                            }

                        if (ctx->ren_x != x || ctx->ren_y != y)
                            {
                                zetui__ansi_move (&ctx->out, x, y);
                                ctx->ren_x = x;
                                ctx->ren_y = y;
                            }

                        zetui__ansi_style (&ctx->out, b->fg, b->bg, b->attrs,
                                           &ctx->ren_fg, &ctx->ren_bg,
                                           &ctx->ren_attrs);

                        cp = b->ch == 0u ? (zetui_u32)' ' : b->ch;
                        ulen = zetui__utf8_enc (cp, utf8);
                        zetui__obuf_append (&ctx->out, utf8, ulen);

                        /* Track how far the terminal cursor advanced;
                           for zero-width codepoints the position is
                           unreliable, so force a move next time. */
                        cw = zetui_char_width (cp);
                        if (cw == 2)
                            ctx->ren_x += 2;
                        else if (cw < 1)
                            ctx->ren_x = -1;
                        else
                            ctx->ren_x++;
                        *f = *b;
                    }
            }

        if (ctx->cursor_visible)
            {
                zetui__ansi_move (&ctx->out, ctx->cursor_x, ctx->cursor_y);
                if (!ctx->term_cursor_on)
                    {
                        zetui__obuf_append_str (&ctx->out, "\033[?25h");
                        ctx->term_cursor_on = 1;
                    }
            }
        else if (ctx->term_cursor_on)
            {
                zetui__obuf_append_str (&ctx->out, "\033[?25l");
                ctx->term_cursor_on = 0;
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
    zetui_init_ex (zetui_error_t *err)
    {
        zetui_ctx_t *ctx;
        zetui_error_t dummy;
        int w, h;

        if (!err)
            err = &dummy;
        *err = ZETUI_OK;

        ctx = (zetui_ctx_t *)calloc (1u, sizeof (zetui_ctx_t));
        if (!ctx)
            {
                *err = ZETUI_ERR_NOMEM;
                return NULL;
            }

        ctx->fd_in = STDIN_FILENO;
        ctx->fd_out = STDOUT_FILENO;

        if (!isatty (ctx->fd_out))
            {
                *err = ZETUI_ERR_NOT_A_TTY;
                free (ctx);
                return NULL;
            }

        if (zetui__enter_raw (ctx->fd_in, &ctx->saved_termios) == -1)
            {
                *err = (errno == ENOTTY) ? ZETUI_ERR_NOT_A_TTY
                                         : ZETUI_ERR_IO;
                free (ctx);
                return NULL;
            }

        if (zetui__query_size (ctx->fd_out, &w, &h) == -1)
            {
                *err = ZETUI_ERR_IO;
                zetui__leave_raw (ctx->fd_in, &ctx->saved_termios);
                free (ctx);
                return NULL;
            }

        if (zetui__obuf_init (&ctx->out, 65536u) == -1)
            {
                *err = ZETUI_ERR_NOMEM;
                zetui__leave_raw (ctx->fd_in, &ctx->saved_termios);
                free (ctx);
                return NULL;
            }

        if (zetui__screen_resize (ctx, w, h) == -1)
            {
                *err = ZETUI_ERR_NOMEM;
                zetui__obuf_free (&ctx->out);
                zetui__leave_raw (ctx->fd_in, &ctx->saved_termios);
                free (ctx);
                return NULL;
            }

        ctx->cursor_visible = 1;
        ctx->cursor_x = 0;
        ctx->cursor_y = 0;
        ctx->term_cursor_on = 0; /* hidden by the init escape below */

        {
            struct sigaction sa;
            sigset_t block;

            memset (&sa, 0, sizeof (sa));
            sa.sa_handler = zetui__sigwinch;
            sigemptyset (&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction (SIGWINCH, &sa, &ctx->old_winch);

            /* Keep SIGWINCH blocked except inside pselect(): the resize
               flag can then only be raised while actually waiting, which
               closes the race between checking the flag and starting to
               wait. Signals arriving in between stay pending. */
            sigemptyset (&block);
            sigaddset (&block, SIGWINCH);
            sigprocmask (SIG_BLOCK, &block, &ctx->orig_mask);
            ctx->select_mask = ctx->orig_mask;
            sigdelset (&ctx->select_mask, SIGWINCH);
        }

        zetui__write_all (ctx->fd_out, "\033[?1049h\033[?25l", 14u);

        return ctx;
    }

    zetui_ctx_t *
    zetui_init (void)
    {
        return zetui_init_ex (NULL);
    }

    void
    zetui_mouse_enable (zetui_ctx_t *ctx)
    {
        if (ctx->mouse_on)
            return;
        /* button events + drag motion + SGR extended coordinates */
        zetui__write_all (ctx->fd_out, "\033[?1000h\033[?1002h\033[?1006h",
                          24u);
        ctx->mouse_on = 1;
    }

    void
    zetui_mouse_disable (zetui_ctx_t *ctx)
    {
        if (!ctx->mouse_on)
            return;
        zetui__write_all (ctx->fd_out, "\033[?1006l\033[?1002l\033[?1000l",
                          24u);
        ctx->mouse_on = 0;
    }

    void
    zetui_focus_enable (zetui_ctx_t *ctx)
    {
        if (ctx->focus_on)
            return;
        zetui__write_all (ctx->fd_out, "\033[?1004h", 8u);
        ctx->focus_on = 1;
    }

    void
    zetui_focus_disable (zetui_ctx_t *ctx)
    {
        if (!ctx->focus_on)
            return;
        zetui__write_all (ctx->fd_out, "\033[?1004l", 8u);
        ctx->focus_on = 0;
    }

    void
    zetui_shutdown (zetui_ctx_t *ctx)
    {
        if (!ctx)
            return;

        zetui_focus_disable (ctx);
        zetui_mouse_disable (ctx);
        sigaction (SIGWINCH, &ctx->old_winch, NULL);
        sigprocmask (SIG_SETMASK, &ctx->orig_mask, NULL);
        zetui__write_all (ctx->fd_out, "\033[?25h\033[?1049l", 14u);
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

    /* Wait up to timeout_ms for input and append it to the buffer.
       Uses pselect() with SIGWINCH unblocked so a resize can only be
       signalled while waiting (never between flag check and wait). */
    static int
    zetui__ibuf_read (zetui__ibuf_t *ib, int fd, int timeout_ms,
                      const sigset_t *mask)
    {
        fd_set rfds;
        struct timespec ts;
        struct timespec *tsp;
        struct timespec start, now;
        long elapsed_ms;
        int remaining_ms;
        ssize_t n;
        int ret;

        if (ib->len >= ZETUI__IBUF)
            return 0;

        remaining_ms = timeout_ms;
        if (timeout_ms > 0)
            clock_gettime (CLOCK_MONOTONIC, &start);

        /* Restart after signals that are not a resize, so an unrelated
           SIGALRM etc. does not surface as a spurious empty event. */
        for (;;)
            {
                FD_ZERO (&rfds);
                FD_SET (fd, &rfds);

                if (remaining_ms < 0)
                    {
                        tsp = NULL;
                    }
                else
                    {
                        ts.tv_sec = remaining_ms / 1000;
                        ts.tv_nsec = (long)(remaining_ms % 1000) * 1000000L;
                        tsp = &ts;
                    }

                ret = pselect (fd + 1, &rfds, NULL, NULL, tsp, mask);
                if (ret >= 0)
                    break;
                if (errno != EINTR || zetui__resize_flag)
                    return ret;

                if (timeout_ms > 0)
                    {
                        clock_gettime (CLOCK_MONOTONIC, &now);
                        elapsed_ms
                            = (long)(now.tv_sec - start.tv_sec) * 1000L
                              + (now.tv_nsec - start.tv_nsec) / 1000000L;
                        if (elapsed_ms >= (long)timeout_ms)
                            return 0;
                        remaining_ms = timeout_ms - (int)elapsed_ms;
                    }
            }

        if (ret == 0)
            return 0;

        n = read (fd, ib->data + ib->len, (size_t)(ZETUI__IBUF - ib->len));
        if (n <= 0)
            return -1;
        ib->len += (int)n;
        return (int)n;
    }

    static int
    zetui__ibuf_fill (zetui__ibuf_t *ib, int fd, int timeout_ms,
                      const sigset_t *mask)
    {
        ib->len = 0;
        ib->pos = 0;
        return zetui__ibuf_read (ib, fd, timeout_ms, mask);
    }

    /* Milliseconds to wait for the rest of a partially received escape
       sequence before treating a lone ESC as the Escape key. */
#define ZETUI__ESC_WAIT_MS 30

    /* Return 1 if buf[0..len), starting with ESC, could be the prefix
       of an escape sequence whose remaining bytes are still in flight. */
    static int
    zetui__esc_incomplete (const unsigned char *buf, int len)
    {
        int i;

        if (len <= 1)
            return 1;
        if (buf[1] == 0x1Bu) /* Alt prefix: check the inner sequence */
            return zetui__esc_incomplete (buf + 1, len - 1);
        if (buf[1] == (unsigned char)'[')
            {
                for (i = 2; i < len; i++)
                    if (buf[i] >= 0x40u && buf[i] <= 0x7Eu)
                        return 0; /* CSI final byte seen */
                return 1;
            }
        if (buf[1] == (unsigned char)'O')
            return len < 3; /* SS3 needs one more byte */
        return 0;
    }

    /* If the pending bytes begin an incomplete escape sequence, wait
       briefly for the remainder (sequences are routinely split across
       read() boundaries, e.g. over SSH). */
    static void
    zetui__ibuf_complete_esc (zetui_ctx_t *ctx)
    {
        zetui__ibuf_t *ib;

        ib = &ctx->in;
        if (ib->pos >= ib->len || ib->data[ib->pos] != 0x1Bu)
            return;

        /* Compact consumed bytes away so there is room to append. */
        if (ib->pos > 0)
            {
                memmove (ib->data, ib->data + ib->pos,
                         (size_t)(ib->len - ib->pos));
                ib->len -= ib->pos;
                ib->pos = 0;
            }

        while (ib->len < ZETUI__IBUF
               && zetui__esc_incomplete (ib->data, ib->len))
            {
                if (zetui__ibuf_read (ib, ctx->fd_in, ZETUI__ESC_WAIT_MS,
                                      &ctx->select_mask)
                    <= 0)
                    break;
            }
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

        if (c == 0x1Bu)
            {
                int rem, i;
                const unsigned char *buf;

                rem = ib->len - ib->pos;
                buf = ib->data + ib->pos;

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

                /* SGR mouse report: ESC [ < Cb ; Cx ; Cy (M|m) */
                if (rem >= 3 && buf[1] == (unsigned char)'['
                    && buf[2] == (unsigned char)'<')
                    {
                        int code, mx, my, j;
                        unsigned char final;

                        code = mx = my = 0;
                        j = 3;
                        while (j < rem && buf[j] >= (unsigned char)'0'
                               && buf[j] <= (unsigned char)'9')
                            code = code * 10 + (buf[j++] - '0');
                        if (j < rem && buf[j] == (unsigned char)';')
                            j++;
                        while (j < rem && buf[j] >= (unsigned char)'0'
                               && buf[j] <= (unsigned char)'9')
                            mx = mx * 10 + (buf[j++] - '0');
                        if (j < rem && buf[j] == (unsigned char)';')
                            j++;
                        while (j < rem && buf[j] >= (unsigned char)'0'
                               && buf[j] <= (unsigned char)'9')
                            my = my * 10 + (buf[j++] - '0');
                        final = (j < rem) ? buf[j] : 0u;

                        if (final == (unsigned char)'M'
                            || final == (unsigned char)'m')
                            {
                                zetui_mouse_event_t *me;

                                ev.type = ZETUI_EVENT_MOUSE;
                                me = &ev.data.mouse;
                                me->x = mx > 0 ? mx - 1 : 0;
                                me->y = my > 0 ? my - 1 : 0;
                                me->mods = ZETUI_MOD_NONE;
                                if (code & 4)
                                    me->mods |= ZETUI_MOD_SHIFT;
                                if (code & 8)
                                    me->mods |= ZETUI_MOD_ALT;
                                if (code & 16)
                                    me->mods |= ZETUI_MOD_CTRL;

                                if (code & 64)
                                    {
                                        me->action = (code & 1)
                                                         ? ZETUI_MOUSE_WHEEL_DOWN
                                                         : ZETUI_MOUSE_WHEEL_UP;
                                        me->button = ZETUI_MOUSE_BUTTON_NONE;
                                    }
                                else
                                    {
                                        switch (code & 3)
                                            {
                                            case 0:
                                                me->button
                                                    = ZETUI_MOUSE_BUTTON_LEFT;
                                                break;
                                            case 1:
                                                me->button
                                                    = ZETUI_MOUSE_BUTTON_MIDDLE;
                                                break;
                                            case 2:
                                                me->button
                                                    = ZETUI_MOUSE_BUTTON_RIGHT;
                                                break;
                                            default:
                                                me->button
                                                    = ZETUI_MOUSE_BUTTON_NONE;
                                                break;
                                            }
                                        if (code & 32)
                                            me->action = ZETUI_MOUSE_MOTION;
                                        else
                                            me->action
                                                = (final == (unsigned char)'M')
                                                      ? ZETUI_MOUSE_PRESS
                                                      : ZETUI_MOUSE_RELEASE;
                                    }
                                ib->pos += j + 1;
                                return ev;
                            }
                        /* malformed: fall through to CSI consumption */
                    }

                /* Focus events: CSI I (gained) and CSI O (lost) */
                if (rem >= 3 && buf[1] == (unsigned char)'[')
                    {
                        if (buf[2] == (unsigned char)'I'
                            || buf[2] == (unsigned char)'O')
                            {
                                ev.type = ZETUI_EVENT_FOCUS;
                                ev.data.focus.focused
                                    = (buf[2] == (unsigned char)'I') ? 1 : 0;
                                ib->pos += 3;
                                return ev;
                            }
                    }

                /* Alt-modified key: terminals prefix the key's bytes
                   with ESC. Consume the prefix, parse the underlying
                   key (sequences included: urxvt sends ESC ESC [ A for
                   Alt+Up) and tag it. ESC [ / ESC O followed by nothing
                   even after the grace wait are Alt+[ / Alt+O rather
                   than torn sequences. */
                if (rem > 1
                    && ((buf[1] != (unsigned char)'['
                         && buf[1] != (unsigned char)'O')
                        || rem == 2))
                    {
                        ib->pos++;
                        ev = zetui__parse (ib);
                        if (ev.type == ZETUI_EVENT_KEY)
                            ev.data.key.mods |= ZETUI_MOD_ALT;
                        return ev;
                    }

                ev.type = ZETUI_EVENT_KEY;
                ev.data.key.key = ZETUI_KEY_ESC;
                ev.data.key.ch = 0x1Bu;
                ev.data.key.mods = ZETUI_MOD_NONE;

                /* Unrecognised but well-formed sequences are consumed
                   whole so their bytes are not replayed as bogus
                   printable-key events. */
                if (rem > 1 && buf[1] == (unsigned char)'[')
                    {
                        for (i = 2; i < rem; i++)
                            if (buf[i] >= 0x40u && buf[i] <= 0x7Eu)
                                {
                                    ib->pos += i + 1;
                                    return ev;
                                }
                    }
                else if (rem > 2 && buf[1] == (unsigned char)'O')
                    {
                        ib->pos += 3;
                        return ev;
                    }

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
        /* Control bytes with a key of their own (Backspace, Tab, Enter)
           are what those keys send; reporting them as Ctrl-modified
           would be wrong. */
        if (c < 32u && c != 8u && c != 9u && c != 13u)
            ev.data.key.mods = ZETUI_MOD_CTRL;
        else
            ev.data.key.mods = ZETUI_MOD_NONE;
        ib->pos++;
        return ev;
    }

    static zetui_event_t
    zetui__resize_ev (zetui_ctx_t *ctx)
    {
        zetui_event_t ev;
        int w, h;

        zetui__resize_flag = 0;
        if (zetui__query_size (ctx->fd_out, &w, &h) == 0
            && zetui__screen_resize (ctx, w, h) == 0)
            {
                /* Both cell buffers are blank again, but the terminal
                   still shows the reflowed pre-resize content; wipe it
                   so the next present() diff starts from a truly blank
                   screen instead of leaving stale cells behind. */
                zetui__write_all (ctx->fd_out, "\033[2J", 4u);
            }

        ev.type = ZETUI_EVENT_RESIZE;
        ev.data.resize.width = ctx->width;
        ev.data.resize.height = ctx->height;
        return ev;
    }

    zetui_event_t
    zetui_poll_event (zetui_ctx_t *ctx)
    {
        zetui_event_t none;

        if (zetui__resize_flag)
            return zetui__resize_ev (ctx);

        if (ctx->in.pos >= ctx->in.len
            && zetui__ibuf_fill (&ctx->in, ctx->fd_in, 0, &ctx->select_mask)
                   <= 0)
            {
                if (zetui__resize_flag)
                    return zetui__resize_ev (ctx);
                memset (&none, 0, sizeof (none));
                none.type = ZETUI_EVENT_NONE;
                return none;
            }

        zetui__ibuf_complete_esc (ctx);
        return zetui__parse (&ctx->in);
    }

    zetui_event_t
    zetui_wait_event (zetui_ctx_t *ctx, int timeout_ms)
    {
        zetui_event_t none;

        if (zetui__resize_flag)
            return zetui__resize_ev (ctx);

        if (ctx->in.pos >= ctx->in.len
            && zetui__ibuf_fill (&ctx->in, ctx->fd_in, timeout_ms,
                                 &ctx->select_mask)
                   <= 0)
            {
                if (zetui__resize_flag)
                    return zetui__resize_ev (ctx);
                memset (&none, 0, sizeof (none));
                none.type = ZETUI_EVENT_NONE;
                return none;
            }

        zetui__ibuf_complete_esc (ctx);
        return zetui__parse (&ctx->in);
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
        size_t avail;
        int bytes, cx, cw;

        if (!str)
            return;
        p = (const unsigned char *)str;
        cx = x;

        while (*p && cx < ctx->width)
            {
                /* Never decode past the NUL terminator: a truncated
                   multi-byte sequence at the end of the string must not
                   read (or skip) beyond it. */
                avail = 1u;
                while (avail < 4u && p[avail] != 0u)
                    avail++;
                bytes = zetui__utf8_dec (p, avail, &cp);
                if (bytes <= 0)
                    break;
                if (cp == (zetui_u32)'\n')
                    break;
                p += bytes;
                cw = zetui_char_width (cp);
                if (cw <= 0)
                    continue; /* combining marks etc. have no cell */
                zetui_set_cell (ctx, cx, y, zetui_cell_make (cp, style));
                cx += cw;
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
                    zetui_box_style_t box, zetui_style_t style)
    {
        const zetui_u32 *t;
        zetui_cell_t c;
        int i;

        if (w < 2 || h < 2)
            return;

        switch (box)
            {
            case ZETUI_BOX_STYLE_HEAVY:
                t = zetui_box_heavy;
                break;
            case ZETUI_BOX_STYLE_DOUBLE:
                t = zetui_box_double;
                break;
            default:
                t = zetui_box_light;
                break;
            }

        c = zetui_cell_make (t[ZETUI_BOX_TL], style);
        zetui_set_cell (ctx, x, y, c);
        c = zetui_cell_make (t[ZETUI_BOX_TR], style);
        zetui_set_cell (ctx, x + w - 1, y, c);
        c = zetui_cell_make (t[ZETUI_BOX_BL], style);
        zetui_set_cell (ctx, x, y + h - 1, c);
        c = zetui_cell_make (t[ZETUI_BOX_BR], style);
        zetui_set_cell (ctx, x + w - 1, y + h - 1, c);

        c = zetui_cell_make (t[ZETUI_BOX_H], style);
        for (i = 1; i < w - 1; i++)
            {
                zetui_set_cell (ctx, x + i, y, c);
                zetui_set_cell (ctx, x + i, y + h - 1, c);
            }

        c = zetui_cell_make (t[ZETUI_BOX_V], style);
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
