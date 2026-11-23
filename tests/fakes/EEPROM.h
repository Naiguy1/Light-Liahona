#ifndef TEST_EEPROM_H
#define TEST_EEPROM_H
#include <array>
#include <cstring>
struct MockEEPROM {
    std::array<unsigned char, 1024> bytes;
    unsigned writes = 0;
    MockEEPROM() { bytes.fill(0xff); }
    template<class T> void get(int address, T &value) {
        std::memcpy(&value, bytes.data() + address, sizeof(T));
    }
    template<class T> void put(int address, const T &value) {
        std::memcpy(bytes.data() + address, &value, sizeof(T)); ++writes;
    }
};
extern MockEEPROM EEPROM;
#endif
