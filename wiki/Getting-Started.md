# Getting Started

## Installation

### Single-header (recommended)

Copy `include/zetui.h` into your project. In **exactly one** translation unit define the implementation before including:

```c
#define ZETUI_IMPLEMENTATION
#include "zetui.h"
```

Every other translation unit includes without the define:

```c
#include "zetui.h"
```

No build system required. Compile with any C89-compatible compiler:

```sh
cc -o myapp myapp.c
```

### Static library via Zig

```sh
zig build
# Produces: zig-out/lib/libzetui.a  zig-out/include/zetui.h
```

Link against `libzetui.a` and add `zig-out/include` to your include path.

## Minimal C example

```c
#define ZETUI_IMPLEMENTATION
#include "zetui.h"

int main(void) {
    zetui_ctx_t *ctx = zetui_init();
    if (!ctx) return 1;

    zetui_cursor_hide(ctx);
    int running = 1;

    while (running) {
        zetui_clear(ctx);
        zetui_draw_str(ctx, 1, 1, "Hello, zetui! (press q to quit)",
                       zetui_style_default());
        zetui_present(ctx);

        zetui_event_t ev = zetui_wait_event(ctx, -1);
        if (ev.type == ZETUI_EVENT_KEY && ev.data.key.ch == 'q')
            running = 0;
    }

    zetui_shutdown(ctx);
    return 0;
}
```

See [`examples/hello.c`](../blob/master/examples/hello.c) and [`examples/demo.c`](../blob/master/examples/demo.c) for full examples.

## Minimal Zig example

Add the module in your `build.zig`:

```zig
// In your build.zig:
const zetui_dep = b.dependency("zetui", .{ .target = target, .optimize = optimize });
exe.root_module.addImport("zetui", zetui_dep.module("zetui"));
```

Then use it:

```zig
const zetui = @import("zetui");

pub fn main() !void {
    var tui = try zetui.Context.init();
    defer tui.deinit();

    tui.cursorHide();
    var running = true;

    while (running) {
        tui.clear();
        tui.drawStr(1, 1, "Hello, zetui! (press q to quit)", .{});
        try tui.present();

        switch (tui.waitEvent(-1)) {
            .key => |ke| if (ke.ch == 'q') running = false,
            else => {},
        }
    }
}
```

See [`examples/hello.zig`](../blob/master/examples/hello.zig) and [`examples/demo.zig`](../blob/master/examples/demo.zig) for full examples.

## Build targets

| Target | Description |
|---|---|
| `zig build` | Build static library + all examples |
| `zig build run-hello-c` | Run the minimal C example |
| `zig build run-hello-zig` | Run the minimal Zig example |
| `zig build run-demo` | Run the C feature demo |
| `zig build run-demo-zig` | Run the Zig feature demo |

## Dev environment (NixOS / Nix)

```sh
nix develop   # or: direnv allow  (requires .envrc)
```

Provides: `zig`, `zls`, `gcc`, `gdb`, `valgrind`, `bear`, `clang-tools`.
