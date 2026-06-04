#ifndef ARDUINO_MINIMAL_H
#define ARDUINO_MINIMAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ユーザが定義する関数のプロトタイプ
extern void setup();
extern void loop();

// 基本的な型
typedef uint8_t byte;
typedef bool boolean;
typedef unsigned int word;

// Arduino 互換のコア型（host-arduino-core から移植: std だけに依存）
#include "WString.h"   // String
#include "Printable.h"
#include "Print.h"     // Print / print / println （HEX/DEC, F() 対応）
#include "Stream.h"    // Stream
#include "pgmspace.h"  // F(), PSTR(), PROGMEM

// --- ピン/論理レベル定数 -------------------------------------------------
#ifndef HIGH
#define HIGH 0x1
#endif
#ifndef LOW
#define LOW 0x0
#endif
#ifndef INPUT
#define INPUT 0x0
#endif
#ifndef OUTPUT
#define OUTPUT 0x1
#endif
#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif
#ifndef INPUT_PULLDOWN
#define INPUT_PULLDOWN 0x3
#endif

#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif

#ifndef CHANGE
#define CHANGE 1
#endif
#ifndef FALLING
#define FALLING 2
#endif
#ifndef RISING
#define RISING 3
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

// --- 数学定数 ------------------------------------------------------------
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif
#ifndef HALF_PI
#define HALF_PI 1.5707963267948966192313216916398
#endif
#ifndef TWO_PI
#define TWO_PI 6.283185307179586476925286766559
#endif
#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.017453292519943295769236907684886
#endif
#ifndef RAD_TO_DEG
#define RAD_TO_DEG 57.295779513082320876798154814105
#endif
#ifndef EULER
#define EULER 2.718281828459045235360287471352
#endif

// --- ビット/バイト操作マクロ ---------------------------------------------
#define bit(b) (1UL << (b))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) ((bitvalue) ? bitSet(value, bit) : bitClear(value, bit))
#define lowByte(w) ((uint8_t)((w) & 0xff))
#define highByte(w) ((uint8_t)((w) >> 8))
#define sq(x) ((x) * (x))
#ifndef _BV
#define _BV(b) (1UL << (b))
#endif

#define radians(deg) ((deg) * DEG_TO_RAD)
#define degrees(rad) ((rad) * RAD_TO_DEG)

// --- 数学/ユーティリティ関数（マクロ衝突を避けるため関数で定義） ---------
template <typename T>
inline const T &min(const T &a, const T &b) { return b < a ? b : a; }

template <typename T>
inline const T &max(const T &a, const T &b) { return a < b ? b : a; }

template <typename T>
inline T constrain(T value, T low, T high) { return value < low ? low : (value > high ? high : value); }

// abs は math.h / stdlib.h の global ::abs に委ねる（テンプレート定義は std::abs と曖昧になる）

inline long map(long x, long in_min, long in_max, long out_min, long out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

inline long random(long howbig) { return howbig <= 0 ? 0 : std::rand() % howbig; }
inline long random(long howsmall, long howbig)
{
    return howsmall >= howbig ? howsmall : howsmall + random(howbig - howsmall);
}
inline void randomSeed(unsigned long seed) { std::srand(static_cast<unsigned int>(seed)); }

inline uint16_t makeWord(uint16_t w) { return w; }
inline uint16_t makeWord(uint8_t h, uint8_t l) { return (static_cast<uint16_t>(h) << 8) | l; }

// Serial / millis / delay / digital I/O など（stdout バックエンド）
#include "ArduinoCompat.h"

#endif // ARDUINO_MINIMAL_H
