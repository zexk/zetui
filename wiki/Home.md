# zetui

ANSI/C89 terminal UI library — single-header, STB-style, with first-class Zig bindings.

## Features

- **Single-header** — drop `include/zetui.h` into your project, define `ZETUI_IMPLEMENTATION` once
- **C89 / POSIX** — no C99, no `<stdint.h>`, no platform-specific headers beyond POSIX.1-2008
- **Double-buffered rendering** — only changed cells are flushed; no flicker; `zetui_invalidate()` forces a full repaint
- **Unicode-aware** — full UTF-8 drawing, wide-character (CJK/emoji) support, zero-width combining marks
- **Input** — keyboard (special keys + modifiers + Shift+Tab), mouse (SGR, all-motion hover), resize (SIGWINCH), focus, bracketed paste
- **Colors** — 16-color ANSI, 256-color xterm palette (`ZETUI_COLOR_256`), 24-bit RGB truecolor
- **Box drawing** — light / heavy / double Unicode line characters
- **Drawing helpers** — column-clipped draw (`zetui_draw_str_len`), printf-style draw (`zetui_draw_printf`)
- **OSC 8 hyperlinks** — clickable text; heap-managed URI table; hover hit-test via `zetui_get_link_at`
- **Cursor shape** — DECSCUSR block / underline / beam (blinking or steady) via `zetui_cursor_set_shape`
- **Mouse pointer shape** — OSC 22 pointer sprite via `zetui_set_pointer_shape`
- **Terminal title** — OSC 2 title setting
- **Clipboard** — OSC 52 clipboard write via `zetui_set_clipboard`
- **Zig bindings** — idiomatic wrapper in `bindings/zetui.zig`

## Requirements

| | |
|---|---|
| OS | POSIX.1-2008 (Linux, macOS, BSDs) |
| C compiler | Any C89-compliant compiler (gcc, clang) |
| Build system | Zig ≥ 0.16.0 (optional, for the static library and Zig bindings) |

## Pages

- [Getting Started](Getting-Started) — installation, minimal examples, build targets
- [API Reference](API-Reference) — complete C API
- [Zig Bindings](Zig-Bindings) — idiomatic Zig wrapper

## License

Unlicense — public domain. See <https://unlicense.org>.
