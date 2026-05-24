/// demo.zig -- zetui comprehensive feature showcase (Zig edition)
///
/// Mirrors demo.c using the idiomatic Zig bindings.
/// Every public symbol from bindings/zetui.zig is exercised here.
///
/// Navigate:  Tab / 1-6   switch panel
/// Quit:      q / Ctrl-C

const std    = @import("std");
const zetui  = @import("zetui");

const SIDEBAR_W: i32   = 16;
const NUM_PANELS: usize = 6;

const panel_titles = [NUM_PANELS][:0]const u8{
    "Colors", "Attributes", "Box Styles", "Input", "UTF-8", "Patterns",
};

// ================================================================== //
// Helper: draw a box with an arbitrary box variant                   //
// (zetui.Context.drawBox always uses the light set)                  //
// ================================================================== //

fn drawBoxTable(
    tui: *zetui.Context,
    x: i32, y: i32, w: i32, h: i32,
    variant: zetui.BoxVariant,
    style: zetui.Style,
) void {
    if (w < 2 or h < 2) return;
    const S = zetui.Box;
    tui.setCell(x,         y,         .{ .ch = zetui.boxCp(variant, S.tl), .style = style });
    tui.setCell(x + w - 1, y,         .{ .ch = zetui.boxCp(variant, S.tr), .style = style });
    tui.setCell(x,         y + h - 1, .{ .ch = zetui.boxCp(variant, S.bl), .style = style });
    tui.setCell(x + w - 1, y + h - 1, .{ .ch = zetui.boxCp(variant, S.br), .style = style });
    var i: i32 = 1;
    while (i < w - 1) : (i += 1) {
        tui.setCell(x + i, y,         .{ .ch = zetui.boxCp(variant, S.h), .style = style });
        tui.setCell(x + i, y + h - 1, .{ .ch = zetui.boxCp(variant, S.h), .style = style });
    }
    i = 1;
    while (i < h - 1) : (i += 1) {
        tui.setCell(x,         y + i, .{ .ch = zetui.boxCp(variant, S.v), .style = style });
        tui.setCell(x + w - 1, y + i, .{ .ch = zetui.boxCp(variant, S.v), .style = style });
    }
}

// ================================================================== //
// Panel 1: Colors                                                     //
// ================================================================== //

const color_names = [16][:0]const u8{
    "black  ", "red    ", "green  ", "yellow ",
    "blue   ", "magenta", "cyan   ", "white  ",
    "Bblack ", "Bred   ", "Bgreen ", "Byellow",
    "Bblue  ", "Bmagnt.", "Bcyan  ", "Bwhite ",
};

fn panelColors(tui: *zetui.Context, x: i32, y: i32, w: i32, h: i32) void {
    _ = w; _ = h;
    const hdr: zetui.Style = .{ .fg = .bright_white, .attrs = zetui.Attr.bold };

    // 16 foreground colours
    tui.drawStr(x, y, "16 foreground colors:", hdr);
    for (0..16) |ci| {
        const col: zetui.Color = @enumFromInt(ci);
        const col_x = x + @as(i32, @intCast(ci % 8)) * 9;
        const col_y = y + 1 + @as(i32, @intCast(ci / 8));
        tui.drawStr(col_x, col_y, color_names[ci], .{ .fg = col });
    }

    // 16 background colours
    tui.drawStr(x, y + 4, "16 background colors:", hdr);
    for (0..16) |ci| {
        const col: zetui.Color = @enumFromInt(ci);
        const col_x = x + @as(i32, @intCast(ci % 8)) * 9;
        const col_y = y + 5 + @as(i32, @intCast(ci / 8));
        tui.drawStr(col_x, col_y, color_names[ci], .{ .bg = col });
    }

    // fg x bg matrix
    tui.drawStr(x, y + 8, "fg x bg matrix (8 basic x 8 basic):", hdr);
    for (0..8) |r| {
        for (0..8) |c| {
            tui.setCell(
                x + @as(i32, @intCast(c)) * 2,
                y + 9 + @as(i32, @intCast(r)),
                .{
                    .ch    = 'A' + @as(u32, @intCast(r)),
                    .style = .{ .fg = @enumFromInt(r), .bg = @enumFromInt(c) },
                },
            );
        }
    }

    // zetui.Context.getCell reads back the buffer -- show it
    tui.drawStr(x, y + 18, "zetui_style_default():", hdr);
    tui.drawStr(x + 23, y + 18, "terminal default fg/bg", .{});
}

