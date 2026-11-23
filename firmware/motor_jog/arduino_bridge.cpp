#include <Arduino.h>
#include <EEPROM.h>
#include "jog.h"
#include "commands.h"
#include "light.h"

static const uint8_t MOTOR_PINS[] = {2, 3, 4, 5};
static const uint8_t SENSOR_PINS[] = {A0, A1, A2, A3};
static const char SENSOR_NAMES[] = {'E', 'N', 'W', 'S'};
static const uint16_t CALIBRATION_TAG = 0x4c32;
static const uint32_t CALIBRATION_MS = 30000UL;
struct SavedCalibration { uint16_t tag; light_calibration values; uint16_t checksum; };

static jog_state motor;
static command_parser parser;
static light_calibration calibration;
static light_calibration candidate;
static bool tracking;
static bool calibrating;
static uint32_t calibrationStarted;
static uint32_t lastCalibrationSample;
static uint32_t lastCalibrationNotice;
static uint32_t lastTrackingSample;

static void readSensors(uint16_t raw[4]) {
    for (uint8_t i = 0; i < 4; ++i) raw[i] = (uint16_t)analogRead(SENSOR_PINS[i]);
}

static bool loadCalibration(light_calibration *values) {
    SavedCalibration saved;
    EEPROM.get(0, saved);
    if (saved.tag != CALIBRATION_TAG || !light_calibration_valid(&saved.values) ||
        saved.checksum != light_calibration_checksum(&saved.values)) return false;
    *values = saved.values;
    return true;
}

static bool saveCalibration() {
    // Invalidate first; publish the tag last so interrupted writes fall back to defaults.
    SavedCalibration saved = {0, calibration, light_calibration_checksum(&calibration)};
    EEPROM.put(0, saved);
    EEPROM.put(0, CALIBRATION_TAG);
    light_calibration check;
    return loadCalibration(&check) &&
           light_calibration_checksum(&check) == saved.checksum;
}

static void applyOutputs() {
    for (uint8_t i = 0; i < 4; ++i)
        if (!(motor.outputs & (1U << i))) digitalWrite(MOTOR_PINS[i], LOW);
    for (uint8_t i = 0; i < 4; ++i)
        if (motor.outputs & (1U << i)) digitalWrite(MOTOR_PINS[i], HIGH);
}

static void position() {
    if (!motor.position_known) {
        Serial.println(F("Position unknown; align with East, then send z + Enter."));
        return;
    }
    const uint16_t tenths = jog_angle_tenths(&motor);
    Serial.print(F("Estimated position: "));
    Serial.print(tenths / 10);
    Serial.print('.');
    Serial.print(tenths % 10);
    Serial.println(F(" deg CCW from East"));
}

static void reportLight() {
    uint16_t raw[4];
    readSensors(raw);
    for (uint8_t i = 0; i < 4; ++i) {
        Serial.print(SENSOR_NAMES[i]); Serial.print(':');
        Serial.print(raw[i]); Serial.print(' ');
    }
    const int16_t angle = light_angle_calibrated(raw, &calibration);
    if (angle < 0) Serial.println(F("Light weak/balanced; holding"));
    else {
        Serial.print(F("Light angle: ")); Serial.print(angle);
        Serial.println(F(" deg CCW from East"));
    }
}

static void help() {
    Serial.println(F("z = set East zero, t = track light, s = stop"));
    Serial.println(F("Optional: c = 30-second calibration, saved for next power-up"));
    Serial.println(F("Also: l = light reading, p = position, f/r = jog, 0..360 = angle"));
    Serial.println(F("Press Enter after commands. Test unloaded; tracking starts only with t."));
}

static void beginCalibration() {
    tracking = false;
    calibrating = true;
    calibrationStarted = lastCalibrationSample = lastCalibrationNotice = millis();
    for (uint8_t i = 0; i < 4; ++i) { candidate.covered[i] = 1023; candidate.bright[i] = 0; }
    Serial.println(F("Calibration: 30 seconds. Cover each sensor, then shine light on each."));
    Serial.println(F("Use the same light distance for each. Motor stays OFF. s cancels."));
}

static void pollCalibration(uint32_t now) {
    if (!calibrating) return;
    if ((uint32_t)(now - lastCalibrationSample) >= 20UL) {
        lastCalibrationSample = now;
        uint16_t raw[4]; readSensors(raw);
        for (uint8_t i = 0; i < 4; ++i) {
            if (raw[i] < candidate.covered[i]) candidate.covered[i] = raw[i];
            if (raw[i] > candidate.bright[i]) candidate.bright[i] = raw[i];
        }
    }
    if ((uint32_t)(now - calibrationStarted) >= CALIBRATION_MS) {
        calibrating = false;
        if (!light_calibration_valid(&candidate)) {
            Serial.println(F("Calibration rejected: each sensor needs a wider light/dark range. Previous settings kept."));
            return;
        }
        calibration = candidate;
        Serial.println(saveCalibration() ? F("Calibration saved. Ready; use z then t.") :
                       F("Calibration applied in RAM; EEPROM save failed."));
    } else if ((uint32_t)(now - lastCalibrationNotice) >= 5000UL) {
        lastCalibrationNotice = now;
        Serial.print(F("Calibration seconds left: "));
        Serial.println((CALIBRATION_MS - (uint32_t)(now - calibrationStarted) + 999UL) / 1000UL);
    }
}

