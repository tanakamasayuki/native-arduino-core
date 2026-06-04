# tests

English: [README.md](README.md)

`native-arduino-core` の最小ローカルテストです。素の `.ino` スケッチが
ローカル platform 経由でコンパイル・実行できることを確認します。

[`host-arduino-core`](https://github.com/tanakamasayuki/host-arduino-core)
と違い、`pytest-embedded` や TCP ベースのシリアルは使いません。native の
ランタイムは `stdout` に出力するだけなので、ハーネスは薄い
**stdout キャプチャ**方式です：

```
compile (arduino-cli) -> .out を実行 -> stdout を assert
```

```
tests/
  conftest.py          # platform リンク + run_sketch フィクスチャ
  pyproject.toml       # pytest のみ
  runtime/
    serial_basic/      # String / print(HEX) / println(float, n) / F() / map / digital I/O
    loop_count/        # loop() の反復 + millis()/delay()
```

各リーフディレクトリが 1 つのテスト対象（`<name>.ino` + `test_<name>.py`）です。

## 仕組み

`conftest.py` は 2 つのことを行います：

1. **platform 解決** — リポジトリを
   `<sketchbook>/hardware/lang-ship/native` に symlink し、リリースを
   インストールせずに `arduino-cli` がローカル platform
   (`lang-ship:native:generic`) を解決できるようにします。symlink が
   使えない環境ではディレクトリコピーにフォールバックし、セッション終了時に
   作成したものを削除します。
2. **`run_sketch` フィクスチャ** — `arduino-cli compile` でスケッチを
   ビルドし、生成された `.out` を実行して `stdout` / 戻り値を返します。

デフォルトの FQBN は `sdl2=disabled` なので、**テスト中にウィンドウは
一切開きません**。SDL2 パスを opt-in に保っているのはこのためです。

native の `loop()` は無限ループのため、**テストスケッチは `exit(0)` を
呼んで終了**します（core 自体は変更しません）。

## 実行

```sh
cd tests
uv run --with pytest pytest -v
```

## テストの追加

`tests/runtime/<name>/<name>.ino`（`exit(0)` で終了）と
`tests/runtime/<name>/test_<name>.py` を作成します：

```python
from pathlib import Path

def test_xxx(run_sketch):
    r = run_sketch(Path(__file__).parent)
    assert r.returncode == 0
    assert "期待する出力" in r.stdout
```

`run_sketch(sketch_dir, fqbn=..., timeout=..., input_text=...)` は
`.stdout` と `.returncode` を持つオブジェクトを返します。
