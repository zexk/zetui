/// zetui — idiomatic Zig bindings for the zetui C library.
///
/// Example:
///   var tui = try zetui.Context.init();
///   defer tui.deinit();
///   tui.clear();
///   tui.drawStr(1, 1, "Hello!", .{});
///   try tui.present();
///   _ = tui.waitEvent(-1);
const std = @import("std");

const c = @cImport({
    @cInclude("zetui.h");
});

// ------------------------------------------------------------------ //
// Re-export C constants and enums with Zig naming                    //
// ------------------------------------------------------------------ //

pub const CursorShape = enum(c_int) {
    default           = c.ZETUI_CURSOR_DEFAULT,
    block_blink       = c.ZETUI_CURSOR_BLOCK_BLINK,
    block_steady      = c.ZETUI_CURSOR_BLOCK_STEADY,
    underline_blink   = c.ZETUI_CURSOR_UNDERLINE_BLINK,
    underline_steady  = c.ZETUI_CURSOR_UNDERLINE_STEADY,
    beam_blink        = c.ZETUI_CURSOR_BEAM_BLINK,
    beam_steady       = c.ZETUI_CURSOR_BEAM_STEADY,
};

pub const ColorSupport = enum(c_int) {
    @"16"       = c.ZETUI_COLOR_16,
    @"256"      = c.ZETUI_COLOR_256,
    truecolor   = c.ZETUI_COLOR_TRUECOLOR,
};

pub const Color = enum(i32) {
    default = c.ZETUI_COLOR_DEFAULT,
    black = c.ZETUI_COLOR_BLACK,
    red = c.ZETUI_COLOR_RED,
    green = c.ZETUI_COLOR_GREEN,
    yellow = c.ZETUI_COLOR_YELLOW,
    blue = c.ZETUI_COLOR_BLUE,
    magenta = c.ZETUI_COLOR_MAGENTA,
    cyan = c.ZETUI_COLOR_CYAN,
    white = c.ZETUI_COLOR_WHITE,
    bright_black = c.ZETUI_COLOR_BRIGHT_BLACK,
    bright_red = c.ZETUI_COLOR_BRIGHT_RED,
    bright_green = c.ZETUI_COLOR_BRIGHT_GREEN,
    bright_yellow = c.ZETUI_COLOR_BRIGHT_YELLOW,
    bright_blue = c.ZETUI_COLOR_BRIGHT_BLUE,
    bright_magenta = c.ZETUI_COLOR_BRIGHT_MAGENTA,
    bright_cyan = c.ZETUI_COLOR_BRIGHT_CYAN,
    bright_white = c.ZETUI_COLOR_BRIGHT_WHITE,

    pub fn toC(self: Color) i32 {
        return @intFromEnum(self);
    }
};

pub const Attr = struct {
    pub const none = c.ZETUI_ATTR_NONE;
    pub const bold = c.ZETUI_ATTR_BOLD;
    pub const dim = c.ZETUI_ATTR_DIM;
    pub const italic = c.ZETUI_ATTR_ITALIC;
    pub const underline = c.ZETUI_ATTR_UNDERLINE;
    pub const blink = c.ZETUI_ATTR_BLINK;
    pub const reverse = c.ZETUI_ATTR_REVERSE;
    pub const hidden = c.ZETUI_ATTR_HIDDEN;
    pub const strike = c.ZETUI_ATTR_STRIKE;
};

pub const Mod = struct {
    pub const none = c.ZETUI_MOD_NONE;
    pub const shift = c.ZETUI_MOD_SHIFT;
    pub const alt = c.ZETUI_MOD_ALT;
    pub const ctrl = c.ZETUI_MOD_CTRL;
};

