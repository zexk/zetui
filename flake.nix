{
  description = "zetui — ANSI/C89 TUI library with first-class Zig bindings";

  inputs = {
    nixpkgs.url     = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            zig
            zls
            gcc
            gdb
            valgrind
            bear
            clang-tools
            doxygen
          ];

          shellHook = ''
            echo ""
            echo "  zetui dev environment"
            echo "  zig build               build library + examples"
            echo "  zig build run-hello-c   run C example"
            echo "  zig build run-hello-zig run Zig example"
            echo "  bear -- zig build       regenerate compile_commands.json"
            echo "  doxygen Doxyfile        generate HTML docs -> docs/html/"
            echo ""
          '';
        };

        # -----------------------------------------------------------
        # Package
        # -----------------------------------------------------------
        packages.default = pkgs.stdenv.mkDerivation {
          pname   = "zetui";
          version = "0.1.0";
          src     = ./.;

          nativeBuildInputs = [ pkgs.zig ];

          buildPhase = ''
            export HOME=$TMPDIR
            zig build -Doptimize=ReleaseSafe --prefix $out
          '';

          # zig build --prefix already copies to $out
          installPhase = "true";

          meta = {
            description = "ANSI/C89 TUI library with Zig bindings";
            license     = pkgs.lib.licenses.unlicense;
            platforms   = pkgs.lib.platforms.linux;
          };
        };

      });
}
