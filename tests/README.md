# tests

English | 日本語: [README.ja.md](README.ja.md)

Minimal local tests for `native-arduino-core`. They verify that a plain
`.ino` sketch compiles and runs through the local platform.

Unlike [`host-arduino-core`](https://github.com/tanakamasayuki/host-arduino-core),
these tests do **not** use `pytest-embedded` or a TCP-backed serial. The
native runtime just prints to `stdout`, so the harness is a thin
**stdout-capture** check:

```
compile (arduino-cli) -> run the .out -> assert on stdout
```

```
tests/
  conftest.py          # platform link + run_sketch fixture
  pyproject.toml       # pytest only
  runtime/
    serial_basic/      # String / print(HEX) / println(float, n) / F() / map / digital I/O
    loop_count/        # loop() iterations + millis()/delay()
```

Each leaf directory is one test target (`<name>.ino` + `test_<name>.py`).

## How it works

`conftest.py` does two things:

1. **Platform resolution** — symlinks the repository into
   `<sketchbook>/hardware/lang-ship/native` so `arduino-cli` resolves the
   local platform (`lang-ship:native:generic`) without installing a
   release. It falls back to a directory copy if symlinks are
   unavailable, and removes whatever it created at session end.
2. **`run_sketch` fixture** — compiles a sketch with
   `arduino-cli compile`, runs the produced `.out`, and returns its
   `stdout` / return code.

The default FQBN has `sdl2=disabled`, so **no window ever opens during
tests** — that is the whole reason the SDL2 path stays opt-in.

Because the native `loop()` runs forever, **test sketches call `exit(0)`**
to terminate (the core itself is left unchanged).

## Running

```sh
cd tests
uv run --with pytest pytest -v
```

## Adding a test

Create `tests/runtime/<name>/<name>.ino` (terminating with `exit(0)`) and
`tests/runtime/<name>/test_<name>.py`:

```python
from pathlib import Path

def test_xxx(run_sketch):
    r = run_sketch(Path(__file__).parent)
    assert r.returncode == 0
    assert "expected output" in r.stdout
```

`run_sketch(sketch_dir, fqbn=..., timeout=..., input_text=...)` returns an
object with `.stdout` and `.returncode`.
