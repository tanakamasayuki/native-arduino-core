// Minimal Arduino compatibility wrappers for native core
#ifndef ARDUINO_COMPAT_H
#define ARDUINO_COMPAT_H

#include <chrono>
#include <thread>
#include <cstdio>
#include <mutex>
#include <unordered_map>
#include <cstdint>

#include "Stream.h"

#ifndef _WIN32
#include <sys/select.h>
#endif

// Serial: 出力は stdout、入力は stdin。Print/Stream を継承するので
// print(x, HEX) / println(float, n) / String / Printable などが全て使える。
class SerialClass : public Stream
{
public:
    void begin(unsigned long /*baud*/) {}
    void begin(unsigned long /*baud*/, uint32_t /*config*/) {}
    void end() {}

    using Print::write;
    size_t write(uint8_t value) override
    {
        std::lock_guard<std::mutex> lk(m_);
        std::fputc(static_cast<int>(value), stdout);
        return 1;
    }

    size_t write(const uint8_t *buffer, size_t size) override
    {
        std::lock_guard<std::mutex> lk(m_);
        return std::fwrite(buffer, 1, size, stdout);
    }

    // 入力: stdin から読む（非ブロッキング判定は POSIX のみ。Windows は 0）。
    int available() override
    {
        if (peeked_ >= 0)
        {
            return 1;
        }
#ifndef _WIN32
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(0, &fds);
        struct timeval tv{0, 0};
        if (select(1, &fds, nullptr, nullptr, &tv) > 0)
        {
            return 1;
        }
#endif
        return 0;
    }

    int read() override
    {
        if (peeked_ >= 0)
        {
            const int c = peeked_;
            peeked_ = -1;
            return c;
        }
        return std::getchar();
    }

    int peek() override
    {
        if (peeked_ < 0)
        {
            peeked_ = std::getchar();
        }
        return peeked_;
    }

    void flush() override { std::fflush(stdout); }

    operator bool() const { return true; }

private:
    std::mutex m_;
    int peeked_ = -1;
};

extern SerialClass Serial;

// --- タイミング ----------------------------------------------------------
inline uint32_t millis()
{
    static const auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count());
}

inline uint32_t micros()
{
    static const auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::microseconds>(now - start).count());
}

inline void delay(unsigned long ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void delayMicroseconds(unsigned int us)
{
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

inline void yield() {}

// --- デジタル/アナログ I/O（スタブ） -------------------------------------
// digitalWrite で書いた値を digitalRead で読めるよう、状態は単一の map を共有する。
inline std::unordered_map<uint8_t, uint8_t> &__nativePinStates()
{
    static std::unordered_map<uint8_t, uint8_t> states;
    return states;
}

inline void pinMode(uint8_t /*pin*/, uint8_t /*mode*/) {}

inline void digitalWrite(uint8_t pin, uint8_t value)
{
    __nativePinStates()[pin] = value ? HIGH : LOW;
}

inline int digitalRead(uint8_t pin)
{
    auto &states = __nativePinStates();
    auto it = states.find(pin);
    return it == states.end() ? LOW : it->second;
}

inline int analogRead(uint8_t /*pin*/) { return 0; }
inline void analogWrite(uint8_t /*pin*/, int /*value*/) {}

#endif // ARDUINO_COMPAT_H