// ================================================================== //
// Panel 2: Attributes                                                 //
// ================================================================== //

fn panelAttrs(tui: *zetui.Context, x: i32, y: i32, w: i32, h: i32) void {
    _ = w; _ = h;
    const hdr: zetui.Style = .{ .fg = .bright_white, .attrs = zetui.Attr.bold };

    tui.drawStr(x, y, "Text attributes (terminal support may vary):", hdr);

    const rows = [_]struct { label: [:0]const u8, style: zetui.Style }{
        .{ .label = "  NONE       normal text -- baseline",             .style = .{} },
        .{ .label = "  BOLD       heavier weight",                      .style = .{ .attrs = zetui.Attr.bold } },
        .{ .label = "  DIM        reduced intensity",                   .style = .{ .attrs = zetui.Attr.dim } },
        .{ .label = "  ITALIC     slanted (if supported)",              .style = .{ .attrs = zetui.Attr.italic } },
        .{ .label = "  UNDERLINE  single underline",                    .style = .{ .attrs = zetui.Attr.underline } },
        .{ .label = "  BLINK      slow blink (if supported)",           .style = .{ .attrs = zetui.Attr.blink } },
        .{ .label = "  REVERSE    swap fg/bg",                          .style = .{ .fg = .white, .bg = .black, .attrs = zetui.Attr.reverse } },
        .{ .label = "  HIDDEN     invisible text (select to reveal)",   .style = .{ .fg = .white, .attrs = zetui.Attr.hidden } },
        .{ .label = "  STRIKE     strikethrough",                       .style = .{ .attrs = zetui.Attr.strike } },
    };

    for (rows, 0..) |row, i| {
        tui.drawStr(x, y + 2 + @as(i32, @intCast(i)), row.label, row.style);
    }

    // Combinations
    tui.drawStr(x, y + 12, "Combinations:", hdr);
    tui.drawStr(x, y + 13, "  BOLD | UNDERLINE  (cyan fg)",
        .{ .fg = .cyan,    .attrs = zetui.Attr.bold | zetui.Attr.underline });
    tui.drawStr(x, y + 14, "  BOLD | REVERSE    (red fg, yellow bg)",
        .{ .fg = .red, .bg = .yellow, .attrs = zetui.Attr.bold | zetui.Attr.reverse });
    tui.drawStr(x, y + 15, "  DIM | ITALIC | STRIKE",
        .{ .attrs = zetui.Attr.dim | zetui.Attr.italic | zetui.Attr.strike });
    tui.drawStr(x, y + 16, "  BOLD | BLINK | UNDERLINE  (bright green)",
        .{ .fg = .bright_green, .attrs = zetui.Attr.bold | zetui.Attr.blink | zetui.Attr.underline });
}

// ================================================================== //
// Panel 3: Box styles + drawing primitives                           //
// ================================================================== //

