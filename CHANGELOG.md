# Changelog

All notable changes to zetui are documented here. Versions correspond to git
tags; this project does not bump `build.zig.zon`'s `.version` field per
release — that field is fixed at `0.1.0` and is not the source of truth.

## [v0.4.1] — 2026-06-16

### Fixed
- **Security:** `zetui_set_title`, `zetui_set_icon_title`, `zetui_set_pointer_shape`,
  and `zetui_register_link` now reject strings containing a C0 control byte
  (including ESC or BEL) or DEL instead of writing them verbatim into an OSC
  escape sequence. Either byte can terminate an OSC sequence early (xterm
  accepts both ST and BEL as terminators) and inject attacker-controlled
  escape sequences — most exploitable via hyperlink URIs, which often
  originate from rendered, untrusted content. `zetui_register_link` now
  returns `0` for such a URI; the other three functions silently no-op.

## [v0.4.0] — 2026-06-14

### Added
- `ZETUI_MOUSE_HOVER` distinguished from `ZETUI_MOUSE_MOTION` (movement with
  no button held vs. drag).
- `zetui_get_link_uri()` — reverse lookup from a hyperlink ID back to its URI.
- `zetui_suspend()` / `zetui_resume()` — SIGTSTP (Ctrl-Z) support: leave the
  alternate screen and raw mode, `raise(SIGSTOP)`, and restore everything
  (including mouse/focus/paste state and a full repaint) on resume.
- `zetui_draw_printf_len()` — printf-style draw clipped to a column budget.
- Styled underlines: double, curly, dotted, dashed, plus an independent
  underline color via SGR 58/59 (**ABI-breaking**: `zetui_style_t` and
  `zetui_cell_t` gained an `ul_color` field).
- `zetui_set_icon_title()` — OSC 1 icon/taskbar title.

### Fixed
- Input buffer (`ZETUI__IBUF`) grown from 64 to 4096 bytes to stop losing
  bytes on large pastes/bursts.

## [v0.3.0] — 2026-06-14

### Added
- Heap-allocated hyperlink URI table (previously fixed-capacity) plus a
  pointer-shape change on hyperlink hover.
- `zetui_cursor_set_shape()` / `zetui_cursor_shape_t` — DECSCUSR cursor shape
  control (block/underline/beam, blinking or steady).
- `ZETUI_KEY_SHIFT_TAB` — Shift+Tab / backtab detection.
- `zetui_invalidate()` — force a full repaint on the next `zetui_present()`.
- `zetui_draw_str_len()` — column-clipped string draw.
- `zetui_draw_printf()` — printf-style draw.
- `zetui_set_clipboard()` — OSC 52 clipboard write (base64-encoded).

### Changed
- Documentation moved from Doxygen to a markdown wiki (`wiki/*.md`), synced
  to the GitHub Wiki via CI on every master push that touches those files.

## [v0.2.0] — 2026-06-14

### Added
- OSC 8 hyperlinks: `zetui_register_link()` deduplicates URIs into a stable
  1-based ID registry; `zetui_draw_link()` draws a string and stamps every
  produced cell with the link ID in one call. `zetui_present()` emits the
  OSC 8 begin/end sequences during differential rendering and closes any
  open link at the end of every frame.

## [v0.1.0] — initial release

### Added
- Single-header, C89/POSIX terminal UI library with double-buffered,
  diff-only rendering.
- Unicode-aware drawing: UTF-8 decoding, wide-character (CJK/emoji) width,
  zero-width combining marks.
- Input: keyboard with special keys + modifiers, Alt-key detection, mouse
  (SGR protocol), terminal resize (SIGWINCH), focus in/out, bracketed paste.
- Colors: 16-color ANSI, then 24-bit RGB truecolor; `zetui_color_support()`
  capability detection.
- `zetui_draw_box()` with light/heavy/double box-drawing styles.
- `zetui_set_title()` — OSC 2 window title.
- Idiomatic Zig bindings (`bindings/zetui.zig`) alongside the C demos.
- Migrated build to the Zig 0.16 build API.
