# API Reference (C)

All declarations live in `include/zetui.h`. The implementation is compiled in exactly one translation unit via `#define ZETUI_IMPLEMENTATION`.

---

## Error codes — `zetui_error_t`

```c
typedef enum zetui_error {
    ZETUI_OK             =  0,
    ZETUI_ERR_NOMEM      = -1,
    ZETUI_ERR_IO         = -2,
    ZETUI_ERR_NOT_A_TTY  = -3,
    ZETUI_ERR_UNSUPPORTED= -4,
    ZETUI_ERR_TIMEOUT    = -5,
} zetui_error_t;
```

---

## Color

### `zetui_color_support_t`

Detected at `zetui_init()` time. Read via [`zetui_color_support()`](#zetui_color_support).

```c
typedef enum zetui_color_support {
    ZETUI_COLOR_16        = 0,  // 16 ANSI colors
    ZETUI_COLOR_256       = 1,  // 256-color xterm palette
    ZETUI_COLOR_TRUECOLOR = 2,  // 24-bit RGB
} zetui_color_support_t;
```

Detection order: `COLORTERM=truecolor|24bit` → `TERM` contains `256color` → fallback to 16.

### `zetui_color_t`

```c
typedef enum zetui_color {
    ZETUI_COLOR_DEFAULT      = -1,
    ZETUI_COLOR_BLACK        =  0,
    ZETUI_COLOR_RED          =  1,
    ZETUI_COLOR_GREEN        =  2,
    ZETUI_COLOR_YELLOW       =  3,
    ZETUI_COLOR_BLUE         =  4,
    ZETUI_COLOR_MAGENTA      =  5,
    ZETUI_COLOR_CYAN         =  6,
    ZETUI_COLOR_WHITE        =  7,
    ZETUI_COLOR_BRIGHT_BLACK =  8,
    ZETUI_COLOR_BRIGHT_RED   =  9,
    ZETUI_COLOR_BRIGHT_GREEN = 10,
    ZETUI_COLOR_BRIGHT_YELLOW= 11,
    ZETUI_COLOR_BRIGHT_BLUE  = 12,
    ZETUI_COLOR_BRIGHT_MAGENTA=13,
    ZETUI_COLOR_BRIGHT_CYAN  = 14,
    ZETUI_COLOR_BRIGHT_WHITE = 15,
} zetui_color_t;
```

`ZETUI_COLOR_DEFAULT` leaves the terminal's default color in place.

### `ZETUI_COLOR_RGB(r, g, b)`

Encode a 24-bit color into a value compatible with `zetui_i32 fg/bg` fields:

```c
zetui_style_t s;
s.fg = ZETUI_COLOR_RGB(255, 128, 0);  // orange foreground
s.bg = ZETUI_COLOR_DEFAULT;
```

Test with `ZETUI_COLOR_IS_RGB(c)` (non-zero if created by `ZETUI_COLOR_RGB`).

### `ZETUI_COLOR_256(n)`

Encode a 256-color xterm palette index (0–255) into a `zetui_i32 fg/bg` field. Emits `\033[38;5;Nm` / `\033[48;5;Nm`. Check first with `zetui_color_support()` returning `ZETUI_COLOR_256` or higher.

```c
zetui_style_t s;
s.fg = ZETUI_COLOR_256(202);  // xterm orange
```

Test with `ZETUI_COLOR_IS_256(c)`.

---

## Cell attributes

Bitfield flags OR-combined into `zetui_style_t.attrs`:

| Constant | Effect |
|---|---|
| `ZETUI_ATTR_NONE`      | No attributes |
| `ZETUI_ATTR_BOLD`      | Bold / increased intensity |
| `ZETUI_ATTR_DIM`       | Dim / decreased intensity |
| `ZETUI_ATTR_ITALIC`    | Italic |
| `ZETUI_ATTR_UNDERLINE` | Single underline |
| `ZETUI_ATTR_BLINK`     | Slow blink |
| `ZETUI_ATTR_REVERSE`   | Swap foreground and background |
| `ZETUI_ATTR_HIDDEN`    | Invisible text |
| `ZETUI_ATTR_STRIKE`    | Strikethrough |

---

## Cell — `zetui_cell_t`

```c
typedef struct zetui_cell {
    zetui_u32 ch;    // Unicode codepoint; 0 = space; ZETUI_WIDE_PAD = wide-char tail
    zetui_i32 fg;    // Foreground: zetui_color_t or ZETUI_COLOR_DEFAULT
    zetui_i32 bg;    // Background: zetui_color_t or ZETUI_COLOR_DEFAULT
    zetui_u32 attrs; // ZETUI_ATTR_* flags OR-combined
    int       link;  // Hyperlink ID from zetui_register_link(); 0 = none
} zetui_cell_t;
```

`ZETUI_WIDE_PAD` (`0xFFFFFFFF`) is placed in the cell to the right of a wide (two-column) character. `zetui_set_cell()` maintains the pairing automatically.

---

## Style — `zetui_style_t`

```c
typedef struct zetui_style {
    zetui_i32 fg;
    zetui_i32 bg;
    zetui_u32 attrs;
} zetui_style_t;
```

Zero-initialize for terminal defaults with no attributes.

---

## Key codes — `zetui_key_t`

`zetui_key_event_t.key` is `ZETUI_KEY_NONE` for printable characters; check `.ch` (Unicode codepoint) instead.

Special keys: `ZETUI_KEY_CTRL_A`…`ZETUI_KEY_CTRL_Z`, `ZETUI_KEY_BACKSPACE` (8), `ZETUI_KEY_TAB` (9), `ZETUI_KEY_ENTER` (13), `ZETUI_KEY_ESC` (27), `ZETUI_KEY_DEL` (127), arrow keys, Home, End, Page Up/Down, Insert, Delete, F1–F12, `ZETUI_KEY_SHIFT_TAB` (278, `ESC [ Z`).

## Key modifiers

OR-combined in `zetui_key_event_t.mods` and `zetui_mouse_event_t.mods`:

| Constant | Key |
|---|---|
| `ZETUI_MOD_NONE`  | No modifier |
| `ZETUI_MOD_SHIFT` | Shift |
| `ZETUI_MOD_ALT`   | Alt / Meta |
| `ZETUI_MOD_CTRL`  | Ctrl |

---

## Events

### `zetui_event_type_t`

```c
typedef enum zetui_event_type {
    ZETUI_EVENT_NONE   = 0,  // no event (poll returned immediately)
    ZETUI_EVENT_KEY    = 1,  // keyboard; see data.key
    ZETUI_EVENT_RESIZE = 2,  // terminal resized; see data.resize
    ZETUI_EVENT_MOUSE  = 3,  // mouse; see data.mouse
    ZETUI_EVENT_FOCUS  = 4,  // focus gained/lost; see data.focus
    ZETUI_EVENT_PASTE  = 5,  // bracketed paste boundary; see data.paste
} zetui_event_type_t;
```

### `zetui_event_t`

```c
typedef struct zetui_event {
    zetui_event_type_t type;
    zetui_event_data_t data;   // union: .key / .resize / .mouse / .focus / .paste
} zetui_event_t;
```

### Payload types

**`zetui_key_event_t`**
```c
zetui_key_t key;   // special key, or ZETUI_KEY_NONE for printable
zetui_u32   ch;    // Unicode codepoint (printable chars)
zetui_u32   mods;  // ZETUI_MOD_* flags
```

**`zetui_resize_event_t`**
```c
int width;   // new terminal width in columns
int height;  // new terminal height in rows
```

**`zetui_mouse_event_t`** (requires `zetui_mouse_enable()`)
```c
zetui_mouse_action_t action;  // PRESS / RELEASE / MOTION / WHEEL_UP / WHEEL_DOWN / HOVER
zetui_mouse_button_t button;  // NONE / LEFT / MIDDLE / RIGHT
int x;         // column (0-based)
int y;         // row (0-based)
zetui_u32 mods;
```

**`zetui_focus_event_t`** (requires `zetui_focus_enable()`)
```c
int focused;  // non-zero = gained focus
```

**`zetui_paste_event_t`** (requires `zetui_paste_enable()`)
```c
int begin;  // non-zero = paste started; zero = paste ended
```

Key events between the two paste boundaries carry pasted text and should bypass normal keybinding processing.

---

## Box drawing

### Box style

```c
typedef enum zetui_box_style {
    ZETUI_BOX_STYLE_LIGHT  = 0,  // ┌─┐
    ZETUI_BOX_STYLE_HEAVY  = 1,  // ┏━┓
    ZETUI_BOX_STYLE_DOUBLE = 2,  // ╔═╗
} zetui_box_style_t;
```

### Box index constants

Used to index into `zetui_box_light[]`, `zetui_box_heavy[]`, `zetui_box_double[]`:

| Constant | Position | Light | Heavy | Double |
|---|---|---|---|---|
| `ZETUI_BOX_TL` | Top-left | ┌ | ┏ | ╔ |
| `ZETUI_BOX_TR` | Top-right | ┐ | ┓ | ╗ |
| `ZETUI_BOX_BL` | Bottom-left | └ | ┗ | ╚ |
| `ZETUI_BOX_BR` | Bottom-right | ┘ | ┛ | ╝ |
| `ZETUI_BOX_H`  | Horizontal | ─ | ━ | ═ |
| `ZETUI_BOX_V`  | Vertical | │ | ┃ | ║ |
| `ZETUI_BOX_LT` | Left T | ├ | ┣ | ╠ |
| `ZETUI_BOX_RT` | Right T | ┤ | ┫ | ╣ |
| `ZETUI_BOX_TT` | Top T | ┬ | ┳ | ╦ |
| `ZETUI_BOX_BT` | Bottom T | ┴ | ┻ | ╩ |
| `ZETUI_BOX_X`  | Cross | ┼ | ╋ | ╬ |

---

## Functions

### Lifecycle

#### `zetui_init`
```c
zetui_ctx_t *zetui_init(void);
```
Enter raw mode and alternate screen. Returns `NULL` if stdout is not a TTY or on error.

#### `zetui_init_ex`
```c
zetui_ctx_t *zetui_init_ex(zetui_error_t *err);
```
Same as `zetui_init()` but stores the failure reason in `*err` (`ZETUI_ERR_NOMEM`, `ZETUI_ERR_NOT_A_TTY`, or `ZETUI_ERR_IO`). Pass `NULL` for `err` to ignore.

#### `zetui_shutdown`
```c
void zetui_shutdown(zetui_ctx_t *ctx);
```
Restore terminal state, disable all optional modes, and free `ctx`. Safe to call with `NULL`.

#### `zetui_suspend`
```c
void zetui_suspend(zetui_ctx_t *ctx);
```
Suspend the TUI: disables mouse/focus/paste reporting, resets cursor and pointer shapes, leaves the alternate screen, restores the original termios, then raises `SIGSTOP`. The process is stopped until the shell sends `SIGCONT` (e.g. `fg`). Call `zetui_resume()` immediately after the process continues.

#### `zetui_resume`
```c
void zetui_resume(zetui_ctx_t *ctx);
```
Resume the TUI after `zetui_suspend()`: re-enters raw mode, switches back to the alternate screen, and re-enables whichever of mouse/focus/paste were active before the suspend. Calls `zetui_invalidate()` so the next `zetui_present()` redraws the full screen.

---

### Terminal info

#### `zetui_width`
```c
int zetui_width(const zetui_ctx_t *ctx);
```
Current terminal width in columns. Updated after each `ZETUI_EVENT_RESIZE`.

#### `zetui_height`
```c
int zetui_height(const zetui_ctx_t *ctx);
```
Current terminal height in rows.

#### `zetui_color_support`
```c
zetui_color_support_t zetui_color_support(const zetui_ctx_t *ctx);
```
Color capability detected at init time. Use this to decide whether to pass `ZETUI_COLOR_RGB` values or fall back to palette colors.

---

### Back-buffer drawing

All draw calls write to the back buffer. Changes are only visible after `zetui_present()`.

#### `zetui_clear`
```c
void zetui_clear(zetui_ctx_t *ctx);
```
Fill the back buffer with blank cells (terminal default style).

#### `zetui_invalidate`
```c
void zetui_invalidate(zetui_ctx_t *ctx);
```
Blank the front buffer so the next `zetui_present()` redraws every cell unconditionally. Use after SIGTSTP/SIGCONT or any event that may have corrupted the visible terminal state.

#### `zetui_set_cell`
```c
void zetui_set_cell(zetui_ctx_t *ctx, int x, int y, zetui_cell_t cell);
```
Write one cell to the back buffer. Out-of-bounds coordinates are silently ignored. Wide codepoints (`zetui_char_width() == 2`) also claim the cell to their right; overwriting either half of an existing wide character truncates it to a space.

#### `zetui_get_cell`
```c
zetui_cell_t zetui_get_cell(const zetui_ctx_t *ctx, int x, int y);
```
Read a cell from the back buffer. Returns a blank cell for out-of-bounds coordinates.

#### `zetui_present`
```c
zetui_error_t zetui_present(zetui_ctx_t *ctx);
```
Diff back buffer against front buffer, flush only changed cells to the terminal, then swap the buffers. Returns `ZETUI_OK` or `ZETUI_ERR_IO`.

---

### Input

#### `zetui_poll_event`
```c
zetui_event_t zetui_poll_event(zetui_ctx_t *ctx);
```
Non-blocking. Returns an event with `type == ZETUI_EVENT_NONE` if no input is available.

#### `zetui_wait_event`
```c
zetui_event_t zetui_wait_event(zetui_ctx_t *ctx, int timeout_ms);
```
Blocking wait. `timeout_ms = -1` blocks indefinitely. Returns `type == ZETUI_EVENT_NONE` on timeout.

#### `zetui_mouse_enable` / `zetui_mouse_disable`
```c
void zetui_mouse_enable(zetui_ctx_t *ctx);
void zetui_mouse_disable(zetui_ctx_t *ctx);
```
Enable/disable SGR mouse reporting (button presses, releases, wheel, and all cursor motion including hover). Off by default because mouse reporting takes over the terminal's native text selection. Automatically disabled by `zetui_shutdown()`.

#### `zetui_focus_enable` / `zetui_focus_disable`
```c
void zetui_focus_enable(zetui_ctx_t *ctx);
void zetui_focus_disable(zetui_ctx_t *ctx);
```
Enable/disable `ZETUI_EVENT_FOCUS` events when the terminal window gains or loses focus. Off by default.

#### `zetui_paste_enable` / `zetui_paste_disable`
```c
void zetui_paste_enable(zetui_ctx_t *ctx);
void zetui_paste_disable(zetui_ctx_t *ctx);
```
Enable/disable bracketed paste mode. When enabled, paste operations are wrapped with `ZETUI_EVENT_PASTE` (begin=1) / `ZETUI_EVENT_PASTE` (begin=0). Off by default.

---

### Drawing helpers

#### `zetui_style_default`
```c
zetui_style_t zetui_style_default(void);
```
Return a style with terminal-default foreground/background and no attributes.

#### `zetui_cell_make`
```c
zetui_cell_t zetui_cell_make(zetui_u32 ch, zetui_style_t style);
```
Construct a `zetui_cell_t` from a codepoint and style.

#### `zetui_char_width`
```c
int zetui_char_width(zetui_u32 cp);
```
Display width of a Unicode codepoint in terminal cells: `2` for wide (CJK, most emoji), `0` for zero-width (combining marks, joiners, variation selectors), `1` otherwise.

#### `zetui_draw_str`
```c
void zetui_draw_str(zetui_ctx_t *ctx, int x, int y,
                    const char *str, zetui_style_t style);
```
Draw a NUL-terminated UTF-8 string starting at `(x, y)`. The cursor advances by `zetui_char_width()` per codepoint; zero-width codepoints are skipped.

#### `zetui_draw_str_len`
```c
int zetui_draw_str_len(zetui_ctx_t *ctx, int x, int y,
                       const char *str, int max_cols,
                       zetui_style_t style);
```
Like `zetui_draw_str` but stops after `max_cols` display columns. Wide characters that would straddle the limit are not drawn. Pass `max_cols < 0` for unlimited (equivalent to `zetui_draw_str`). Returns the number of display columns written.

#### `zetui_draw_printf`
```c
void zetui_draw_printf(zetui_ctx_t *ctx, int x, int y,
                       zetui_style_t style, const char *fmt, ...);
```
Format a string with `printf`-style arguments and draw it at `(x, y)`. Internally formats into a 4096-byte stack buffer; output is silently truncated if longer.

#### `zetui_draw_box`
```c
void zetui_draw_box(zetui_ctx_t *ctx, int x, int y, int w, int h,
                    zetui_box_style_t box, zetui_style_t style);
```
Draw a box-drawing rectangle. `w` and `h` must each be ≥ 2.

#### `zetui_fill_rect`
```c
void zetui_fill_rect(zetui_ctx_t *ctx, int x, int y, int w, int h,
                     zetui_cell_t cell);
```
Fill a rectangular region with a single cell.

#### `zetui_draw_hline`
```c
void zetui_draw_hline(zetui_ctx_t *ctx, int x, int y, int len,
                      zetui_u32 ch, zetui_style_t style);
```
Draw a horizontal line of `len` cells filled with codepoint `ch`.

#### `zetui_draw_vline`
```c
void zetui_draw_vline(zetui_ctx_t *ctx, int x, int y, int len,
                      zetui_u32 ch, zetui_style_t style);
```
Draw a vertical line of `len` cells filled with codepoint `ch`.

---

### Cursor

#### `zetui_cursor_hide` / `zetui_cursor_show`
```c
void zetui_cursor_hide(zetui_ctx_t *ctx);
void zetui_cursor_show(zetui_ctx_t *ctx);
```
Hide or show the hardware cursor. Most TUI apps hide it immediately after init.

#### `zetui_cursor_move`
```c
void zetui_cursor_move(zetui_ctx_t *ctx, int x, int y);
```
Move the hardware cursor to column `x`, row `y` (both 0-based).

#### `zetui_cursor_set_shape`
```c
typedef enum zetui_cursor_shape {
    ZETUI_CURSOR_DEFAULT          = 0,
    ZETUI_CURSOR_BLOCK_BLINK      = 1,
    ZETUI_CURSOR_BLOCK_STEADY     = 2,
    ZETUI_CURSOR_UNDERLINE_BLINK  = 3,
    ZETUI_CURSOR_UNDERLINE_STEADY = 4,
    ZETUI_CURSOR_BEAM_BLINK       = 5,
    ZETUI_CURSOR_BEAM_STEADY      = 6,
} zetui_cursor_shape_t;

void zetui_cursor_set_shape(zetui_ctx_t *ctx, zetui_cursor_shape_t shape);
```
Set the hardware cursor shape via DECSCUSR (`CSI Ps SP q`). Takes effect immediately. `zetui_shutdown()` resets to `ZETUI_CURSOR_DEFAULT` automatically. Supported by most modern terminals (xterm, kitty, foot, alacritty).

---

### Terminal window

#### `zetui_set_title`
```c
void zetui_set_title(zetui_ctx_t *ctx, const char *title);
```
Set the terminal window title via OSC 2. The string must be NUL-terminated and must not contain control bytes. Accepted by most modern terminal emulators.

#### `zetui_set_clipboard`
```c
void zetui_set_clipboard(zetui_ctx_t *ctx, const char *str);
```
Copy a NUL-terminated UTF-8 string to the system clipboard via OSC 52. Internally base64-encodes `str` and emits `OSC 52 ; c ; <b64> ST`. Supported by most modern terminals (alacritty, kitty, xterm, foot); may be blocked in multiplexers without pass-through configured. Passing `NULL` or an empty string clears the clipboard selection.

#### `zetui_set_pointer_shape`
```c
void zetui_set_pointer_shape(zetui_ctx_t *ctx, const char *name);
```
Change the mouse pointer sprite via OSC 22. `name` is an X11 cursor name such as `"pointer"` (hand) or `"default"` (arrow). Honoured by kitty, foot, and other modern terminals; silently ignored by terminals that do not support OSC 22. `zetui_shutdown()` resets the pointer automatically.

#### `zetui_register_link`
```c
int zetui_register_link(zetui_ctx_t *ctx, const char *uri);
```
Register a hyperlink URI and return a stable ID (1-based). Registering the same URI twice returns the same ID. The library copies `uri` internally — the caller does not need to keep it alive after this call. Returns 0 on failure (`NULL` uri or out of memory). The URI table grows dynamically with no hard cap.

#### `zetui_get_link_at`
```c
int zetui_get_link_at(const zetui_ctx_t *ctx, int x, int y);
```
Return the hyperlink ID of the cell currently visible at `(x, y)` by reading the front buffer. Returns 0 if no link is active at that cell or if the coordinates are out of bounds. Use this to hit-test mouse position against rendered hyperlinks.

#### `zetui_get_link_uri`
```c
const char *zetui_get_link_uri(const zetui_ctx_t *ctx, int id);
```
Reverse-lookup: given a link ID returned by `zetui_register_link()`, return the registered URI string. Returns `NULL` if `id` is out of range. The pointer is owned by the library — do not free or mutate it.

#### `zetui_draw_link`
```c
void zetui_draw_link(zetui_ctx_t *ctx, int x, int y,
                     const char *str, const char *uri,
                     zetui_style_t style);
```
Draw a UTF-8 string with an OSC 8 hyperlink attached. `zetui_present()` emits the OSC 8 begin/end escape sequences during differential rendering.