fn panelBoxes(tui: *zetui.Context, x: i32, y: i32, w: i32, h: i32) void {
    _ = h;
    const hdr:   zetui.Style = .{ .fg = .bright_white, .attrs = zetui.Attr.bold };
    const plain: zetui.Style = .{};
    const accent: zetui.Style = .{ .fg = .cyan, .attrs = zetui.Attr.bold };

    tui.drawStr(x, y, "zetui_box_light / heavy / double  via drawBoxTable():", hdr);

    const bw = @divTrunc(w - 2, 3) - 1;
    const bh: i32 = 7;

    // Light -- uses tui.drawBox (the built-in light wrapper)
    tui.drawBox(x, y + 1, bw, bh, plain);
    tui.drawStr(x + 2, y + 2, "light", accent);
    tui.setCell(x,          y + 4, .{ .ch = zetui.boxCp(.light, zetui.Box.lt), .style = plain });
    tui.drawHline(x + 1, y + 4, bw - 2, zetui.boxCp(.light, zetui.Box.h), plain);
    tui.setCell(x + bw - 1, y + 4, .{ .ch = zetui.boxCp(.light, zetui.Box.rt), .style = plain });

    // Heavy
    const bx_heavy = x + bw + 1;
    drawBoxTable(tui, bx_heavy, y + 1, bw, bh, .heavy, plain);
    tui.drawStr(bx_heavy + 2, y + 2, "heavy", accent);
    tui.setCell(bx_heavy,          y + 4, .{ .ch = zetui.boxCp(.heavy, zetui.Box.lt), .style = plain });
    tui.drawHline(bx_heavy + 1, y + 4, bw - 2, zetui.boxCp(.heavy, zetui.Box.h), plain);
    tui.setCell(bx_heavy + bw - 1, y + 4, .{ .ch = zetui.boxCp(.heavy, zetui.Box.rt), .style = plain });

    // Double
    const bx_double = x + (bw + 1) * 2;
    drawBoxTable(tui, bx_double, y + 1, bw, bh, .double, plain);
    tui.drawStr(bx_double + 2, y + 2, "double", accent);
    tui.setCell(bx_double,          y + 4, .{ .ch = zetui.boxCp(.double, zetui.Box.lt), .style = plain });
    tui.drawHline(bx_double + 1, y + 4, bw - 2, zetui.boxCp(.double, zetui.Box.h), plain);
    tui.setCell(bx_double + bw - 1, y + 4, .{ .ch = zetui.boxCp(.double, zetui.Box.rt), .style = plain });

    // hlines
    tui.drawStr(x, y + 9, "drawHline (light / heavy / double):", hdr);
    tui.drawHline(x, y + 10, w - 1, zetui.boxCp(.light,  zetui.Box.h), plain);
    tui.drawHline(x, y + 11, w - 1, zetui.boxCp(.heavy,  zetui.Box.h), plain);
    tui.drawHline(x, y + 12, w - 1, zetui.boxCp(.double, zetui.Box.h), plain);

    // vlines
    tui.drawStr(x, y + 14, "drawVline:", hdr);
    tui.drawVline(x + 11, y + 14, 5, zetui.boxCp(.light,  zetui.Box.v), plain);
    tui.drawVline(x + 13, y + 14, 5, zetui.boxCp(.heavy,  zetui.Box.v), plain);
    tui.drawVline(x + 15, y + 14, 5, zetui.boxCp(.double, zetui.Box.v), plain);

    // fillRect
    tui.drawStr(x, y + 20, "fillRect:", hdr);
    tui.fillRect(x + 10, y + 20, 18, 3, .{ .style = .{ .fg = .black, .bg = .green } });
    tui.drawStr(x + 11, y + 21, "filled region",
        .{ .fg = .bright_white, .bg = .green, .attrs = zetui.Attr.bold });
}

// ================================================================== //
// Panel 4: Input events                                              //
// ================================================================== //

fn keyName(k: zetui.Key) []const u8 {
    return switch (k) {
        .none        => "NONE",
        .ctrl_a      => "Ctrl-A",    .ctrl_b => "Ctrl-B",
        .ctrl_c      => "Ctrl-C",    .ctrl_d => "Ctrl-D",
        .ctrl_e      => "Ctrl-E",    .ctrl_f => "Ctrl-F",
        .ctrl_g      => "Ctrl-G",
        .backspace   => "Backspace",
        .tab         => "Tab",
        .enter       => "Enter",
        .ctrl_k      => "Ctrl-K",    .ctrl_l => "Ctrl-L",
        .ctrl_n      => "Ctrl-N",    .ctrl_p => "Ctrl-P",
        .ctrl_q      => "Ctrl-Q",    .ctrl_r => "Ctrl-R",
        .ctrl_s      => "Ctrl-S",    .ctrl_u => "Ctrl-U",
        .ctrl_w      => "Ctrl-W",    .ctrl_x => "Ctrl-X",
        .ctrl_y      => "Ctrl-Y",    .ctrl_z => "Ctrl-Z",
        .esc         => "Escape",
        .del         => "Del(127)",
        .arrow_up    => "Arrow Up",  .arrow_down  => "Arrow Down",
        .arrow_left  => "Arrow Left",.arrow_right => "Arrow Right",
        .home        => "Home",      .end         => "End",
        .page_up     => "Page Up",   .page_down   => "Page Down",
        .insert      => "Insert",    .delete      => "Delete",
        .f1  => "F1",  .f2  => "F2",  .f3  => "F3",  .f4  => "F4",
        .f5  => "F5",  .f6  => "F6",  .f7  => "F7",  .f8  => "F8",
        .f9  => "F9",  .f10 => "F10", .f11 => "F11", .f12 => "F12",
        _ => "unknown",
    };
}