pub const Key = enum(c_int) {
    none = c.ZETUI_KEY_NONE,
    ctrl_a = c.ZETUI_KEY_CTRL_A,
    ctrl_b = c.ZETUI_KEY_CTRL_B,
    ctrl_c = c.ZETUI_KEY_CTRL_C,
    ctrl_d = c.ZETUI_KEY_CTRL_D,
    ctrl_e = c.ZETUI_KEY_CTRL_E,
    ctrl_f = c.ZETUI_KEY_CTRL_F,
    ctrl_g = c.ZETUI_KEY_CTRL_G,
    backspace = c.ZETUI_KEY_BACKSPACE,
    tab = c.ZETUI_KEY_TAB,
    ctrl_j = c.ZETUI_KEY_CTRL_J,
    enter = c.ZETUI_KEY_ENTER,
    ctrl_k = c.ZETUI_KEY_CTRL_K,
    ctrl_l = c.ZETUI_KEY_CTRL_L,
    ctrl_n = c.ZETUI_KEY_CTRL_N,
    ctrl_o = c.ZETUI_KEY_CTRL_O,
    ctrl_p = c.ZETUI_KEY_CTRL_P,
    ctrl_q = c.ZETUI_KEY_CTRL_Q,
    ctrl_r = c.ZETUI_KEY_CTRL_R,
    ctrl_s = c.ZETUI_KEY_CTRL_S,
    ctrl_t = c.ZETUI_KEY_CTRL_T,
    ctrl_u = c.ZETUI_KEY_CTRL_U,
    ctrl_v = c.ZETUI_KEY_CTRL_V,
    ctrl_w = c.ZETUI_KEY_CTRL_W,
    ctrl_x = c.ZETUI_KEY_CTRL_X,
    ctrl_y = c.ZETUI_KEY_CTRL_Y,
    ctrl_z = c.ZETUI_KEY_CTRL_Z,
    esc = c.ZETUI_KEY_ESC,
    del = c.ZETUI_KEY_DEL,
    arrow_up = c.ZETUI_KEY_ARROW_UP,
    arrow_down = c.ZETUI_KEY_ARROW_DOWN,
    arrow_left = c.ZETUI_KEY_ARROW_LEFT,
    arrow_right = c.ZETUI_KEY_ARROW_RIGHT,
    home = c.ZETUI_KEY_HOME,
    end = c.ZETUI_KEY_END,
    page_up = c.ZETUI_KEY_PAGE_UP,
    page_down = c.ZETUI_KEY_PAGE_DOWN,
    insert = c.ZETUI_KEY_INSERT,
    delete = c.ZETUI_KEY_DELETE,
    f1 = c.ZETUI_KEY_F1,
    f2 = c.ZETUI_KEY_F2,
    f3 = c.ZETUI_KEY_F3,
    f4 = c.ZETUI_KEY_F4,
    f5 = c.ZETUI_KEY_F5,
    f6 = c.ZETUI_KEY_F6,
    f7 = c.ZETUI_KEY_F7,
    f8 = c.ZETUI_KEY_F8,
    f9 = c.ZETUI_KEY_F9,
    f10 = c.ZETUI_KEY_F10,
    f11 = c.ZETUI_KEY_F11,
    f12         = c.ZETUI_KEY_F12,
    shift_tab   = c.ZETUI_KEY_SHIFT_TAB,
    _,
};

/// Convenience constructor for a true-color value (Style.rgb_fg / rgb_bg).
pub fn rgb(r: u8, g: u8, b: u8) Rgb {
    return .{ .r = r, .g = g, .b = b };
}

/// Display width of a codepoint in terminal cells: 2 for wide (CJK,
/// most emoji), 0 for zero-width (combining marks), 1 otherwise.
pub fn charWidth(cp: u21) i32 {
    return c.zetui_char_width(cp);
}

/// Marker codepoint occupying the cell right of a wide character.
pub const wide_pad: u32 = c.ZETUI_WIDE_PAD;

// ------------------------------------------------------------------ //
// Box-drawing tables                                                  //
// ------------------------------------------------------------------ //

/// Indices into a box codepoint table.
pub const Box = struct {
    pub const tl: usize = c.ZETUI_BOX_TL;
    pub const tr: usize = c.ZETUI_BOX_TR;
    pub const bl: usize = c.ZETUI_BOX_BL;
    pub const br: usize = c.ZETUI_BOX_BR;
    pub const h: usize = c.ZETUI_BOX_H;
    pub const v: usize = c.ZETUI_BOX_V;
    pub const lt: usize = c.ZETUI_BOX_LT;
    pub const rt: usize = c.ZETUI_BOX_RT;
    pub const tt: usize = c.ZETUI_BOX_TT;
    pub const bt: usize = c.ZETUI_BOX_BT;
    pub const x: usize = c.ZETUI_BOX_X;
};

pub const BoxVariant = enum(c_uint) {
    light  = c.ZETUI_BOX_STYLE_LIGHT,
    heavy  = c.ZETUI_BOX_STYLE_HEAVY,
    double = c.ZETUI_BOX_STYLE_DOUBLE,
};

/// Return the Unicode codepoint at `idx` from the chosen box table.
pub fn boxCp(variant: BoxVariant, idx: usize) u21 {
    return @intCast(switch (variant) {
        .light => c.zetui_box_light[idx],
        .heavy => c.zetui_box_heavy[idx],
        .double => c.zetui_box_double[idx],
    });
}

