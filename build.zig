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

    const lib_mod = b.createModule(.{
        .target   = target,
        .optimize = optimize,
        .link_libc = true,
    });
    lib_mod.addCSourceFiles(.{
        .files = &.{"src/zetui.c"},
        .flags = c_flags,
    });
    lib_mod.addIncludePath(b.path("include"));

    const lib = b.addLibrary(.{
        .name        = "zetui",
        .root_module = lib_mod,
    });
    lib.installHeadersDirectory(b.path("include"), ".", .{});
    b.installArtifact(lib);

    // ----------------------------------------------------------------
    // Zig module (wraps the C lib with idiomatic Zig API)
    // ----------------------------------------------------------------

    const zetui_mod = b.addModule("zetui", .{
        .root_source_file = b.path("bindings/zetui.zig"),
        .target           = target,
        .optimize         = optimize,
    });
    zetui_mod.addIncludePath(b.path("include"));
    zetui_mod.linkLibrary(lib);

    // ----------------------------------------------------------------
    // C example
    // ----------------------------------------------------------------

    const hello_c_mod = b.createModule(.{
        .target    = target,
        .optimize  = optimize,
        .link_libc = true,
    });
    hello_c_mod.addCSourceFiles(.{
        .files = &.{"examples/hello.c"},
        .flags = c_flags,
    });
    hello_c_mod.addIncludePath(b.path("include"));
    hello_c_mod.linkLibrary(lib);

    const hello_c = b.addExecutable(.{
        .name        = "hello-c",
        .root_module = hello_c_mod,
    });
    b.installArtifact(hello_c);

    const run_hello_c      = b.addRunArtifact(hello_c);
    const run_hello_c_step = b.step("run-hello-c", "Run the C example");
    run_hello_c_step.dependOn(&run_hello_c.step);

    // ----------------------------------------------------------------
    // Zig example
    // ----------------------------------------------------------------

    const hello_zig_mod = b.createModule(.{
        .root_source_file = b.path("examples/hello.zig"),
        .target           = target,
        .optimize         = optimize,
    });
    hello_zig_mod.addImport("zetui", zetui_mod);

    const hello_zig = b.addExecutable(.{
        .name        = "hello-zig",
        .root_module = hello_zig_mod,
    });
    b.installArtifact(hello_zig);

    const run_hello_zig      = b.addRunArtifact(hello_zig);
    const run_hello_zig_step = b.step("run-hello-zig", "Run the Zig example");
    run_hello_zig_step.dependOn(&run_hello_zig.step);

    // ----------------------------------------------------------------
    // C feature demo
    // ----------------------------------------------------------------

    const demo_mod = b.createModule(.{
        .target    = target,
        .optimize  = optimize,
        .link_libc = true,
    });
    demo_mod.addCSourceFiles(.{
        .files = &.{"examples/demo.c"},
        .flags = c_flags,
    });
    demo_mod.addIncludePath(b.path("include"));
    demo_mod.linkLibrary(lib);

    const demo = b.addExecutable(.{
        .name        = "demo",
        .root_module = demo_mod,
    });
    b.installArtifact(demo);

    const run_demo      = b.addRunArtifact(demo);
    const run_demo_step = b.step("run-demo", "Run the full feature demo");
    run_demo_step.dependOn(&run_demo.step);

    // ----------------------------------------------------------------
    // Zig feature demo
    // ----------------------------------------------------------------

    const demo_zig_mod = b.createModule(.{
        .root_source_file = b.path("examples/demo.zig"),
        .target           = target,
        .optimize         = optimize,
    });
    demo_zig_mod.addImport("zetui", zetui_mod);

    const demo_zig = b.addExecutable(.{
        .name        = "demo-zig",
        .root_module = demo_zig_mod,
    });
    b.installArtifact(demo_zig);

    const run_demo_zig      = b.addRunArtifact(demo_zig);
    const run_demo_zig_step = b.step("run-demo-zig", "Run the Zig feature demo");
    run_demo_zig_step.dependOn(&run_demo_zig.step);
}