fn panelInput(
    tui: *zetui.Context,
    x: i32, y: i32, w: i32, h: i32,
    last: zetui.Event,
    total: i32,
) void {
    _ = w;
    const hdr: zetui.Style = .{ .fg = .bright_white, .attrs = zetui.Attr.bold };
    const lbl: zetui.Style = .{ .fg = .yellow };
    const val: zetui.Style = .{ .fg = .cyan, .attrs = zetui.Attr.bold };
    const dim: zetui.Style = .{ .fg = .bright_black };

    tui.drawStr(x, y,     "Input event inspector", hdr);
    tui.drawStr(x, y + 1, "(Tab and 1-6 switch panels -- all other keys reported here)", dim);

    var buf: [128]u8 = undefined;
    var row = y + 3;

    switch (last) {
        .none => tui.drawStr(x, row, "No event yet -- press any key.", dim),

        .key => |ke| {
            tui.drawStr(x,      row, "event type  :", lbl);
            tui.drawStr(x + 14, row, "KEY",           val);
            row += 1;

            // Key name
            tui.drawStr(x, row, "key name    :", lbl);
            if (ke.ch >= 32 and ke.ch != 127 and ke.key == .none) {
                const s = std.fmt.bufPrint(&buf, "{u}", .{ke.ch}) catch "?";
                tui.drawSlice(x + 14, row, s, val);
            } else {
                tui.drawSlice(x + 14, row, keyName(ke.key), val);
            }
            row += 1;

            // Key code
            const code_s = std.fmt.bufPrint(&buf, "{d}", .{@intFromEnum(ke.key)}) catch "?";
            tui.drawStr(x,      row, "key code    :", lbl);
            tui.drawSlice(x + 14, row, code_s, val);
            row += 1;

            // Codepoint
            const cp_s = std.fmt.bufPrint(&buf, "U+{X}", .{ke.ch}) catch "?";
            tui.drawStr(x,      row, "codepoint   :", lbl);
            tui.drawSlice(x + 14, row, cp_s, val);
            row += 1;

            // Modifiers
            const mod_str: []const u8 =
                if (ke.mods & zetui.Mod.ctrl  != 0) "CTRL"
                else if (ke.mods & zetui.Mod.alt   != 0) "ALT"
                else if (ke.mods & zetui.Mod.shift != 0) "SHIFT"
                else "none";
            tui.drawStr(x,      row, "modifiers   :", lbl);
            tui.drawSlice(x + 14, row, mod_str, val);
            row += 2;

            // Glyph preview (fillRect + drawStr + cursorMove)
            if (ke.ch >= 32 and ke.ch != 127) {
                const glyph_s = std.fmt.bufPrint(&buf, "{u}", .{ke.ch}) catch "?";
                tui.drawStr(x, row, "glyph:", lbl);
                tui.fillRect(x + 8, row - 1, 5, 3,
                    .{ .style = .{ .bg = .blue } });
                tui.drawSlice(x + 10, row, glyph_s,
                    .{ .fg = .bright_white, .bg = .blue, .attrs = zetui.Attr.bold });
                // Park cursor on the glyph -- demonstrates cursorMove
                tui.cursorMove(x + 10, row);
            }
        },

        .resize => |re| {
            tui.drawStr(x,      row, "event type  :", lbl);
            tui.drawStr(x + 14, row, "RESIZE",        val);
            row += 1;
            const sz_s = std.fmt.bufPrint(&buf, "{d}x{d}", .{ re.width, re.height }) catch "?";
            tui.drawStr(x,      row, "new size    :", lbl);
            tui.drawSlice(x + 14, row, sz_s, val);
        },
    }

    // Total event counter
    const total_s = std.fmt.bufPrint(&buf, "total events: {d}", .{total}) catch "?";
    tui.drawSlice(x, y + h - 2, total_s, dim);
}

