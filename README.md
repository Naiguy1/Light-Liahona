# Liahona firmware

An offline, battery-powered pointer that turns toward a dominant horizontal light direction. The controller design has migrated to a classic 5 V Arduino Nano-style board with USB-C. The user approved the Nano pin assignments on 2026-11-20. The supplied chip photographs confirm ATmega328P (ATMEGA328P-AU). User confirmed uploads work with ATmega328P (Old Bootloader).

## Project status

User reports successful heartbeat and four-channel raw sensor tests using **ATmega328P (Old Bootloader)**. Current Arduino sketch: [firmware/liahona/liahona.ino](firmware/liahona/liahona.ino), which adds preliminary endpoint normalization only. This new sketch still needs a build/upload and hardware check. Sensor calibration remains a separate sketch. A standalone motor-jog milestone is now available; see below.

The root CMake files and main/ component are historical ESP32 scaffolding, not the Nano project (the old main.c has since been removed). Open the Arduino sketch directly.

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

## Motor jog in VSCodium

Follow [docs/vscodium.md](docs/vscodium.md) for Arduino CLI installation, compile/upload commands and serial checks. Open `firmware/motor_jog/motor_jog.ino` for the separate motor milestone. Most stepping logic is C (`jog.c`); the Arduino bridge is C++ (`arduino_bridge.cpp`). No extra motor libraries are required.

On the intact spare ULN2003 board, D2–D5 connect to IN1–IN4. The user now powers driver VCC and the sensors from the Nano USB-fed 5V rail, with common GND and a 470 µF supply capacitor; boost stays disconnected. This arrangement's available current is not verified. No home switch is installed and D6 is unused. Each serial `f`/`r` command runs a bounded unloaded jog and releases the coils; `s` stops. There is no startup motion or automatic tracking. Hardware verification is pending.
