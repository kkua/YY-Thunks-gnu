# thunks-gnu

Build-time helper that compiles [YY-Thunks](https://github.com/Chuyu-Team/YY-Thunks)
(MinGW-w64 / GCC port) into `libyythunks.a` and hands it to the linker, so a Rust
binary can call newer Win32 APIs while still running on older Windows.

This crate exports no Rust symbols. Its only job is to be called from your
`build.rs`.

> 中文版见 [README.md](README.md)。

## Usage

```toml
# Cargo.toml
[build-dependencies]
thunks-gnu = "0.1"
```

```rust
// build.rs
fn main() {
    thunks_gnu::build();
}
```

That's it: `build()` compiles the vendored YY-Thunks source into
`$OUT_DIR/libyythunks.a` and emits the corresponding `cargo:rustc-link-*`
instructions (static lib first, so YY-Thunks wins over the system import
libraries).

## Requirements

| Requirement | Notes |
| --- | --- |
| Target | **`x86_64-pc-windows-gnu` only.** Any other target prints a `cargo::warning` and does nothing; `i686` and Clang fail fast with an explanatory message |
| Compiler | MinGW-w64 `g++` on `PATH` (or via `CXX`). GCC-only: the port relies on `-fno-toplevel-reorder` |
| Rust | 1.85+ (`edition = "2024"`) |
| Build time | ~5–10 s for the single translation unit (once per profile/out-dir) |

## Features → minimum supported Windows

The feature is the *lowest* system you want to support (upstream's
`YY_Thunks_Target`). The higher the target, the fewer thunks get compiled.

| Feature | `YY_Thunks_Target` | Meaning |
| --- | --- | --- |
| `vista` (default) | `__WindowsNT6` | Vista / Server 2008 |
| `win7` | `__WindowsNT6_1` | Windows 7 / Server 2008 R2 |
| `win8` | `__WindowsNT6_2` | Windows 8 / Server 2012 |
| `win10_10240` | `__WindowsNT10_10240` | Windows 10 1507 |
| `win10_19041` | `__WindowsNT10_19041` | Windows 10 2004 / 20H1 |

If several are enabled, the highest wins. `windows_vista` and `win10` are
aliases for `vista` and `win10_10240`.

Targets below Vista (XP / Server 2003) and 32-bit x86 are not supported by the
GCC port; enabling them cannot be expressed through these features.

## Notes and limitations

- **`x86_64-pc-windows-gnu` only.** This is a hard requirement, enforced at build
  time:
  - non-GNU Windows targets (e.g. `x86_64-pc-windows-msvc`) — the build script
    just prints a `cargo::warning` and returns (no `.a`, no `-l` emitted);
  - `i686-pc-windows-gnu` — upstream depends on MSVC's `__identifier` to build
    stdcall IAT symbols containing `@` (`_imp__Api@N`), which GCC cannot assemble,
    so the build script `panic`s;
  - Clang — the port relies on GCC-specific `-fno-toplevel-reorder` (upstream does
    auto-registration via a "first array element is a sentinel" trick that Clang
    does not honour), so the build script also `panic`s.
  In short: this crate is usable **only** with `x86_64-pc-windows-gnu`; every other
  target triple simply does not compile.
- **Link order matters.** `yythunks` is emitted before any system import
  libraries on purpose; if you add extra `-l` entries yourself, keep it first.
  Wrong order surfaces as `multiple definition` at link time, not as a silent
  loss of the thunks.
- **System import libraries.** Upstream declares its dependencies with
  `#pragma comment(lib, ...)`, which GCC ignores. Rust's `windows-gnu` target
  already links the common ones; if you hit an `undefined reference`, add the
  missing `-l` yourself (the full list is kept, commented out, at the end of
  `src/lib.rs`, and the CMake build generates it as
  `yythunks-link-requirements.txt`).
- **No C++ runtime needed.** The library is built with `-fno-exceptions
  -fno-unwind-tables`, so it never references `__gxx_personality_seh0` or
  `_Unwind_Resume`. You may still need `-lgcc` (`__chkstk_ms`,
  `__popcountdi2`) if your linker is not the `g++` driver.
- **SEH is degraded.** GCC has no `__try`/`__except`; the port executes the
  `__try` body and leaves the `__except` body unreachable. Normal paths behave
  like upstream, but malformed-PE / probing-read failure paths are no longer
  caught.
- **Same artifact size as the CMake build.** CMake uses no build type here (neither
  optimization nor debug info), while cc adds `-g -gdwarf-2` per the Cargo profile
  and `-ffunction-sections -fdata-sections` unconditionally; this crate turns them
  off, so a dev build's `libyythunks.a` matches the CMake output size (apart from a
  few bytes in the ar header). Optimization is forced to `-O0` as well: `-O3` was
  observed to crash with `STATUS_ACCESS_VIOLATION` during YY-Thunks' static
  registration, and the port is a thin shim where skipping optimization costs nothing.
- **Build output is silent by default.** The upstream code produces a lot of GCC
  warnings (including macro-redefinition ones, for which GCC has no `-Wno-`
  switch) plus its own `#pragma message` notes; none of them are printed. Set
  `THUNKS_GNU_SHOW_WARNINGS=1` to bring them back. Compiler *errors* are always
  printed, regardless of that switch.
- Everything needed is vendored under `vendor/`, so the build works offline and
  does not depend on the upstream tree.

## License

MIT, same as upstream YY-Thunks. Vendored sources are derived from
YY-Thunks — Copyright (c) 2018 Chuyu-Team. See [LICENSE](LICENSE).
