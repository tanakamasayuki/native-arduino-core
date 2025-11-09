# native-arduino-core

English | 日本語: [README.ja.md](README.ja.md)

Minimal Arduino core + Boards Manager package that drives the host PC’s native `gcc` / `g++` toolchain. Sketches compile into regular executables and the “upload” recipe simply runs that executable on the host.

## Highlights

- Architecture `native`, board prefix `lang-ship:native`.
- Core files live in `cores/native` and provide a tiny `Arduino.h` plus `ArduinoCompat.*` (Serial, timing, digital/analog stubs, weak `main()`).
- Upload recipe (`tools.run.upload`) executes `{build.path}/{build.project_name}.out` (Windows launches through `cmd /c start`).
- Ships with a “library” named `Native` that only holds example sketches (Plane/* and SDL2/*) plus helper `sdl_main.cpp` files.
- Releases are published as `native-arduino-core-<version>.zip` and indexed via `package_index.json`, served from GitHub Pages:  
  `https://tanakamasayuki.github.io/native-arduino-core/package_index.json`

## Boards and menus

### Generic board

- `lang-ship:native:generic` exposes four menus defined in `boards.txt`:
  - `sdl2`: `disabled` / `enabled`. When enabled it injects `-DNATIVE_USE_SDL2 -lSDL2`. Requires SDL2 dev packages on the host.
  - `m5gfx_board`: selects one of the `M5Unified`/`M5GFX` presets (e.g. `board_M5Stack`, `board_M5StickCPlus2`, `board_M5Tab5`). Adds macros such as `-DM5GFX_BOARD=<board>` and `-DM5GFX_SHOW_FRAME`.
  - `m5gfx_scale`: scaling factor (`x1`, `x2`, `x3`, `x4`, `x8`) mapped to `-DM5GFX_SCALE=<n>`.
  - `m5gfx_rotation`: rotation preset (`r0`…`r7`) mapped to `-DM5GFX_ROTATION=<n>`.

### Preconfigured SDL2 + M5 presets

`boards.txt` also defines convenience FQBNs that keep SDL2 enabled and pin a board macro for each device family:

```
lang-ship:native:SLD2_M5Stack
lang-ship:native:SLD2_M5StackCore2
lang-ship:native:SLD2_M5StackCoreS3
lang-ship:native:SLD2_M5StackCoreInk
lang-ship:native:SLD2_M5StickCPlus
lang-ship:native:SLD2_M5StickCPlus2
lang-ship:native:SLD2_M5Dial
lang-ship:native:SLD2_M5Tab5
```

Use them when you want `LovyanGFX` / `M5Unified` sketches to pop up in an SDL2 window without manually setting menu options.

## Repository layout → package contents

- `platform.txt` & `boards.txt`: live at repo root and are copied verbatim into the ZIP.
- `cores/native/Arduino.h`, `ArduinoCompat.*`, `main.cpp`: minimal runtime that calls `setup()` exactly once, then loops forever.
- `libraries/Native/`: metadata + examples
  - `examples/Plane/BlankSketch` and `FullApi`: quick smoke tests for Serial/timing/digital stubs.
  - `examples/SDL2/LovyanGFXBasic` and `M5UnifiedBasic`: demonstrate how to pair SDL2 builds with LovyanGFX/M5Unified (each folder contains a `sketch.yaml` capturing menu options, required libraries, and the Boards Manager URL).
  - `src/dummy.h`: placeholder so Arduino IDE accepts the library.
- `scripts/build_package.py`: copies `cores`, `libraries`, `boards.txt`, `platform.txt`, zips them, computes SHA-256, and updates `package_index.json`.
- `.github/workflows/release.yml`: runs the packaging script on tag push or manual dispatch, pushes `package_index.json` back to `main`, publishes the ZIP + index to GitHub Releases, and mirrors the whole `package/` folder (including an updated `package_index.json`) to the `gh-pages` branch.
- `package_index.json`: checked in for convenience; the release workflow keeps it synchronized with the latest build.

## Getting started with Arduino CLI / IDE

1. Make sure host `gcc`/`g++` (and `SDL2` dev files if you plan to enable the SDL2 menu) are on `PATH`.
2. Register the Boards Manager index:

   ```bash
   arduino-cli config add board_manager.additional_urls https://tanakamasayuki.github.io/native-arduino-core/package_index.json
   arduino-cli core update-index
   arduino-cli core install lang-ship:native
   ```

3. Compile an example sketch (no serial port is needed because the program runs locally):

   ```bash
   arduino-cli compile \
     --fqbn lang-ship:native:generic \
     --build-path ./build \
     --clean \
     libraries/Native/examples/Plane/BlankSketch
   ```

4. Run the produced executable (this is also what `arduino-cli upload` would do):

   ```bash
   ./build/BlankSketch.ino.out
   ```

5. To enable SDL2 + M5 options, extend the FQBN the same way the bundled `sketch.yaml` files do, e.g.

   ```bash
   arduino-cli compile \
     --fqbn lang-ship:native:generic:sdl2=enabled,m5gfx_board=board_M5Stack,m5gfx_scale=x2,m5gfx_rotation=r0 \
     libraries/Native/examples/SDL2/M5UnifiedBasic
   ```

   Each example folder also contains a `sketch.yaml`. With Arduino CLI 0.35+ you can run the exact profile recorded in that file:

   ```bash
   arduino-cli compile --profile generic libraries/Native/examples/Plane/BlankSketch
   arduino-cli compile --profile generic libraries/Native/examples/SDL2/M5UnifiedBasic
   ```

   Selecting a profile automatically applies the stored FQBN, Boards Manager URL, and listed library dependencies (LovyanGFX, M5Unified, etc.).

## Manual build without Arduino CLI

You can also build the minimal core directly with `g++`:

```bash
mkdir -p build
cp libraries/Native/examples/Plane/BlankSketch/BlankSketch.ino build/BlankSketch.cpp
g++ -std=gnu++11 -O2 -I cores/native \
    -o build/BlankSketch.out \
    build/BlankSketch.cpp cores/native/main.cpp
./build/BlankSketch.out
```

This is useful for quickly validating the core without installing Boards Manager entries.

## Packaging & releasing your own build

1. Update sources, then run:

   ```bash
   python3 scripts/build_package.py --version 0.1.23 --repo tanakamasayuki/native-arduino-core
   ```

   - Copies `cores/` and `libraries/` into `package/native-arduino-core/`.
   - Copies `platform.txt` and `boards.txt`.
   - Produces `native-arduino-core-0.1.23.zip`.
   - Computes SHA-256 and adds (or replaces) the corresponding entry inside `package_index.json`.

2. Commit the updated `package_index.json`, tag `v0.1.23`, and push.
3. The `Build and Release Native Arduino Core` workflow creates/publishes the ZIP + index and mirrors `package/` to GitHub Pages so the Boards Manager URL stays fresh.

## Limitations & notes

- There is no flashing/upload to external hardware; sketches always execute on the host PC.
- `Serial.available()` / `Serial.read()` are stubbed (always no data) because there is no serial device attached.
- SDL2 mode depends on the system’s SDL2 runtime; install `libsdl2-dev` (Linux), `brew install sdl2` (macOS), or the official development package (Windows/MSYS2).
- `M5GFX_*` macros only toggle compile-time behaviour; you still need to install the actual `M5Unified` / `M5GFX` libraries from Arduino Library Manager or git.

Issues and feature requests are welcome in the tracker.