// ------------------------------------------------------------------ //
// Style                                                               //
// ------------------------------------------------------------------ //

pub const Rgb = struct {
    r: u8,
    g: u8,
    b: u8,
};

pub const Style = struct {
    fg: Color = .default,
    bg: Color = .default,
    attrs: u32 = Attr.none,
    rgb_fg: ?Rgb = null,
    rgb_bg: ?Rgb = null,
    /// xterm 256-color palette index for the foreground (overrides fg and rgb_fg).
    pal_fg: ?u8 = null,
    /// xterm 256-color palette index for the background (overrides bg and rgb_bg).
    pal_bg: ?u8 = null,

    fn toC(self: Style) c.zetui_style_t {
        const fg_val: i32 = if (self.pal_fg) |n|
            (@as(i32, 1) << 25) | @as(i32, n)
        else if (self.rgb_fg) |v|
            (@as(i32, 1) << 24) | (@as(i32, v.r) << 16) | (@as(i32, v.g) << 8) | @as(i32, v.b)
        else
            self.fg.toC();
        const bg_val: i32 = if (self.pal_bg) |n|
            (@as(i32, 1) << 25) | @as(i32, n)
        else if (self.rgb_bg) |v|
            (@as(i32, 1) << 24) | (@as(i32, v.r) << 16) | (@as(i32, v.g) << 8) | @as(i32, v.b)
        else
            self.bg.toC();
        return .{
            .fg = fg_val,
            .bg = bg_val,
            .attrs = self.attrs,
        };
    }
};

// ------------------------------------------------------------------ //
// Cell                                                                //
// ------------------------------------------------------------------ //

pub const Cell = struct {
    ch: u32 = ' ',
    style: Style = .{},
    link: c_int = 0,

    fn toC(self: Cell) c.zetui_cell_t {
        var cc = c.zetui_cell_make(self.ch, self.style.toC());
        cc.link = self.link;
        return cc;
    }
};

// ------------------------------------------------------------------ //
// Events                                                              //
// ------------------------------------------------------------------ //

pub const KeyEvent = struct {
    key: Key,
    ch: u21,
    mods: u32,
};

pub const ResizeEvent = struct {
    width: i32,
    height: i32,
};

pub const MouseAction = enum(c_int) {
    press = c.ZETUI_MOUSE_PRESS,
    release = c.ZETUI_MOUSE_RELEASE,
    motion = c.ZETUI_MOUSE_MOTION,
    wheel_up = c.ZETUI_MOUSE_WHEEL_UP,
    wheel_down = c.ZETUI_MOUSE_WHEEL_DOWN,
};

pub const MouseButton = enum(c_int) {
    none = c.ZETUI_MOUSE_BUTTON_NONE,
    left = c.ZETUI_MOUSE_BUTTON_LEFT,
    middle = c.ZETUI_MOUSE_BUTTON_MIDDLE,
    right = c.ZETUI_MOUSE_BUTTON_RIGHT,
};

pub const MouseEvent = struct {
    action: MouseAction,
    button: MouseButton,
    x: i32,
    y: i32,
    mods: u32,
};

pub const FocusEvent = struct {
    focused: bool,
};

pub const PasteEvent = struct {
    begin: bool,
};

pub const Event = union(enum) {
    none,
    key: KeyEvent,
    resize: ResizeEvent,
    mouse: MouseEvent,
    focus: FocusEvent,
    paste: PasteEvent,

    fn fromC(ev: c.zetui_event_t) Event {
        switch (ev.type) {
            c.ZETUI_EVENT_KEY => {
                const ke = ev.data.key;
                return .{ .key = .{
                    .key = @enumFromInt(ke.key),
                    .ch = @intCast(ke.ch),
                    .mods = ke.mods,
                } };
            },
            c.ZETUI_EVENT_RESIZE => {
                const re = ev.data.resize;
                return .{ .resize = .{
                    .width = re.width,
                    .height = re.height,
                } };
            },
            c.ZETUI_EVENT_MOUSE => {
                const me = ev.data.mouse;
                return .{ .mouse = .{
                    .action = @enumFromInt(me.action),
                    .button = @enumFromInt(me.button),
                    .x = me.x,
                    .y = me.y,
                    .mods = me.mods,
                } };
            },
            c.ZETUI_EVENT_FOCUS => {
                return .{ .focus = .{ .focused = ev.data.focus.focused != 0 } };
            },
            c.ZETUI_EVENT_PASTE => {
                return .{ .paste = .{ .begin = ev.data.paste.begin != 0 } };
            },
            else => return .none,
        }
    }
};

