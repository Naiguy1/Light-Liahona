# ESP32 to Nano migration record

Approved 2026-11-20: migrate the design to a classic 5 V Nano-style USB-C board and adopt A0/A1/A2/A3 for E/N/W/S, D2/D3/D4/D5 for ULN2003 IN1–IN4 and D6 for home. Processor marking is confirmed as ATmega328P from supplied photographs; bootloader selection remains to be established by upload.

## Completed in this documentation change

- Updated the README, hardware notes, verification worksheet and AI workflow.
- Added a Nano wiring reference and marked the old ESP32 wiring image as superseded.
- Recorded approval separately from unverified assembly and operation.

## Remaining implementation work

| Area | Required work |
| --- | --- |
| Firmware entry point | Introduce Arduino setup()/loop() and a serial heartbeat as the first isolated change |
| Project structure | Create the Arduino sketch and a single pin-definition source; retire or relocate ESP-IDF CMake/main files during the firmware migration |
| Logging | Replace ESP_LOGI with bounded Serial logging; document baud rate |
| ADC | Use analogRead on A0–A3 and recalibrate raw ranges, normalization and thresholds |
| Scheduling | Replace ESP-IDF facilities with cooperative elapsed-time scheduling |
| Switch | Debounce D6 and validate active-LOW operation before motion |
| Motion | Add timed ULN2003 stepping, travel limits, measured direction and bounded homing |
| Memory | Account for the intended ATmega328P's 2 KB SRAM; avoid large buffers and dynamic String accumulation |
| Power | Confirm clone power circuit, boost capability and a USB/external power selection method |
| Mechanical | Measure board and USB clearance, revise carrier as needed, preserve hardware where practical |
| Deliverables | Regenerate any physical wiring illustration and printable controller mount when their implementation is requested |

The existing ESP32 scaffold remains unchanged in this documentation-only commit. No Nano build, upload or hardware result is claimed. See AI_WORKFLOW.md for milestones and README.md for planned Arduino IDE instructions.

## Progress update, 2026-11-20

The prior implementation checklist above is historical planning. Heartbeat and raw four-sensor uploads are user verified using ATmega328P (Old Bootloader). firmware/liahona/liahona.ino now implements normalized serial readings, pending verification. The legacy CMake/main files remain separate. Motor and home input work is still pending; see motor-bringup.md.

2026-11-23: standalone firmware/motor_jog adds a C stepping controller and Arduino C++ bridge; target build and native controller checks pass. Motor hardware verification remains pending. VSCodium CLI/tasks instructions are in vscodium.md. No home switch is installed; D6 is unused.
