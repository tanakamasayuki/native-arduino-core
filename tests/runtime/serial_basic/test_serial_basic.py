from pathlib import Path


def test_serial_basic(run_sketch):
    r = run_sketch(Path(__file__).parent)
    assert r.returncode == 0
    out = r.stdout

    assert "hello world" in out
    assert "dec=42 hex=2A" in out      # 基数指定 print(n, HEX)
    assert "pi=3.142" in out           # 浮動小数点の桁数指定
    assert "flash" in out              # F() マクロ
    assert "map=50" in out             # map()
    assert "led=1" in out              # digitalWrite -> digitalRead が共有状態
    assert out.strip().endswith("done")
