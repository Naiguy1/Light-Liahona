#include <Arduino.h>

// E / N / W / S. Endpoints measured by the user on 2026-11-20.
const uint8_t SENSOR_PINS[] = {A0, A1, A2, A3};
const char SENSOR_NAMES[] = {'E', 'N', 'W', 'S'};
const int COVERED[] = {165, 155, 165, 75};
const int BRIGHT[] = {967, 956, 971, 932};
unsigned long lastReport = 0;

// Tenths of a percent. Use 32-bit multiplication on the 16-bit AVR.
long normalizedTenths(int raw, uint8_t sensor) {
  long value = (raw - COVERED[sensor]) * 1000L /
               (BRIGHT[sensor] - COVERED[sensor]);
  if (value < 0) return 0;
  if (value > 1000) return 1000;
  return value;
}

void setup() {
  // No motor or home-switch pins are initialized in this sensor milestone.
  Serial.begin(115200);
  Serial.println(F("Liahona normalized sensor test; motor power disconnected"));
}

void loop() {
  unsigned long now = millis();
  if (now - lastReport < 250UL) return;
  lastReport = now;
  for (uint8_t i = 0; i < 4; ++i) {
    int raw = analogRead(SENSOR_PINS[i]);
    long value = normalizedTenths(raw, i);
    if (i != 0) Serial.print(F("  "));
    Serial.print(SENSOR_NAMES[i]);
    Serial.print(F(":"));
    Serial.print(raw);
    Serial.print(F("/"));
    Serial.print(value / 10);
    Serial.print('.');
    Serial.print(value % 10);
    Serial.print('%');
  }
  Serial.println();
}
