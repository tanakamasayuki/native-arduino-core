#include "Arduino.h"

#ifdef NATIVE_USE_SDL2

// SDL2 (LovyanGFX / M5GFX) backend.
//
// boards.txt の sdl2=enabled が NATIVE_USE_SDL2 を定義し -lSDL2 をリンクする。
// このとき LovyanGFX/M5GFX は SDL バックエンド (Panel_sdl) を選ぶ。
// Panel_sdl::main をメインスレッドで回し（SDL イベントポンプ）、setup()/loop()
// はワーカースレッドで実行する必要があるため、ここで起動する。
//
// 以前は各サンプルに sdl_main.cpp を置いて main() を提供していたが、この処理を
// コアに取り込んだので .ino だけで動く。
//
// Panel_sdl::main はライブラリ側で定義される。コアアーカイブはライブラリより先に
// コンパイルされるため、ヘッダを include せず前方宣言だけ行い、最終リンクで解決する。
// （host-arduino-core と同方式。ただし native は実ウィンドウを出すので
//   SDL_VIDEODRIVER=dummy は設定しない。）

#include <stdint.h>

namespace lgfx
{
    inline namespace v1
    {
        class Panel_sdl
        {
        public:
            static int main(int (*fn)(bool *), uint32_t msec_step_exec);
        };
    }
}

static int native_sdl_thunk(bool *running)
{
    setup();
    do
    {
        loop();
    } while (*running);
    return 0;
}

// weak にしておくと、サンプルが独自 main()（旧 sdl_main.cpp 等）を持つ場合は
// そちらが優先される。
#ifdef _WIN32
int main(int argc, char **argv);
#else
int main(int argc, char **argv) __attribute__((weak));
#endif
int main(int, char **)
{
    // 第2引数はブレークポイント時のステップ実行用。画面更新を確実にするための
    // スロー実行をミリ秒で指定できる。
    return lgfx::v1::Panel_sdl::main(native_sdl_thunk, 128);
}

#else // !NATIVE_USE_SDL2 — 通常の native ランタイム

// Provide a weak default main so user sketches can override main() if desired.
// The linker will prefer a user-defined strong main over this weak symbol.
int main(int argc, char **argv) __attribute__((weak));
int main(int argc, char **argv)
{
    // Arduino-style entrypoint: call setup() once, then loop() forever.
    setup();
    for (;;)
    {
        loop();
    }
    return 0;
}

#endif
