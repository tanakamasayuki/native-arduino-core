
# native-arduino-core（日本語）

日本語 | English: [README.md](README.md)

ホスト PC に入っている `gcc` / `g++` を Arduino IDE / Arduino CLI から直接呼び出し、ネイティブ実行ファイルとしてスケッチをビルドする最小構成の Arduino コア + Boards Manager パッケージです。`upload` はビルドしたバイナリをそのまま起動します。

## ハイライト

- アーキテクチャ `native`、ボード ID は `lang-ship:native:*`。
- `cores/native` に最小限の `Arduino.h` / `ArduinoCompat.*` / `main.cpp` を同梱。`Serial` や `millis()` など基本 API をホスト上で模擬します。
- `tools.run.upload` レシピは `{build.path}/{build.project_name}.out` を実行するだけ（Windows は `cmd /c start` で起動）。
- `libraries/Native` は実際のライブラリコードではなく、Plane（素の API）と SDL2（LovyanGFX / M5Unified）向けのサンプル・`sketch.yaml` をまとめた「ライブラリ」です。
- リリース ZIP と `package_index.json` を GitHub Pages (`https://tanakamasayuki.github.io/native-arduino-core/package_index.json`) に公開。

## ボードとメニュー

### `lang-ship:native:generic`

`boards.txt` 上で以下の 4 メニューを持つ汎用ボードです。

- `sdl2`: `enabled` で `-DNATIVE_USE_SDL2 -lSDL2` を付与。ホストに SDL2 開発環境が必要です。
- `m5gfx_board`: `board_M5Stack` / `board_M5StackCore2` / … / `board_M5Tab5` などを選択し、`-DM5GFX_BOARD=<board> -DM5GFX_SHOW_FRAME -DM5GFX_BACK_COLOR=0x222222u` を付与します。
- `m5gfx_scale`: `x1`〜`x8` で `-DM5GFX_SCALE=<n>` を切り替え。
- `m5gfx_rotation`: `r0`〜`r7` で `-DM5GFX_ROTATION=<n>` を切り替え。

### SDL2 + M5 プリセット

SDL2 を常時有効にしたボード ID も用意しています。LovyanGFX / M5Unified のサンプルと同じ設定をワンコマンドで選択できます。

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

## リポジトリ構成（= パッケージ内容）

- `platform.txt` / `boards.txt`: ルートにあり、ZIP にそのまま含めます。
- `cores/native/Arduino.h`, `ArduinoCompat.{h,cpp}`, `main.cpp`: `setup()` 呼び出し → 永久 `loop()` のみを提供。
- `libraries/Native/`
  - `examples/Plane/BlankSketch`, `FullApi`: Serial や timing/digital API の動作確認スケッチ。
  - `examples/SDL2/LovyanGFXBasic`, `M5UnifiedBasic`: SDL2 ウィンドウ + LovyanGFX/M5Unified 連携例。各フォルダに `sketch.yaml`, `sdl_main.cpp` を収録。
  - `src/dummy.h`: Arduino IDE が空ライブラリを嫌うためのダミーファイル。
- `scripts/build_package.py`: `cores` / `libraries` / `platform.txt` / `boards.txt` を `package/native-arduino-core/` にコピーし、ZIP 作成と SHA-256 計算、`package_index.json` 更新まで自動化。
- `.github/workflows/release.yml`: タグ push または手動実行で上記スクリプトを実行し、`package_index.json` を `main` にコミット、ZIP と index を Release に添付し、`package/` 以下を `gh-pages` へ公開。
- `package_index.json`: 最新リリースのメタデータを保持（ワークフローから自動更新）。

## 事前準備（gcc / SDL2 のインストール）

