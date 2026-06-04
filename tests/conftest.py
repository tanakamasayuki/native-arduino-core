"""native-arduino-core の最小テストハーネス。

仕組み:
  1. リポジトリを <sketchbook>/hardware/lang-ship/native に symlink して、
     リリースをインストールせずに fqbn `lang-ship:native:generic` を
     ローカル解決させる（host-arduino-core と同方式）。
  2. `run_sketch` フィクスチャで「arduino-cli compile → 生成された .out を
     実行 → stdout をキャプチャ」する。

前提: テストスケッチは setup()/loop() のどこかで exit(0) を呼んで終了する
（native の loop() は本来無限ループのため。core 自体は変更しない方針）。
"""

import os
import shutil
import subprocess
import tempfile
from pathlib import Path

import pytest

REPO_ROOT = Path(__file__).resolve().parent.parent
PACKAGE_NAME = "lang-ship"
ARCH = "native"
FQBN = f"{PACKAGE_NAME}:{ARCH}:generic"


def _sketchbook_dir() -> Path:
    out = subprocess.run(
        ["arduino-cli", "config", "get", "directories.user"],
        check=True,
        capture_output=True,
        text=True,
    ).stdout.strip()
    if not out:
        out = str(Path.home() / "Arduino")
    return Path(out).expanduser()


@pytest.fixture(scope="session", autouse=True)
def _local_platform_link():
    """リポジトリを sketchbook の hardware/ に登録（symlink、無理ならコピー）。"""
    target = _sketchbook_dir() / "hardware" / PACKAGE_NAME / ARCH
    created = ""  # "symlink" | "copy" | "" (既存を再利用)

    if target.exists() or target.is_symlink():
        if target.is_symlink() and Path(os.readlink(target)) == REPO_ROOT:
            pass  # 既にこのリポジトリを指している
        else:
            pytest.fail(
                f"{target} が既に存在し {REPO_ROOT} を指していません。"
                "削除（または貼り直し）してから再実行してください。"
            )
    else:
        target.parent.mkdir(parents=True, exist_ok=True)
        try:
            target.symlink_to(REPO_ROOT, target_is_directory=True)
            created = "symlink"
        except OSError:
            shutil.copytree(str(REPO_ROOT), str(target))
            created = "copy"

    yield target

    if created == "symlink" and target.is_symlink():
        target.unlink()
    elif created == "copy" and target.exists():
        shutil.rmtree(target)


class SketchResult:
    def __init__(self, stdout: str, returncode: int):
        self.stdout = stdout
        self.returncode = returncode


@pytest.fixture
def run_sketch():
    """スケッチをコンパイル・実行して stdout を返す。

    使い方:
        def test_x(run_sketch):
            r = run_sketch(Path(__file__).parent / "serial_basic")
            assert "hello" in r.stdout
    """

    def _run(sketch_dir, fqbn: str = FQBN, timeout: float = 15.0,
             input_text: str | None = None) -> SketchResult:
        sketch_dir = Path(sketch_dir)
        with tempfile.TemporaryDirectory() as out_dir:
            compile_proc = subprocess.run(
                ["arduino-cli", "compile",
                 "--fqbn", fqbn,
                 "--output-dir", out_dir,
                 "--warnings", "none",
                 str(sketch_dir)],
                capture_output=True,
                text=True,
            )
            if compile_proc.returncode != 0:
                pytest.fail(
                    f"compile failed for {sketch_dir}\n"
                    f"stdout:\n{compile_proc.stdout}\n"
                    f"stderr:\n{compile_proc.stderr}"
                )
            outs = list(Path(out_dir).glob("*.out"))
            assert outs, f"no .out produced in {out_dir}: {os.listdir(out_dir)}"
            proc = subprocess.run(
                [str(outs[0])],
                input=input_text,
                capture_output=True,
                text=True,
                timeout=timeout,
            )
            return SketchResult(proc.stdout, proc.returncode)

    return _run
