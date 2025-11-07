#ifndef ARDUINO_MINIMAL_H
#define ARDUINO_MINIMAL_H

#include <stdint.h>

// ユーザが定義する関数のプロトタイプ
extern void setup();
extern void loop();

// 基本的な型やマクロを最小限に定義
typedef uint8_t byte;

#endif // ARDUINO_MINIMAL_H

// Include compatibility wrappers (Serial, millis, digital I/O, etc.)
#include "ArduinoCompat.h"