/// Split a C color value into its palette / 24-bit RGB / 256-color representation.
fn splitColor(v: i32) struct { palette: Color, rgb: ?Rgb, pal: ?u8 } {
    const uv: u32 = @bitCast(v);
    if ((uv >> 25) == 1) {
        return .{ .palette = .default, .rgb = null, .pal = @intCast(uv & 0xFF) };
    }
    if ((uv >> 24) == 1) {
        return .{ .palette = .default, .rgb = .{
            .r = @intCast((v >> 16) & 0xFF),
            .g = @intCast((v >> 8) & 0xFF),
            .b = @intCast(v & 0xFF),
        }, .pal = null };
    }
    return .{ .palette = @enumFromInt(v), .rgb = null, .pal = null };
}

// ------------------------------------------------------------------ //
// Error set                                                           //
// ------------------------------------------------------------------ //

pub const Error = error{
    NoMemory,
    Io,
    NotATty,
    Unsupported,
    Timeout,
};

fn mapError(code: c.zetui_error_t) Error!void {
    switch (code) {
        c.ZETUI_OK => {},
        c.ZETUI_ERR_NOMEM => return error.NoMemory,
        c.ZETUI_ERR_IO => return error.Io,
        c.ZETUI_ERR_NOT_A_TTY => return error.NotATty,
        c.ZETUI_ERR_UNSUPPORTED => return error.Unsupported,
        c.ZETUI_ERR_TIMEOUT => return error.Timeout,
        else => return error.Io,
    }
}

// ------------------------------------------------------------------ //
// Context                                                             //
// ------------------------------------------------------------------ //

