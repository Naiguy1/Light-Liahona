# Liahona firmware

An offline, battery-powered pointer that turns toward a dominant horizontal light direction. The controller design has migrated to a classic 5 V Arduino Nano-style board with USB-C. The user approved the Nano pin assignments on 2026-11-20. The supplied chip photographs confirm ATmega328P (ATMEGA328P-AU). User confirmed uploads work with ATmega328P (Old Bootloader).

## Project status

User reports successful heartbeat and four-channel raw sensor tests using **ATmega328P (Old Bootloader)**. Current Arduino sketch: [firmware/liahona/liahona.ino](firmware/liahona/liahona.ino), which adds preliminary endpoint normalization only. This new sketch still needs a build/upload and hardware check. Motor power remains disconnected.

The root CMake files and main/main.c are historical ESP32 scaffolding, not the Nano project. Open the Arduino sketch directly.

## Documentation

- [HARDWARE.md](HARDWARE.md): approved pin map, sensors, power and parts.
- [AI_WORKFLOW.md](AI_WORKFLOW.md): development rules and milestones.
- [Nano wiring reference](docs/nano-wiring.md): connection-by-connection wiring.
- [Hardware verification worksheet](docs/hardware-template.md): measured wiring and observations.
- [Migration notes](docs/nano-migration.md): firmware, tooling and enclosure changes still required.

## Build, upload and check

Install Arduino AVR Boards in Arduino IDE. Open `firmware/liahona/liahona.ino` (folder and sketch names match). Select Arduino Nano → **ATmega328P (Old Bootloader)** → current COM port. Close Serial Monitor before Verify / Upload, then reopen it at **115200 baud**.

Expect the startup line `Liahona normalized sensor test; motor power disconnected` and labeled E/N/W/S raw values and percentages every 250 ms. No motor or switch code is enabled. IDE/core versions remain TBD.

- [Measured endpoints and sensor checks](docs/sensor-calibration.md)
- [Motor bring-up milestones](docs/motor-bringup.md)

`idf.py` is only for the historical ESP32 scaffold and must not be used for the Nano.

## References

- [Classic Nano hardware documentation](https://docs.arduino.cc/hardware/nano)
- [Official Nano pinout](https://docs.arduino.cc/resources/pinouts/A000005-full-pinout.pdf)
- [Arduino IDE](https://docs.arduino.cc/software/ide/)