// ================================================================== //
// Panel 5: UTF-8 via setCell                                         //
// ================================================================== //

fn panelUtf8(tui: *zetui.Context, x: i32, y: i32, w: i32, h: i32) void {
    _ = w; _ = h;
    const hdr: zetui.Style = .{ .fg = .bright_white, .attrs = zetui.Attr.bold };

    const blocks = [_]u32{
        0x2588, 0x2587, 0x2586, 0x2585, 0x2584, 0x2583, 0x2582, 0x2581,
        0x258F, 0x258E, 0x258D, 0x258C, 0x258B, 0x258A, 0x2589, 0x25A0,
    };
    const misc = [_]u32{
        0x2665, 0x2666, 0x2663, 0x2660,
        0x263A, 0x263B, 0x2605, 0x2606,
        0x2764, 0x2713, 0x2717, 0x2714,
        0x2190, 0x2192, 0x2191, 0x2193,
        0x21D0, 0x21D2, 0x21D1, 0x21D3,
        0x00B7, 0x2022, 0x25CF, 0x25CB,
    };
    const braille = [_]u32{
        0x2800, 0x2801, 0x2802, 0x2803, 0x2804, 0x2805, 0x2806, 0x2807,
        0x2840, 0x2841, 0x2842, 0x2843, 0x2844, 0x2845, 0x2846, 0x2847,
    };

    var row = y;

    // All box codepoints from all three tables -- exercises boxCp + Box.*
    tui.drawStr(x, row, "All box-drawing codepoints (light / heavy / double):", hdr);
    row += 1;
    const variants = [_]zetui.BoxVariant{ .light, .heavy, .double };
    const variant_colors = [_]zetui.Color{ .cyan, .yellow, .green };
    for (variants, variant_colors) |variant, fg| {
        for (0..11) |i| {
            tui.setCell(x + @as(i32, @intCast(i)) * 2, row, .{
                .ch    = zetui.boxCp(variant, i),
                .style = .{ .fg = fg },
            });
        }
        row += 1;
    }

    // Block elements
    row += 1;
    tui.drawStr(x, row, "Block elements:", hdr);
    row += 1;
    for (blocks, 0..) |cp, i| {
        tui.setCell(x + @as(i32, @intCast(i)) * 2, row,
            .{ .ch = cp, .style = .{ .fg = .bright_cyan } });
    }

    // Misc symbols
    row += 2;
    tui.drawStr(x, row, "Misc symbols:", hdr);
    row += 1;
    for (misc, 0..) |cp, i| {
        tui.setCell(x + @as(i32, @intCast(i)) * 3, row,
            .{ .ch = cp, .style = .{ .fg = .bright_magenta } });
    }

    // Braille
    row += 2;
    tui.drawStr(x, row, "Braille patterns (U+2800-284F sample):", hdr);
    row += 1;
    for (braille, 0..) |cp, i| {
        tui.setCell(x + @as(i32, @intCast(i)) * 2, row,
            .{ .ch = cp, .style = .{ .fg = .bright_white } });
    }

    // drawStr with literal multi-byte UTF-8 bytes
    row += 2;
    tui.drawStr(x, row, "drawStr with inline UTF-8:", hdr);
    row += 1;
    tui.drawStr(x, row,
        "\xe2\x94\x8c\xe2\x94\x80\xe2\x94\x90 " ++
        "\xe2\x96\x88\xe2\x96\x84\xe2\x96\x80 " ++
        "\xe2\x99\xa5 \xe2\x98\x85 \xe2\x9c\x93",
        .{ .fg = .bright_yellow });
}

// ================================================================== //
// Panel 6: Drawing patterns (getCell, fill, primitives)              //
// ================================================================== //

