
# native-arduino-core（日本語）

日本語 | English: [README.md](README.md)

このリポジトリは、Arduino IDE または Arduino CLI から PC にインストールされた system の gcc/g++ を利用してネイティブ実行ファイルをビルドする、非常にシンプルな Arduino ボードパッケージ（ボードマネージャ用）です。

特徴:
- GCC は system PATH から呼び出します。追加のクロスツールチェインは不要です。
- `Arduino.h` は最小実装で、`main()` から `setup()` と `loop()` を呼び出すだけの構成です。
- `upload` コマンドは、ビルドされた実行ファイルをそのまま実行します（PC 上での実行を想定）。

ボードID: `lang-ship:native:generic`

含まれるファイル:
- `hardware/lang-ship/native/1.0.0/` : ボードコア（`platform.txt`, `boards.txt`, `cores/native/`）
- `examples/BlankSketch/BlankSketch.ino` : 確認用の最小スケッチ
- `.github/workflows/release.yml` : タグ push で Release を作るワークフロー（手動実行も可）
- `package_index.json` : Boards Manager 用 index（このリポジトリの v1.0.0 に紐づく設定）

ローカルで簡単に試す方法:

1. system の gcc/g++ が PATH 上にあることを確認します。
2. リポジトリルートで以下を実行して簡易ビルドを行えます（Linux/Mac/Bash 環境）:

```bash
# build ディレクトリを作成して、core とサンプルをコンパイルします
mkdir -p build
cp examples/BlankSketch/BlankSketch.ino build/BlankSketch.cpp
g++ -std=gnu++11 -O2 -I hardware/lang-ship/native/1.0.0/cores/native \
	-o build/BlankSketch.out build/BlankSketch.cpp hardware/lang-ship/native/1.0.0/cores/native/main.cpp

# 実行ファイルをそのまま実行（upload 相当の動作）
./build/BlankSketch.out
```

Arduino CLI でボードマネージャ経由にする場合:

1. `package_index.json` を Boards Manager に登録するか、リリースアセットとして作成される `native-arduino-core-<version>.zip` をダウンロードして `hardware/` に展開します。
2. `arduino-cli board list` や `arduino-cli compile --fqbn lang-ship:native:generic /path/to/sketch` を使ってビルドを行います。

注意: 本コアの `upload` レシピはビルド結果の実行ファイルを単にシェルで実行するだけです。実機のフラッシュやシリアルへの書き込みは行いません。

フィードバックや要望があれば Issue を立ててください。

