# Zig Bindings

`bindings/zetui.zig` provides an idiomatic Zig wrapper around the C library. It is exposed as a Zig module named `zetui` via `build.zig`.

## Adding as a dependency

In your project's `build.zig.zon`:

```zig
.dependencies = .{
    .zetui = .{
        .url = "https://github.com/zexk/zetui/archive/<commit-or-tag>.tar.gz",
        .hash = "<hash>",
    },
},
```

In `build.zig`:

```zig
const zetui_dep = b.dependency("zetui", .{ .target = target, .optimize = optimize });
exe.root_module.addImport("zetui", zetui_dep.module("zetui"));
```

---

## Type mapping

| C | Zig |
|---|---|
| `zetui_error_t` | `zetui.Error` (Zig error set) |
| `zetui_color_support_t` | `zetui.ColorSupport` |
| `zetui_color_t` | `zetui.Color` |
| `ZETUI_ATTR_*` | `zetui.Attr.bold`, `.dim`, … |
| `ZETUI_MOD_*` | `zetui.Mod.shift`, `.alt`, `.ctrl` |
| `zetui_key_t` | `zetui.Key` |
| `zetui_cell_t` | `zetui.Cell` |
| `zetui_style_t` | `zetui.Style` |
| `zetui_event_t` | `zetui.Event` (tagged union) |
| `zetui_key_event_t` | `zetui.KeyEvent` |
| `zetui_resize_event_t` | `zetui.ResizeEvent` |
| `zetui_mouse_event_t` | `zetui.MouseEvent` |
| `zetui_focus_event_t` | `zetui.FocusEvent` |
| `zetui_paste_event_t` | `zetui.PasteEvent` |
| `zetui_box_style_t` | `zetui.BoxVariant` |
| `ZETUI_BOX_*` | `zetui.Box.tl`, `.tr`, … |
| `ZETUI_WIDE_PAD` | `zetui.wide_pad` |
| `zetui_ctx_t *` | `zetui.Context` |

---

## `zetui.Style`

The Zig `Style` struct extends the C struct with optional true-color fields:

```zig
pub const Style = struct {
    fg:     Color = .default,
    bg:     Color = .default,
    attrs:  u32   = Attr.none,
    rgb_fg: ?Rgb  = null,   // overrides fg when set
    rgb_bg: ?Rgb  = null,   // overrides bg when set
};
```

Use `zetui.rgb(r, g, b)` to construct an `Rgb` value:

```zig
const orange = zetui.Style{
    .rgb_fg = zetui.rgb(255, 128, 0),
    .attrs   = zetui.Attr.bold,
};
tui.drawStr(1, 1, "orange bold text", orange);
```

---

## `zetui.Context` methods

| Method | C equivalent |
|---|---|
| `Context.init() !Context` | `zetui_init_ex()` |
| `ctx.deinit()` | `zetui_shutdown()` |
| `ctx.width() i32` | `zetui_width()` |
| `ctx.height() i32` | `zetui_height()` |
| `ctx.colorSupport() ColorSupport` | `zetui_color_support()` |
| `ctx.clear()` | `zetui_clear()` |
| `ctx.present() !void` | `zetui_present()` |
| `ctx.setCell(x, y, cell)` | `zetui_set_cell()` |
| `ctx.getCell(x, y) Cell` | `zetui_get_cell()` |
| `ctx.pollEvent() Event` | `zetui_poll_event()` |
| `ctx.waitEvent(ms) Event` | `zetui_wait_event()` |
| `ctx.mouseEnable()` | `zetui_mouse_enable()` |
| `ctx.mouseDisable()` | `zetui_mouse_disable()` |
| `ctx.focusEnable()` | `zetui_focus_enable()` |
| `ctx.focusDisable()` | `zetui_focus_disable()` |
| `ctx.pasteEnable()` | `zetui_paste_enable()` |
| `ctx.pasteDisable()` | `zetui_paste_disable()` |
| `ctx.drawStr(x, y, str, style)` | `zetui_draw_str()` |
| `ctx.drawSlice(x, y, slice, style)` | `zetui_draw_str()` (copies to stack buf) |
| `ctx.drawBox(x, y, w, h, variant, style)` | `zetui_draw_box()` |
| `ctx.fillRect(x, y, w, h, cell)` | `zetui_fill_rect()` |
| `ctx.drawHline(x, y, len, ch, style)` | `zetui_draw_hline()` |
| `ctx.drawVline(x, y, len, ch, style)` | `zetui_draw_vline()` |
| `ctx.cursorHide()` | `zetui_cursor_hide()` |
| `ctx.cursorShow()` | `zetui_cursor_show()` |
| `ctx.cursorMove(x, y)` | `zetui_cursor_move()` |
| `ctx.setTitle(title)` | `zetui_set_title()` |
| `ctx.setTitleSlice(slice)` | `zetui_set_title()` (copies to stack buf) |
| `ctx.registerLink(uri) c_int` | `zetui_register_link()` |
| `ctx.drawLink(x, y, str, uri, style)` | `zetui_draw_link()` |

---

## Event handling

`Event` is a tagged union; use a `switch` on it:

```zig
switch (tui.waitEvent(-1)) {
    .key => |ke| {
        if (ke.key == .ctrl_c or ke.ch == 'q') running = false;
    },
    .resize => |re| {
        _ = re.width; _ = re.height; // re-draw at new size
    },
    .mouse => |me| {
        if (me.action == .press and me.button == .left) { /* ... */ }
    },
    .focus => |fe| {
        _ = fe.focused; // bool
    },
    .paste => |pe| {
        _ = pe.begin; // bool: true = start, false = end
    },
    .none => {},
}
```

## Utility functions

```zig
// Display width of a Unicode codepoint (0, 1, or 2 columns)
zetui.charWidth(cp: u21) i32

// Construct an Rgb value
zetui.rgb(r: u8, g: u8, b: u8) Rgb

// Get the codepoint at index idx from the given box table
zetui.boxCp(variant: BoxVariant, idx: usize) u21
```