fn panelPatterns(tui: *zetui.Context, x: i32, y: i32, w: i32, h: i32) void {
    _ = h;
    const hdr: zetui.Style = .{ .fg = .bright_white, .attrs = zetui.Attr.bold };

    var row = y;

    // fillRect colour gradient
    tui.drawStr(x, row, "fillRect gradient (8 solid colour bands):", hdr);
    row += 1;
    for (0..8) |ci| {
        const col: zetui.Color = @enumFromInt(ci);
        tui.fillRect(x + @as(i32, @intCast(ci)) * 5, row, 5, 2,
            .{ .style = .{ .fg = col, .bg = col } });
    }
    row += 3;

    // hlines from all three box sets
    tui.drawStr(x, row, "drawHline (light / heavy / double):", hdr);
    row += 1;
    const hl_colors = [_]zetui.Color{ .bright_cyan, .bright_yellow, .bright_green };
    for (0..3) |i| {
        const variant: zetui.BoxVariant = @enumFromInt(i);
        tui.drawHline(x, row + @as(i32, @intCast(i)), w - 1,
            zetui.boxCp(variant, zetui.Box.h),
            .{ .fg = hl_colors[i] });
    }
    row += 4;

    // getCell -> swap fg/bg -> setCell
    tui.drawStr(x, row, "getCell \u{2192} swap fg/bg \u{2192} setCell:", hdr);
    row += 1;
    const lbl: zetui.Style = .{ .fg = .yellow, .attrs = zetui.Attr.dim };
    tui.drawStr(x, row, "label:", lbl);
    tui.drawStr(x + 7, row, "original (green on blue)",
        .{ .fg = .green, .bg = .blue });
    row += 1;
    tui.drawStr(x, row, "label:", lbl);
    // Read original row back from the back buffer and invert
    var col_i: i32 = x + 7;
    while (col_i < x + 7 + 24) : (col_i += 1) {
        var cell = tui.getCell(col_i, row - 1);
        const tmp = cell.style.fg;
        cell.style.fg = cell.style.bg;
        cell.style.bg = tmp;
        tui.setCell(col_i, row, cell);
    }
    row += 2;

    // Cross-hatch via setCell
    tui.drawStr(x, row, "setCell cross-hatch pattern:", hdr);
    row += 1;
    for (0..6) |ri| {
        for (0..32) |ci| {
            const ch: u32 = if ((ri + ci) % 2 == 0) 0x2588 else 0x2591;
            const fg: zetui.Color = @enumFromInt((ri + ci) % 8);
            tui.setCell(x + @as(i32, @intCast(ci)), row + @as(i32, @intCast(ri)),
                .{ .ch = ch, .style = .{ .fg = fg } });
        }
    }
}

// ================================================================== //
// Chrome: outer frame, title, sidebar, status bar                   //
// ================================================================== //

