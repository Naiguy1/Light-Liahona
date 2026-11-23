#include <Arduino.h>
#include "jog.h"

// Approved mapping for the intact spare ULN2003 board. IN5..IN7 unused.
static const uint8_t MOTOR_PINS[] = {2, 3, 4, 5};
static jog_state motor;

static void applyOutputs() {
    // Turn unwanted channels off before enabling wanted channels.
    for (uint8_t i = 0; i < 4; ++i) {
        if (!(motor.outputs & (1U << i))) digitalWrite(MOTOR_PINS[i], LOW);
    }
    for (uint8_t i = 0; i < 4; ++i) {
        if (motor.outputs & (1U << i)) digitalWrite(MOTOR_PINS[i], HIGH);
    }
}

static void help() {
    Serial.println(F("f = forward jog, r = reverse jog, s = stop/release, ? = help"));
    Serial.println(F("128 half-steps per jog; direction and shaft angle uncalibrated."));
}

void setup() {
    // Preload LOW before switching each pin to output. No startup motion.
    for (uint8_t i = 0; i < 4; ++i) {
        digitalWrite(MOTOR_PINS[i], LOW);
        pinMode(MOTOR_PINS[i], OUTPUT);
    }
    jog_init(&motor);
    Serial.begin(115200);
    Serial.println(F("Liahona motor jog ready; coils OFF"));
    help();
}

void loop() {
    // Process a bounded number of characters so serial traffic cannot starve stepping.
    for (uint8_t n = 0; n < 16 && Serial.available(); ++n) {
        const char command = (char)Serial.read();
        if (command == 's') {
            jog_stop(&motor);
            applyOutputs();
            // Discard already queued motion commands after a stop.
            while (Serial.available()) Serial.read();
            Serial.println(F("Stopped; coils OFF"));
            return;
        }
        if (command == 'f' || command == 'r') {
            if (jog_start(&motor, command == 'f' ? 1 : -1, millis())) {
                Serial.println(command == 'f' ? F("Jog forward") : F("Jog reverse"));
            } else {
                Serial.println(F("Busy; command ignored"));
            }
        } else if (command == '?' && !motor.active) {
            help();
        }
        // Ignore CR/LF and all other characters. No unbounded command buffer.
    }
    const uint8_t previousOutputs = motor.outputs;
    const jog_result result = jog_tick(&motor, millis());
    if (motor.outputs != previousOutputs) applyOutputs();
    if (result == JOG_DONE) Serial.println(F("Jog complete; coils OFF"));
    if (result == JOG_TIMED_OUT) Serial.println(F("Jog timeout; coils OFF"));
}
