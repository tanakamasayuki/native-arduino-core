from pathlib import Path


def test_loop_count(run_sketch):
    r = run_sketch(Path(__file__).parent)
    assert r.returncode == 0
    lines = [ln for ln in r.stdout.splitlines() if ln]

    assert lines[0] == "start"
    assert "loop 0" in lines
    assert "loop 1" in lines
    assert "loop 2" in lines
    assert "loop 3" not in lines          # 3 回で exit
    assert "elapsed_ms_ok=1" in lines     # millis() が進んでいる
