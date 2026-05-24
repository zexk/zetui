const std = @import("std");

pub fn build(b: *std.Build) void {
    const target   = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const c_flags = &[_][]const u8{
        "-std=c89",
        "-pedantic",
        "-Wall",
        "-Wextra",
        "-Wno-unused-parameter",
        "-D_POSIX_C_SOURCE=200809L",
    };

    // ----------------------------------------------------------------
    // Static library
    // ----------------------------------------------------------------

    const lib = b.addStaticLibrary(.{
        .name     = "zetui",
        .target   = target,
        .optimize = optimize,
    });

    lib.addCSourceFiles(.{
        .files = &.{"src/zetui.c"},
        .flags = c_flags,
    });

    lib.addIncludePath(b.path("include"));
    lib.linkLibC();
    b.installArtifact(lib);

    // Install public headers alongside the library
    lib.installHeadersDirectory(b.path("include"), ".", .{});

    // ----------------------------------------------------------------
    // Zig module (wraps the C lib with idiomatic Zig API)
    // ----------------------------------------------------------------

    const zetui_mod = b.addModule("zetui", .{
        .root_source_file = b.path("bindings/zetui.zig"),
    });
    zetui_mod.addIncludePath(b.path("include"));
    zetui_mod.linkLibrary(lib);

    // ----------------------------------------------------------------
    // C example
    // ----------------------------------------------------------------

    const hello_c = b.addExecutable(.{
        .name   = "hello-c",
        .target = target,
        .optimize = optimize,
    });
    hello_c.addCSourceFile(.{
        .file  = b.path("examples/hello.c"),
        .flags = c_flags,
    });
    hello_c.addIncludePath(b.path("include"));
    hello_c.linkLibrary(lib);
    hello_c.linkLibC();
    b.installArtifact(hello_c);

    const run_hello_c      = b.addRunArtifact(hello_c);
    const run_hello_c_step = b.step("run-hello-c", "Run the C example");
    run_hello_c_step.dependOn(&run_hello_c.step);

    // ----------------------------------------------------------------
    // Zig example
    // ----------------------------------------------------------------

    const hello_zig = b.addExecutable(.{
        .name             = "hello-zig",
        .root_source_file = b.path("examples/hello.zig"),
        .target           = target,
        .optimize         = optimize,
    });
    hello_zig.root_module.addImport("zetui", zetui_mod);
    b.installArtifact(hello_zig);

    const run_hello_zig      = b.addRunArtifact(hello_zig);
    const run_hello_zig_step = b.step("run-hello-zig", "Run the Zig example");
    run_hello_zig_step.dependOn(&run_hello_zig.step);

    // ----------------------------------------------------------------
    // Feature demo
    // ----------------------------------------------------------------

    const demo = b.addExecutable(.{
        .name     = "demo",
        .target   = target,
        .optimize = optimize,
    });
    demo.addCSourceFile(.{
        .file  = b.path("examples/demo.c"),
        .flags = c_flags,
    });
    demo.addIncludePath(b.path("include"));
    demo.linkLibrary(lib);
    demo.linkLibC();
    b.installArtifact(demo);

    const run_demo      = b.addRunArtifact(demo);
    const run_demo_step = b.step("run-demo", "Run the full feature demo");
    run_demo_step.dependOn(&run_demo.step);

    // ----------------------------------------------------------------
    // Zig feature demo
    // ----------------------------------------------------------------

    const demo_zig = b.addExecutable(.{
        .name             = "demo-zig",
        .root_source_file = b.path("examples/demo.zig"),
        .target           = target,
        .optimize         = optimize,
    });
    demo_zig.root_module.addImport("zetui", zetui_mod);
    demo_zig.addIncludePath(b.path("include")); // needed for @cImport inside bindings
    b.installArtifact(demo_zig);

    const run_demo_zig      = b.addRunArtifact(demo_zig);
    const run_demo_zig_step = b.step("run-demo-zig", "Run the Zig feature demo");
    run_demo_zig_step.dependOn(&run_demo_zig.step);
}