pub const Context = struct {
    raw: *c.zetui_ctx_t,

    /// Open the TUI on the controlling terminal.
    pub fn init() Error!Context {
        var code: c.zetui_error_t = c.ZETUI_OK;
        const ptr = c.zetui_init_ex(&code) orelse {
            try mapError(code);
            return error.Io; // NULL with ZETUI_OK cannot happen
        };
        return .{ .raw = ptr };
    }

    /// Restore terminal state and free all resources.
    pub fn deinit(self: *Context) void {
        c.zetui_shutdown(self.raw);
    }

    pub fn width(self: *const Context) i32 {
        return c.zetui_width(self.raw);
    }
    pub fn height(self: *const Context) i32 {
        return c.zetui_height(self.raw);
    }
    pub fn colorSupport(self: *const Context) ColorSupport {
        return @enumFromInt(c.zetui_color_support(self.raw));
    }

    /// Clear the back buffer.
    pub fn clear(self: *Context) void {
        c.zetui_clear(self.raw);
    }

    /// Flush the back buffer to the terminal.
    pub fn present(self: *Context) Error!void {
        return mapError(c.zetui_present(self.raw));
    }

    /// Set one cell in the back buffer.
    pub fn setCell(self: *Context, x: i32, y: i32, cell: Cell) void {
        c.zetui_set_cell(self.raw, x, y, cell.toC());
    }

    /// Get one cell from the back buffer.
    pub fn getCell(self: *const Context, x: i32, y: i32) Cell {
        const raw_cell = c.zetui_get_cell(self.raw, x, y);
        const fg = splitColor(raw_cell.fg);
        const bg = splitColor(raw_cell.bg);
        return .{
            .ch = raw_cell.ch,
            .style = .{
                .fg = fg.palette,
                .bg = bg.palette,
                .attrs = raw_cell.attrs,
                .rgb_fg = fg.rgb,
                .rgb_bg = bg.rgb,
                .pal_fg = fg.pal,
                .pal_bg = bg.pal,
            },
        };
    }

    /// Non-blocking event poll.
    pub fn pollEvent(self: *Context) Event {
        return Event.fromC(c.zetui_poll_event(self.raw));
    }

    /// Blocking event wait. Pass -1 to block forever.
    pub fn waitEvent(self: *Context, timeout_ms: i32) Event {
        return Event.fromC(c.zetui_wait_event(self.raw, timeout_ms));
    }

    /// Enable SGR mouse reporting (delivers Event.mouse). Off by default;
    /// note it takes over the terminal's native text selection.
    pub fn mouseEnable(self: *Context) void {
        c.zetui_mouse_enable(self.raw);
    }

    /// Disable mouse reporting.
    pub fn mouseDisable(self: *Context) void {
        c.zetui_mouse_disable(self.raw);
    }

    /// Enable focus-change reporting (ZETUI_EVENT_FOCUS events).
    pub fn focusEnable(self: *Context) void {
        c.zetui_focus_enable(self.raw);
    }

    /// Disable focus-change reporting.
    pub fn focusDisable(self: *Context) void {
        c.zetui_focus_disable(self.raw);
    }

    /// Enable bracketed paste mode (ZETUI_EVENT_PASTE events).
    pub fn pasteEnable(self: *Context) void {
        c.zetui_paste_enable(self.raw);
    }

    /// Disable bracketed paste mode.
    pub fn pasteDisable(self: *Context) void {
        c.zetui_paste_disable(self.raw);
    }

    /// Register a hyperlink URI; returns a stable ID (1-based, 0 = failure).
    pub fn registerLink(self: *Context, uri: [*:0]const u8) c_int {
        return c.zetui_register_link(self.raw, uri);
    }

    /// Return the hyperlink ID at (x, y) in the front buffer; 0 if none.
    pub fn getLink(self: *const Context, x: i32, y: i32) c_int {
        return c.zetui_get_link_at(self.raw, x, y);
    }

    /// Change the mouse pointer sprite via OSC 22 (e.g. "pointer", "default").
    pub fn setPointerShape(self: *Context, name: [*:0]const u8) void {
        c.zetui_set_pointer_shape(self.raw, name);
    }

    /// Draw a null-terminated string with an OSC 8 hyperlink.
    pub fn drawLink(self: *Context, x: i32, y: i32, str: [*:0]const u8, uri: [*:0]const u8, style: Style) void {
        c.zetui_draw_link(self.raw, x, y, str, uri, style.toC());
    }

    /// Set the terminal window title (OSC 2).
    pub fn setTitle(self: *Context, title: [*:0]const u8) void {
        c.zetui_set_title(self.raw, title);
    }

    /// Set the terminal window title from a Zig slice.
    pub fn setTitleSlice(self: *Context, title: []const u8) void {
        var buf: [256]u8 = undefined;
        const n = @min(title.len, buf.len - 1);
        @memcpy(buf[0..n], title[0..n]);
        buf[n] = 0;
        c.zetui_set_title(self.raw, &buf);
    }

    /// Draw a null-terminated UTF-8 string.
    pub fn drawStr(self: *Context, x: i32, y: i32, str: [*:0]const u8, style: Style) void {
        c.zetui_draw_str(self.raw, x, y, str, style.toC());
    }

    /// Draw a Zig slice (copies to a stack buffer with null terminator).
    pub fn drawSlice(self: *Context, x: i32, y: i32, str: []const u8, style: Style) void {
        var buf: [4096]u8 = undefined;
        const n = @min(str.len, buf.len - 1);
        @memcpy(buf[0..n], str[0..n]);
        buf[n] = 0;
        c.zetui_draw_str(self.raw, x, y, &buf, style.toC());
    }

    /// Draw a box border using the given box-drawing character set.
    pub fn drawBox(self: *Context, x: i32, y: i32, w: i32, h: i32, box: BoxVariant, style: Style) void {
        c.zetui_draw_box(self.raw, x, y, w, h, @intFromEnum(box), style.toC());
    }

    /// Fill a rectangle with a single cell.
    pub fn fillRect(self: *Context, x: i32, y: i32, w: i32, h: i32, cell: Cell) void {
        c.zetui_fill_rect(self.raw, x, y, w, h, cell.toC());
    }

    pub fn drawHline(self: *Context, x: i32, y: i32, len: i32, ch: u21, style: Style) void {
        c.zetui_draw_hline(self.raw, x, y, len, ch, style.toC());
    }

    pub fn drawVline(self: *Context, x: i32, y: i32, len: i32, ch: u21, style: Style) void {
        c.zetui_draw_vline(self.raw, x, y, len, ch, style.toC());
    }

    pub fn cursorHide(self: *Context) void {
        c.zetui_cursor_hide(self.raw);
    }
    pub fn cursorShow(self: *Context) void {
        c.zetui_cursor_show(self.raw);
    }
    pub fn cursorMove(self: *Context, x: i32, y: i32) void {
        c.zetui_cursor_move(self.raw, x, y);
    }
    pub fn cursorSetShape(self: *Context, shape: CursorShape) void {
        c.zetui_cursor_set_shape(self.raw, @intFromEnum(shape));
    }
};
