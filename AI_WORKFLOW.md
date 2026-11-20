# Liahona Arduino development workflow

Use the approved Nano design in README.md and HARDWARE.md. Read those files, docs/nano-wiring.md and relevant source before edits. Confirm processor/bootloader before selecting the AVR build target. Pin assignments are approved; physical verification is a separate record. Do not ask for pin approval again unless proposing a change.

## Rules

- Make one narrow behavior change at a time, with the motor disabled unless motion is explicitly part of that task.
- Match firmware pin definitions to HARDWARE.md: E/N/W/S = A0/A1/A2/A3, driver IN1–IN4 = D2–D5, home = D6.
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

For a confirmed ATmega328P Nano: Arduino IDE → Arduino AVR Boards → Arduino Nano → matching processor/bootloader → actual port → Verify → Upload → Serial Monitor at sketch baud. The first Nano sketch has not yet been implemented. Current ESP-IDF files are not an uploadable Nano project.

## Task prompt

> Read README.md, HARDWARE.md, AI_WORKFLOW.md and relevant source. Use the approved classic Nano assignments. Make one narrow change: [behavior]. Keep the motor disabled unless this task involves motion. Explain the diff, assumptions, how to build/upload, and the serial/hardware checks. Distinguish design approval from bench verification.
