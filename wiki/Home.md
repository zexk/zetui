# zetui

ANSI/C89 terminal UI library — single-header, STB-style, with first-class Zig bindings.

## Features

- **Single-header** — drop `include/zetui.h` into your project, define `ZETUI_IMPLEMENTATION` once
- **C89 / POSIX** — no C99, no `<stdint.h>`, no platform-specific headers beyond POSIX.1-2008
- **Double-buffered rendering** — only changed cells are flushed; no flicker
- **Unicode-aware** — full UTF-8 drawing, wide-character (CJK/emoji) support, zero-width combining marks
- **Input** — keyboard (special keys + modifiers), mouse (SGR), resize (SIGWINCH), focus, bracketed paste
- **Colors** — 16-color ANSI, 256-color xterm palette, 24-bit RGB truecolor
- **Box drawing** — light / heavy / double Unicode line characters
- **OSC 8 hyperlinks** — clickable text in supporting terminals
- **Terminal title** — OSC 2 title setting
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
