/// hello.zig — minimal Zig example
/// Build: zig build run-hello-zig
const std = @import("std");
const zetui = @import("zetui");

pub fn main() !void {
    var tui = try zetui.Context.init();
    defer tui.deinit();

    tui.cursorHide();

    var running = true;
    while (running) {
        const w = tui.width();
        const h = tui.height();

        tui.clear();

        // Outer border
        tui.drawBox(0, 0, w, h, .{});

        // Title bar
        tui.fillRect(1, 1, w - 2, 1, .{ .style = .{ .bg = .blue } });
        tui.drawStr(2, 1, " zetui demo ", .{ .fg = .cyan, .attrs = zetui.Attr.bold });

        // Content
        tui.drawStr(2, 3, "Hello from zetui (Zig)!", .{ .fg = .cyan, .attrs = zetui.Attr.bold });
        tui.drawStr(2, 5, "Press q or Ctrl-C to quit.", .{});

        // Size info
        var buf: [64]u8 = undefined;
        const info = try std.fmt.bufPrintZ(&buf, "Size: {d}x{d}", .{ w, h });
        tui.drawStr(2, 7, info, .{ .fg = .white, .attrs = zetui.Attr.dim });

        try tui.present();

        const ev = tui.waitEvent(-1);
        switch (ev) {
            .key => |ke| {
                if (ke.key == .ctrl_c or ke.ch == 'q') running = false;
            },
            else => {},
        }
    }

    tui.cursorShow();
}
