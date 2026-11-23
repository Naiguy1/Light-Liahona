#include <Arduino.h>
#include "jog.h"
#include "commands.h"
#include "light.h"

static const uint8_t MOTOR_PINS[] = {2, 3, 4, 5};
static jog_state motor;
static command_parser parser;

static void applyOutputs() {
    for (uint8_t i = 0; i < 4; ++i) {
        if (!(motor.outputs & (1U << i))) digitalWrite(MOTOR_PINS[i], LOW);
    }
    for (uint8_t i = 0; i < 4; ++i) {
        if (motor.outputs & (1U << i)) digitalWrite(MOTOR_PINS[i], HIGH);
    }
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
    Serial.print(F(" deg CCW from East; half-steps="));
    Serial.println(motor.position);
}

static void reportLight() {
    static const uint8_t SENSOR_PINS[] = {A0, A1, A2, A3};
    static const char SENSOR_NAMES[] = {'E', 'N', 'W', 'S'};
    uint16_t raw[4];
    for (uint8_t i = 0; i < 4; ++i) {
        raw[i] = (uint16_t)analogRead(SENSOR_PINS[i]);
        Serial.print(SENSOR_NAMES[i]);
        Serial.print(':');
        Serial.print(raw[i]);
        Serial.print(' ');
    }
    const int16_t angle = light_angle(raw);
    if (angle < 0) Serial.println(F("Light balanced; no direction"));
    else {
        Serial.print(F("Light angle: "));
        Serial.print(angle);
        Serial.println(F(" deg CCW from East"));
    }
}

static void help() {
    Serial.println(F("Commands + Enter: z=East zero, 0..360=angle, p=position, ?=help"));
    Serial.println(F("f=CCW jog, r=CW jog; l=light angle only; s=STOP immediately"));
    Serial.println(F("4096 half-steps/rev; angle moves require zero; test UNLOADED."));
}

static void handle(motor_command command) {
    if (command.kind == COMMAND_NONE) return;
    if (command.kind == COMMAND_STOP) {
        jog_stop(&motor);
        applyOutputs();
        Serial.println(F("Stopped; coils OFF; zero cleared"));
        return;
    }
    if (motor.active) {
        Serial.println(F("Busy; command ignored (s stops)"));
        return;
    }
    switch (command.kind) {
        case COMMAND_FORWARD:
        case COMMAND_REVERSE:
            jog_start(&motor, command.kind == COMMAND_FORWARD ? 1 : -1, millis());
            Serial.println(command.kind == COMMAND_FORWARD ? F("Jog CCW") : F("Jog CW"));
            break;
        case COMMAND_ZERO:
            jog_set_zero(&motor);
            Serial.println(F("Zero set: East 0.0 deg; coils OFF"));
            break;
        case COMMAND_ANGLE: {
            const move_result result = jog_move_to(&motor, command.degrees, millis());
            if (result == MOVE_NEEDS_ZERO) position();
            else if (result == MOVE_AT_TARGET) Serial.println(F("Already at target; coils OFF"));
            else if (result == MOVE_STARTED) {
                Serial.print(F("Target "));
                Serial.print(command.degrees == 360 ? 0 : command.degrees);
                Serial.print(F(" deg; "));
                Serial.print(motor.direction > 0 ? F("CCW; ") : F("CW; "));
                Serial.print(motor.remaining);
                Serial.println(F(" half-steps"));
            } else Serial.println(F("Angle rejected"));
            break;
        }
        case COMMAND_POSITION: position(); break;
        case COMMAND_LIGHT: reportLight(); break;
        case COMMAND_HELP: help(); break;
        default: Serial.println(F("Invalid command; use z, 0..360, f, r, p, l, s or ?")); break;
    }
}

void setup() {
    for (uint8_t i = 0; i < 4; ++i) {
        digitalWrite(MOTOR_PINS[i], LOW);
        pinMode(MOTOR_PINS[i], OUTPUT);
    }
    jog_init(&motor);
    command_init(&parser);
    Serial.begin(115200);
    Serial.println(F("Liahona manual position ready; coils OFF; zero required"));
    help();
}

void loop() {
    for (uint8_t n = 0; n < 16 && Serial.available(); ++n) {
        handle(command_feed(&parser, (char)Serial.read()));
    }
    const uint8_t previousOutputs = motor.outputs;
    const jog_result result = jog_tick(&motor, millis());
    if (motor.outputs != previousOutputs) applyOutputs();
    if (result == JOG_DONE) {
        Serial.println(F("Move complete; coils OFF"));
        position();
    }
    if (result == JOG_TIMED_OUT) Serial.println(F("Move timeout; coils OFF; zero cleared"));
}
