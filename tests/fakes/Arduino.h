#ifndef TEST_ARDUINO_H
#define TEST_ARDUINO_H
#include <stdint.h>
#include <string>
#include <sstream>
#define F(text) text
#define LOW 0
#define HIGH 1
#define OUTPUT 1
#define A0 14
#define A1 15
#define A2 16
#define A3 17
extern uint32_t mock_now;
extern uint16_t mock_sensors[4];
extern int mock_pins[32];
extern unsigned mock_high_writes;
inline uint32_t millis() { return mock_now; }
inline int analogRead(uint8_t pin) { return mock_sensors[pin - A0]; }
inline void pinMode(uint8_t, int) {}
inline void digitalWrite(uint8_t pin, int level) {
    mock_pins[pin] = level;
    if (level) ++mock_high_writes;
}
struct MockSerial {
    std::string input, output;
    void begin(unsigned long) {}
    int available() { return (int)input.size(); }
    int read() { char c = input.front(); input.erase(0, 1); return c; }
    template<class T> void print(const T &value) {
        std::ostringstream stream; stream << value; output += stream.str();
    }
    template<class T> void println(const T &value) { print(value); output += '\n'; }
};
extern MockSerial Serial;
#endif
