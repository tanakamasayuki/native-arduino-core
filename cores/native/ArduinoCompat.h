// Minimal Arduino compatibility wrappers for native core
#ifndef ARDUINO_COMPAT_H
#define ARDUINO_COMPAT_H

#include <chrono>
#include <thread>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <cstdint>
#include <string>

// Simple Serial-like class
class SerialClass
{
public:
    void begin(unsigned long /*baud*/) { /* no-op for native */ }

    template <typename T>
    void print(const T &v)
    {
        std::lock_guard<std::mutex> lk(m_);
        std::cout << v;
    }

    template <typename T>
    void println(const T &v)
    {
        std::lock_guard<std::mutex> lk(m_);
        std::cout << v << std::endl;
    }

    void println()
    {
        std::lock_guard<std::mutex> lk(m_);
        std::cout << std::endl;
    }

    size_t write(const char *buf, size_t len)
    {
        std::lock_guard<std::mutex> lk(m_);
        std::cout.write(buf, len);
        return len;
    }

    // minimal input emulation (not implemented)
    int available() { return 0; }
    int read() { return -1; }

private:
    std::mutex m_;
};

extern SerialClass Serial;

// Timing helpers
using ms_t = std::chrono::milliseconds;
inline uint32_t millis()
{
    static const auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return static_cast<uint32_t>(std::chrono::duration_cast<ms_t>(now - start).count());
}

inline void delay(unsigned long ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void delayMicroseconds(unsigned int us)
{
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

// Digital I/O stubs
enum PinMode
{
    INPUT = 0,
    OUTPUT = 1
};
inline void pinMode(int pin, PinMode mode)
{
    static std::unordered_map<int, int> modes;
    modes[pin] = static_cast<int>(mode);
}

inline void digitalWrite(int pin, int value)
{
    static std::unordered_map<int, int> states;
    states[pin] = value ? 1 : 0;
}

inline int digitalRead(int pin)
{
    static std::unordered_map<int, int> states;
    auto it = states.find(pin);
    return it == states.end() ? 0 : it->second;
}

// Analog stubs
inline int analogRead(int /*pin*/) { return 0; }
inline void analogWrite(int /*pin*/, int /*value*/) {}

#endif // ARDUINO_COMPAT_H
