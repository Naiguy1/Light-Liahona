# Liahona Arduino development workflow

Use the approved Nano design in README.md and HARDWARE.md. Read those files, docs/nano-wiring.md and relevant source before edits. Confirm processor/bootloader before selecting the AVR build target. Pin assignments are approved; physical verification is a separate record. Do not ask for pin approval again unless proposing a change.

## Rules

- Make one narrow behavior change at a time, with the motor disabled unless motion is explicitly part of that task.
- Match firmware pin definitions to HARDWARE.md: E/N/W/S = A0/A1/A2/A3, driver IN1–IN4 = D2–D5, D6 reserved for an optional home/zero input (none installed).
- Explain files changed, assumptions, build/upload steps, serial baud rate and exact expected output or hardware behavior.
- Record actual tool versions, build results and observations; never claim a flash, wiring check or motion test without evidence.
- Preserve working milestones in Git. The legacy ESP-IDF scaffold is historical and cannot build for Nano.
- Use small fixed buffers and restrained logging for the intended ATmega328P target. Prefer elapsed-time scheduling with millis()/micros() and nonblocking motor updates. Avoid long delays that prevent switch monitoring or bounded homing.

## Milestones and acceptance criteria

1. Nano sketch and serial heartbeat only: print `Liahona Nano ready` and periodic heartbeat; motor disconnected.
2. East sensor A0 only: log raw values; cover and illuminate it to confirm repeatable changes.
3. Four labeled E/N/W/S channels: cover each separately to verify map.
4. Dark/bright calibration, normalization, direction estimate: verify orientation with one controlled light source. Recalibrate for Nano ADC and 5 V dividers; do not reuse ESP32 raw thresholds.
5. Filtering, low-light/low-confidence state and deadband: test darkness, uniform light and saturation without motion.
6. Home input only: debounce; confirm released HIGH and pressed LOW.
7. Small bounded stepper moves: confirm coil sequence, direction, mechanical travel and a stop/de-energize behavior. Do not assume exact output steps per revolution.
8. Bounded homing: define timeout, maximum steps, stuck-switch handling and fault behavior before testing.
9. Tracking and battery integration: check jitter, supply sag, resets and motor/sensor interference.

## Build and upload

For a confirmed ATmega328P Nano: Arduino IDE → Arduino AVR Boards → Arduino Nano → matching processor/bootloader → actual port → Verify → Upload → Serial Monitor at sketch baud. Current sketch: firmware/liahona/liahona.ino. Select ATmega328P (Old Bootloader), confirmed by successful user uploads. Heartbeat and raw sensor measurements are user verified; calibration is user verified. Current ESP-IDF files are not an uploadable Nano project.

## Task prompt

> Read README.md, HARDWARE.md, AI_WORKFLOW.md and relevant source. Use the approved classic Nano assignments. Make one narrow change: [behavior]. Keep the motor disabled unless this task involves motion. Explain the diff, assumptions, how to build/upload, and the serial/hardware checks. Distinguish design approval from bench verification.

Measured endpoints and installed resistor values are in docs/sensor-calibration.md. Keep normalization verification separate from motor milestones in docs/motor-bringup.md.

Current bench milestone (2026-11-23): separate firmware/motor_jog sketch, controlled via Arduino CLI in VSCodium. Use intact spare driver, original IN1–IN4 mapping, USB-fed Nano 5V supply as recorded in HARDWARE.md. Keep D6 unused. Do not enable homing or tracking based on assumed switch hardware. See docs/vscodium.md for build/upload/serial checks.

Current extension: serial manual zero and integer angle moves in the same motor_jog sketch. Use measured 4096 half-steps/revolution, f=CCW/r=CW viewed from shaft end. Keep it separate from sensor tracking. No EEPROM position restore or automatic homing; no installed home switch. Zero is required after reset/stop/timeout, and reported position is an open-loop estimate. See docs/manual-position.md for build/upload and unloaded checks.

Keep subsequent implementation simple: reuse verified calibration and the user-tested 2 ms half-step interval. The current change adds only a one-shot `l` sensor-angle diagnostic to the existing motor sketch; manual movement remains explicit. No new recalibration routine or control framework.

## Current firmware — flash once, configure through serial

The user requests a general, usable firmware without source edits, and a simple workflow. Current README is authoritative for operation: `sh run.sh` or the single Liahona upload/monitor task, then z/t/s. Verified defaults remain available. An optional c command collects sensor minima/maxima for 30 seconds and saves valid values to EEPROM. No physical zero/position is persisted. See docs/development.md for current checks; earlier milestone restrictions on automatic tracking and adding a calibration routine are superseded by this explicit user request. Keep tracking opt-in after manual zero and test unloaded until assembled travel is established.
