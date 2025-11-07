# native-arduino-core

English | 日本語: [README.ja.md](README.ja.md)

This repository provides a minimal Arduino core and Boards Manager package that uses the system gcc/g++ installed on the host PC to build native executables.

Goals:
- Allow Arduino IDE / Arduino CLI to compile sketches using the system gcc/g++ (no cross toolchain required).
- The `upload` step simply executes the built binary locally instead of flashing a device.

Board ID:
- lang-ship:native:generic

Repository layout (important parts):
- `hardware/lang-ship/native/1.0.0/` : `platform.txt`, `boards.txt`, `cores/native/` (minimal core implementation)
- `examples/BlankSketch/BlankSketch.ino` : minimal sketch (only `setup()` and `loop()`)
- `.github/workflows/release.yml` : GitHub Actions workflow to create a release (tag-triggered and manual)
- `package_index.json` : Boards Manager package index for v1.0.0

Quick usage:
1. Add the URL to `package_index.json` (or the release ZIP produced by this repository) to Arduino IDE / Arduino CLI Boards Manager.
2. Install the board `lang-ship:native:generic`.
3. Open the sample sketch and build it. This assumes `gcc`/`g++` are available on PATH.

See the Japanese version `README.ja.md` for the Japanese translation.
