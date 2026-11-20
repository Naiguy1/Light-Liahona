# Liahona firmware

An offline, battery-powered pointer that turns toward a dominant horizontal light direction. The controller design has migrated to a classic 5 V Arduino Nano-style board with USB-C. The user approved the Nano pin assignments on 2026-11-20. The supplied chip photographs confirm ATmega328P (ATMEGA328P-AU). Bootloader selection remains to be confirmed by upload.

## Project status

This change updates documentation only. The checked-in `main/main.c` and CMake files are the original ESP32 / ESP-IDF serial startup scaffold. They are not Nano firmware. There is no Arduino sketch yet, and no Nano build, upload, sensor reading, or motor behavior has been demonstrated. Wiring was reported verified by the user on 2026-11-20.

The next firmware change should add only a Nano serial heartbeat. Subsequent changes bring up one subsystem at a time. Approved assignments authorize implementation; they do not certify physical wiring.

## Documentation

- [HARDWARE.md](HARDWARE.md): approved pin map, sensors, power and parts.
- [AI_WORKFLOW.md](AI_WORKFLOW.md): development rules and milestones.
- [Nano wiring reference](docs/nano-wiring.md): connection-by-connection wiring.
- [Hardware verification worksheet](docs/hardware-template.md): measured wiring and observations.
- [Migration notes](docs/nano-migration.md): firmware, tooling and enclosure changes still required.

## Planned Arduino development setup

1. The supplied photographs confirm ATmega328P on the purchased board. Confirm bootloader selection when uploading. A Nano-shaped clone is not sufficient to identify its processor or bootloader.
2. For the confirmed ATmega328P, install Arduino IDE and the Arduino AVR Boards package. Record versions here when installed: IDE **TBD**, AVR core **TBD**.
3. Select Arduino Nano, the processor/bootloader matching the board, and its actual serial port. The USB-C connector does not identify the USB serial chip or guarantee that a driver is required.
4. Once an Arduino sketch exists, open it, select Verify, then Upload. Open Serial Monitor at the baud rate specified by that sketch.

These are planned steps: there is currently no Nano sketch to build or flash. `idf.py` builds only the legacy ESP32 scaffold and must not be used for the Nano.

## First acceptance check

The planned heartbeat sketch should print `Liahona Nano ready` once, then a periodic heartbeat. These messages are acceptance criteria, not current output. Keep the motor disconnected. Confirm reliable startup before adding the East sensor on A0.

## References

- [Classic Nano hardware documentation](https://docs.arduino.cc/hardware/nano)
- [Official Nano pinout](https://docs.arduino.cc/resources/pinouts/A000005-full-pinout.pdf)
- [Arduino IDE](https://docs.arduino.cc/software/ide/)
