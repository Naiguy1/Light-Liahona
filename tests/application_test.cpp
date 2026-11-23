#include <cassert>
#include <iostream>
#include "Arduino.h"
#include "EEPROM.h"

uint32_t mock_now;
uint16_t mock_sensors[4] = {165, 155, 165, 75};
int mock_pins[32];
unsigned mock_high_writes;
MockSerial Serial;
MockEEPROM EEPROM;
void setup();
void loop();

static bool coilsOff() {
    return !mock_pins[2] && !mock_pins[3] && !mock_pins[4] && !mock_pins[5];
}
static void send(const char *text) {
    Serial.input = text;
    while (Serial.available()) loop();
}
static void advance(uint32_t duration) {
    for (uint32_t elapsed = 0; elapsed < duration; elapsed += 2) {
        mock_now += 2; loop();
    }
}
static void setAll(uint16_t value) {
    for (auto &sensor : mock_sensors) sensor = value;
}

int main() {
    setup();
    assert(coilsOff() && EEPROM.writes == 0);
    assert(Serial.output.find("built-in verified defaults") != std::string::npos);
    mock_sensors[1] = 956; // North light.
    send("t\n");
    advance(500);
    assert(mock_high_writes == 0); // Cannot track until zeroed.
    send("z\nt\n");
    advance(1000);
    assert(mock_high_writes > 0 && !coilsOff());
    send("s\n");
    assert(coilsOff());
    unsigned stopped = mock_high_writes;
    advance(1000);
    assert(mock_high_writes == stopped); // No automatic restart.
    send("t\n");
    advance(300);
    assert(mock_high_writes == stopped); // Stop cleared reference.
    setAll(1023); // Balanced saturated scene: no direction.
    send("z\nt\n");
    advance(500);
    assert(mock_high_writes == stopped);
    send("s\n");

    // Capture extrema, keeping coils off and EEPROM unchanged until completion.
    send("c\n");
    setAll(100); advance(100);
    setAll(900); advance(100);
    send("t\n90\nf\n"); // Calibration rejects all movement commands.
    advance(30000);
    assert(mock_high_writes == stopped && coilsOff());
    assert(EEPROM.writes == 2 && Serial.output.find("Calibration saved") != std::string::npos);
    unsigned savedWrites = EEPROM.writes;
    Serial.output.clear(); setup();
    assert(Serial.output.find("Calibration: saved") != std::string::npos);
    assert(coilsOff());
    // Symmetric saved endpoints yield exactly North from these readings.
    setAll(100); mock_sensors[1] = 900;
    send("l\n");
    assert(Serial.output.find("Light angle: 90 deg") != std::string::npos);
    assert(EEPROM.writes == savedWrites); // Reads/boot/tracking do not wear EEPROM.

    send("c\n"); setAll(500); advance(30100); // Inadequate range: retain saved profile.
    assert(EEPROM.writes == savedWrites);
    send("c\n"); setAll(0); advance(100); send("s\n"); advance(30100);
    assert(EEPROM.writes == savedWrites && coilsOff()); // Cancelled capture never saves.
    EEPROM.bytes[4] ^= 1; // Corrupt the stored values without updating checksum.
    Serial.output.clear(); setup();
    assert(Serial.output.find("built-in verified defaults") != std::string::npos);
    assert(coilsOff());

    // At the current zero, East light should not cause repeated tiny corrections.
    setAll(165); mock_sensors[0] = 967; mock_sensors[1] = 155; mock_sensors[3] = 75;
    send("z\nt\n"); advance(1000);
    assert(mock_high_writes == stopped);
    // A manual angle takes ownership: tracking must not undo it afterward.
    send("90\n");
    advance(3000);
    Serial.output.clear(); send("p\n");
    assert(Serial.output.find("90.0 deg") != std::string::npos && coilsOff());
    stopped = mock_high_writes;
    // A fresh startup never resumes tracking or retains the reference.
    Serial.output.clear(); setup();
    mock_sensors[0] = 165; mock_sensors[1] = 956;
    advance(1000);
    assert(mock_high_writes == stopped && coilsOff());
    std::cout << "application tests passed: zero/stop gating, tracking, calibration persistence/cancel/corruption, idle outputs\n";
}