fn drawChrome(tui: *zetui.Context, sel: usize, total: i32) void {
    const w = tui.width();
    const h = tui.height();

    const frame:      zetui.Style = .{ .fg = .bright_black };
    const title_bg:   zetui.Style = .{ .bg = .blue };
    const title_txt:  zetui.Style = .{ .fg = .bright_white, .bg = .blue, .attrs = zetui.Attr.bold };
    const tab_sel:    zetui.Style = .{ .fg = .bright_white, .bg = .blue, .attrs = zetui.Attr.bold };
    const tab_norm:   zetui.Style = .{ .fg = .white };
    const dim:        zetui.Style = .{ .fg = .bright_black };

    // Outer heavy box
    drawBoxTable(tui, 0, 0, w, h, .heavy, frame);

    // Title bar
    tui.fillRect(1, 1, w - 2, 1, .{ .style = title_bg });
    tui.drawStr(2, 1, "zetui feature demo", title_txt);
    {
        var buf: [32]u8 = undefined;
        const s = std.fmt.bufPrint(&buf, "size:{d}x{d}", .{ w, h }) catch return;
        tui.drawSlice(w - @as(i32, @intCast(s.len)) - 2, 1, s, title_txt);
    }

    // Row 2: ┣━━┳━━┫
    const S = zetui.Box;
    tui.setCell(0,         2, .{ .ch = zetui.boxCp(.heavy, S.lt), .style = frame });
    tui.drawHline(1, 2, SIDEBAR_W - 1, zetui.boxCp(.heavy, S.h), frame);
    tui.setCell(SIDEBAR_W, 2, .{ .ch = zetui.boxCp(.heavy, S.tt), .style = frame });
    tui.drawHline(SIDEBAR_W + 1, 2, w - SIDEBAR_W - 2, zetui.boxCp(.heavy, S.h), frame);
    tui.setCell(w - 1,     2, .{ .ch = zetui.boxCp(.heavy, S.rt), .style = frame });

    // Vertical divider
    tui.drawVline(SIDEBAR_W, 3, h - 5, zetui.boxCp(.heavy, S.v), frame);

    // Row h-2: ┣━━┻━━┫
    tui.setCell(0,         h - 2, .{ .ch = zetui.boxCp(.heavy, S.lt), .style = frame });
    tui.drawHline(1, h - 2, SIDEBAR_W - 1, zetui.boxCp(.heavy, S.h), frame);
    tui.setCell(SIDEBAR_W, h - 2, .{ .ch = zetui.boxCp(.heavy, S.bt), .style = frame });
    tui.drawHline(SIDEBAR_W + 1, h - 2, w - SIDEBAR_W - 2, zetui.boxCp(.heavy, S.h), frame);
    tui.setCell(w - 1,     h - 2, .{ .ch = zetui.boxCp(.heavy, S.rt), .style = frame });

    // Sidebar
    for (0..NUM_PANELS) |i| {
        const ms = if (i == sel) tab_sel else tab_norm;
        var buf: [20]u8 = undefined;
        const entry = std.fmt.bufPrint(&buf, " {d} {s:<12}", .{ i + 1, panel_titles[i] })
            catch continue;
        if (i == sel)
            tui.fillRect(1, 3 + @as(i32, @intCast(i)) * 2, SIDEBAR_W - 1, 1,
                .{ .style = ms });
        tui.drawSlice(1, 3 + @as(i32, @intCast(i)) * 2, entry, ms);
    }

    // Status bar
    tui.drawStr(1, h - 1, " q:quit  Tab/1-6:switch panel", dim);
    {
        var buf: [32]u8 = undefined;
        const s = std.fmt.bufPrint(&buf, "events:{d}", .{total}) catch return;
        tui.drawSlice(w - @as(i32, @intCast(s.len)) - 2, h - 1, s, dim);
    }
}

// ================================================================== //
// Main loop                                                          //
// ================================================================== //

pub fn main() !void {
    var tui = try zetui.Context.init();
    defer tui.deinit();

    tui.cursorHide();

    var sel:   usize = 0;
    var total: i32   = 0;
    var last   = zetui.Event.none;
    var running = true;

    while (running) {
        const w  = tui.width();
        const h  = tui.height();
        const cx = SIDEBAR_W + 1;
        const cy: i32 = 3;
        const cw = w - SIDEBAR_W - 2;
        const ch = h - 5;

        tui.clear();
        drawChrome(&tui, sel, total);

        switch (sel) {
            0 => panelColors(&tui,   cx, cy, cw, ch),
            1 => panelAttrs(&tui,    cx, cy, cw, ch),
            2 => panelBoxes(&tui,    cx, cy, cw, ch),
            3 => panelInput(&tui,    cx, cy, cw, ch, last, total),
            4 => panelUtf8(&tui,     cx, cy, cw, ch),
            5 => panelPatterns(&tui, cx, cy, cw, ch),
            else => {},
        }

        // Show cursor only on Input panel when a printable glyph is displayed
        const show_cursor = sel == 3 and switch (last) {
            .key => |ke| ke.ch >= 32 and ke.ch != 127,
            else => false,
        };
        if (show_cursor) tui.cursorShow() else tui.cursorHide();

        try tui.present();

        // Block for next event (zetui.Context.waitEvent)
        const ev = tui.waitEvent(-1);
        total += 1;
        last = ev;

        switch (ev) {
            .key => |ke| {
                if (ke.key == .ctrl_c or ke.ch == 'q') running = false;
                if (ke.key == .tab)
                    sel = (sel + 1) % NUM_PANELS;
                if (ke.ch >= '1' and ke.ch <= '6')
                    sel = @intCast(ke.ch - '1');
            },
            else => {},
        }

        // Drain any extra events -- demonstrates pollEvent (non-blocking)
        switch (tui.pollEvent()) {
            .none => {},
            else => |extra| { total += 1; last = extra; },
        }
    }

    tui.cursorShow();
}
