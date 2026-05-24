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
    enter = c.ZETUI_KEY_ENTER,
    ctrl_k = c.ZETUI_KEY_CTRL_K,
    ctrl_l = c.ZETUI_KEY_CTRL_L,
    ctrl_n = c.ZETUI_KEY_CTRL_N,
    ctrl_p = c.ZETUI_KEY_CTRL_P,
    ctrl_q = c.ZETUI_KEY_CTRL_Q,
    ctrl_r = c.ZETUI_KEY_CTRL_R,
    ctrl_s = c.ZETUI_KEY_CTRL_S,
    ctrl_u = c.ZETUI_KEY_CTRL_U,
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
    f12 = c.ZETUI_KEY_F12,
    _,
};

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

pub const BoxVariant = enum { light, heavy, double };

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

pub const Style = struct {
    fg: Color = .default,
    bg: Color = .default,
    attrs: u32 = Attr.none,

    fn toC(self: Style) c.zetui_style_t {
        return .{
            .fg = self.fg.toC(),
            .bg = self.bg.toC(),
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

    fn toC(self: Cell) c.zetui_cell_t {
        return c.zetui_cell_make(self.ch, self.style.toC());
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

pub const Event = union(enum) {
    none,
    key: KeyEvent,
    resize: ResizeEvent,

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
            else => return .none,
        }
    }
};

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
        const ptr = c.zetui_init() orelse return error.NotATty;
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
        return .{
            .ch = raw_cell.ch,
            .style = .{
                .fg = @enumFromInt(raw_cell.fg),
                .bg = @enumFromInt(raw_cell.bg),
                .attrs = raw_cell.attrs,
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

    /// Draw a box border using light box-drawing characters.
    pub fn drawBox(self: *Context, x: i32, y: i32, w: i32, h: i32, style: Style) void {
        c.zetui_draw_box(self.raw, x, y, w, h, style.toC());
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
};
