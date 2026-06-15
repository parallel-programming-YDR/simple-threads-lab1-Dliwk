{
  description = "C++ dev environment for simple-threads-lab1";

  inputs = {
    nixpkgs.url = "nixpkgs";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = pkgs.mkShell {
          packages = with pkgs; [
            # build system
            cmake
            ninja

            # LSP, tooling, formatting
            # NOTE: clang-tools MUST come before clang so the *wrapped* clangd
            # (which knows the stdlib -isystem paths) wins in PATH. Otherwise the
            # clang setup-hook puts an unwrapped clangd first and it can't find
            # <atomic> etc. See https://blog.kotatsu.dev/posts/2024-04-10-nixpkgs-clangd-missing-headers/
            clang-tools   # clangd, clang-format, clang-tidy
            gdb

            # toolchain (clang/LLVM)
            clang
            lld
          ];

          # Make sure clangd uses the clang headers
          shellHook = ''
            export CC=clang
            export CXX=clang++
            echo "C++ devShell ready: $(clang --version | head -1)"
          '';
        };
      });
}