static void handle(motor_command command) {
    if (command.kind == COMMAND_NONE) return;
    if (command.kind == COMMAND_STOP) {
        tracking = calibrating = false;
        jog_stop(&motor);
        applyOutputs();
        Serial.println(F("Stopped; tracking OFF; coils OFF; zero cleared"));
        return;
    }
    if (calibrating) {
        Serial.println(F("Calibrating; s cancels"));
        return;
    }
    if (motor.active) {
        Serial.println(F("Busy; command ignored (s stops)"));
        return;
    }
    switch (command.kind) {
        case COMMAND_FORWARD:
        case COMMAND_REVERSE:
            tracking = false;
            jog_start(&motor, command.kind == COMMAND_FORWARD ? 1 : -1, millis());
            Serial.println(command.kind == COMMAND_FORWARD ? F("Jog CCW") : F("Jog CW"));
            break;
        case COMMAND_ZERO:
            tracking = false;
            jog_set_zero(&motor);
            Serial.println(F("Zero set: East 0.0 deg; coils OFF"));
            break;
        case COMMAND_TRACK:
            if (!motor.position_known) position();
            else {
                tracking = true;
                lastTrackingSample = millis();
                Serial.println(F("Tracking ON; s stops"));
            }
            break;
        case COMMAND_CALIBRATE: beginCalibration(); break;
        case COMMAND_ANGLE: {
            tracking = false;
            const move_result result = jog_move_to(&motor, command.degrees, millis());
            if (result == MOVE_NEEDS_ZERO) position();
            else if (result == MOVE_AT_TARGET) Serial.println(F("Already at target; coils OFF"));
            else if (result == MOVE_STARTED) {
                Serial.print(F("Target ")); Serial.print(command.degrees == 360 ? 0 : command.degrees);
                Serial.print(F(" deg; ")); Serial.print(motor.direction > 0 ? F("CCW; ") : F("CW; "));
                Serial.print(motor.remaining); Serial.println(F(" half-steps"));
            } else Serial.println(F("Angle rejected"));
            break;
        }
        case COMMAND_POSITION: position(); break;
        case COMMAND_LIGHT: reportLight(); break;
        case COMMAND_HELP: help(); break;
        default: Serial.println(F("Unknown command; ? shows help")); break;
    }
}

static void pollTracking(uint32_t now) {
    if (!tracking || calibrating || motor.active) return;
    if (!motor.position_known) { tracking = false; return; }
    if ((uint32_t)(now - lastTrackingSample) < 250UL) return;
    lastTrackingSample = now;
    uint16_t raw[4]; readSensors(raw);
    const int16_t angle = light_angle_calibrated(raw, &calibration);
    if (angle < 0) return;
    const int16_t current = (int16_t)(jog_angle_tenths(&motor) / 10U);
    int16_t error = angle - current;
    if (error < 0) error = -error;
    if (error > 180) error = 360 - error;
    if (error <= 5) return; // Small deadband avoids chasing tiny changes.
    if (jog_move_to(&motor, (uint16_t)angle, now) == MOVE_STARTED) {
        Serial.print(F("Tracking target: ")); Serial.print(angle); Serial.println(F(" deg"));
    }
}

void setup() {
    for (uint8_t i = 0; i < 4; ++i) {
        digitalWrite(MOTOR_PINS[i], LOW);
        pinMode(MOTOR_PINS[i], OUTPUT);
    }
    jog_init(&motor);
    command_init(&parser);
    tracking = calibrating = false;
    Serial.begin(115200);
    Serial.println(F("Liahona ready; coils OFF; zero required"));
    if (loadCalibration(&calibration)) Serial.println(F("Calibration: saved"));
    else { calibration = LIGHT_DEFAULT_CALIBRATION; Serial.println(F("Calibration: built-in verified defaults")); }
    help();
}

void loop() {
    for (uint8_t n = 0; n < 16 && Serial.available(); ++n)
        handle(command_feed(&parser, (char)Serial.read()));
    const uint32_t now = millis();
    const uint8_t previousOutputs = motor.outputs;
    const jog_result result = jog_tick(&motor, now);
    if (motor.outputs != previousOutputs) applyOutputs();
    if (result == JOG_DONE) { Serial.println(F("Move complete; coils OFF")); position(); }
    if (result == JOG_TIMED_OUT) {
        tracking = false;
        Serial.println(F("Move timeout; tracking OFF; coils OFF; zero cleared"));
    }
    pollCalibration(now);
    pollTracking(now);
}