- **Windows**  
  1. MSYS2 を winget で導入：  
     ```powershell
     winget install MSYS2.MSYS2
     ```
  2. gcc / gdb と SDL2 を入れます：  
     ```bash
     C:\msys64\usr\bin\pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-gdb
     C:\msys64\usr\bin\pacman -S mingw-w64-ucrt-x86_64-SDL2
     ```
     SDL2 は LovyanGFX / M5Unified を使う場合に必須です。
  3. 環境変数 `PATH` に **必ず** `C:\msys64\ucrt64\bin` を追加します（`pacman` などを直接叩きたい場合は `C:\msys64\usr\bin`、MinGW ツールチェーンを併用するなら `C:\msys64\mingw64\bin` も追加）。  
     設定手順：Windows 設定 → システム → バージョン情報 → 「システムの詳細設定」 → 「環境変数」 → 「システム環境変数」の `Path` → 「編集」 → 「新規」で `C:\msys64\ucrt64\bin`（必要に応じて他のパス）を追加 → OK を押して閉じる → 新しいターミナルを開き直す。  
     追加後に `g++ --version` や `sdl2-config --version` で確認できます。
  4. Windows ネイティブ環境は I/O やプロセス生成のオーバーヘッドでビルド時間が長くなる傾向があります。可能であれば WSL（Ubuntu など）上で CLI を実行し、ホスト PC 側はエディタのみとする運用を推奨します。VS Code の *Arduino CLI Wrapper* 拡張（[Marketplace](https://marketplace.visualstudio.com/items?itemName=tanakamasayuki.vscode-arduino-cli-wrapper)）を使えば、Windows からリモート開発で WSL 上のビルド・実行をトリガーできます。

- **Linux / macOS**  
  SDL2 を含むネイティブ開発環境の整備手順は LovyanGFX 公式ドキュメント（[M5GFX/examples/PlatformIO_SDL](https://github.com/m5stack/M5GFX/tree/master/examples/PlatformIO_SDL)）が詳しいです。概要は以下のとおりです。
  - Linux（Ubuntu 系）：  
    ```bash
    sudo apt update
    sudo apt install build-essential libsdl2 libsdl2-dev
    ```
  - macOS：  
    ```bash
    xcode-select --install
    brew install sdl2
    ```
  SDL2 を使わない場合でも `gcc` / `g++`（Xcode Command Line Tools や build-essential）を先に入れておくとスムーズです。

## Arduino IDE での導入

1. Arduino IDE の *ファイル → 環境設定* を開き、**追加のボードマネージャのURL** に `https://tanakamasayuki.github.io/native-arduino-core/package_index.json` を追記します（複数ある場合はカンマ区切り）。
2. *ツール → ボード → ボードマネージャ* を開き、`Native` で検索して **Native Arduino Core**（lang-ship）をインストールします。
3. *ツール → ボード → Native Arduino Core → Native (Generic)* を選び、例として *ファイル → スケッチ例 → Native → Plane → BlankSketch* をビルドしてください。実行はホスト PC 上で行われるため、シリアルポート指定は不要です。
4. LovyanGFX / M5Unified の SDL デモを動かす場合は、コンパイル前に *ツール* メニューで `sdl2` を **Enabled**、`m5gfx_*` オプションを目的の値に設定します。

## Arduino CLI での導入

1. 上記の手順でホストに `gcc` / `g++`（SDL2 を使う場合は SDL2 ランタイム・開発ヘッダ）を用意。
2. Boards Manager に index URL を追加し、コアをインストール：

   ```bash
   arduino-cli config add board_manager.additional_urls https://tanakamasayuki.github.io/native-arduino-core/package_index.json
   arduino-cli core update-index
   arduino-cli core install lang-ship:native
   ```

3. サンプルをビルド：

   ```bash
   arduino-cli compile \
     --fqbn lang-ship:native:generic \
     --build-path ./build \
     --clean \
     libraries/Native/examples/Plane/BlankSketch
   ./build/BlankSketch.ino.out
   ```

4. SDL2 + M5 設定をつける例：

   ```bash
   arduino-cli compile \
     --fqbn lang-ship:native:generic:sdl2=enabled,m5gfx_board=board_M5Stack,m5gfx_scale=x2,m5gfx_rotation=r0 \
     libraries/Native/examples/SDL2/M5UnifiedBasic
   ```

5. 各サンプルフォルダには `sketch.yaml` が入っており、`arduino-cli compile --profile generic <sketch>` で保存済みプロフィール（FQBN / 追加 URL / 依存ライブラリ）をそのまま再現できます。

   ```bash
   arduino-cli compile --profile generic libraries/Native/examples/Plane/BlankSketch
   arduino-cli compile --profile generic libraries/Native/examples/SDL2/M5UnifiedBasic
   ```

   シリアルポートは不要なので `port: NONE` のままで問題ありません。

## CLI を使わない簡易ビルド

```bash
mkdir -p build
cp libraries/Native/examples/Plane/BlankSketch/BlankSketch.ino build/BlankSketch.cpp
g++ -std=gnu++11 -O2 -I cores/native \
    -o build/BlankSketch.out \
    build/BlankSketch.cpp cores/native/main.cpp
./build/BlankSketch.out
```

Arduino CLI を導入する前の動作確認に使えます。

## パッケージ化とリリースフロー

1. 例：`python3 scripts/build_package.py --version 0.1.23 --repo tanakamasayuki/native-arduino-core`
   - `package/native-arduino-core/` を作成し、`cores` / `libraries` / `platform.txt` / `boards.txt` をコピー。
   - `native-arduino-core-0.1.23.zip` を出力し SHA-256 を求める。
   - `package_index.json` に `0.1.23` を追加（既存バージョンがあれば置き換え）。
2. `package_index.json` をコミットし、`v0.1.23` タグを push。
3. GitHub Actions (`Build and Release Native Arduino Core`) が ZIP / index を Release と GitHub Pages に公開し、Boards Manager URL を最新化。

## 制限事項 / 注意

- 物理デバイスへの書き込みは行いません。生成物はホスト OS 上で直接実行されます。
- `Serial.available()` / `Serial.read()` はスタブ実装のため常に未入力扱いです。
- SDL2 メニューを使う場合はホスト側に SDL2 ランタイム + 開発用ヘッダが必須です。
- `M5GFX_*` マクロは LovyanGFX / M5Unified の挙動を切り替えるための定義であり、ライブラリ本体は別途 Arduino Library Manager 等からインストールしてください。

不具合報告・要望は Issues までお願いします。
