# Current firmware development

Use the current README for normal operation: `sh run.sh`, `z`, `t`, `s`, optional `c`. Defaults reuse the verified E/N/W/S endpoints (165/967, 155/956, 165/971, 75/932), 4096 half-steps/revolution and 2 ms stepping. No code changes are needed after mounting to capture new sensor ranges.

## Files and behavior

- `light.c/h`: endpoint normalization, angle and 10% minimum directional-contrast gate; calibration range validation and CRC16.
- `arduino_bridge.cpp`: simple 250 ms idle tracking loop with a 5-degree deadband, a nonblocking 30-second calibration capture and EEPROM reads/writes. Only a successful explicit capture writes calibration; the loop does not continuously write EEPROM. All candidate spans must be at least 100 and all ADC endpoints within 0..1023.
- `commands.c/h`: extends the existing parser with `t` and `c`.
- `jog.c/h`: existing bounded open-loop position/motion controller, unchanged in this milestone.
- `run.sh`, `.vscode/tasks.json`: one build/upload/monitor action with old-bootloader target. No automatic device-permission changes or forced Git changes.

On boot the bridge reads a tagged, checksummed calibration record from EEPROM address 0 (20 bytes on ATmega328P). Invalid/tagless/corrupted data uses built-in defaults. A save invalidates the tag before writing and commits the tag last, then verifies the readback. Interrupted writes may lose the previously saved profile but cannot intentionally publish a half-written profile; next boot falls back to defaults if validation fails. Calibration is retained across normal power cycles. Chip erase or deliberately overwriting EEPROM can remove it. Position and tracking mode are never restored from EEPROM.

Calibration collection disables tracking and never drives the motor. It samples every 20 ms and reports progress every 5 s. Invalid capture leaves both live and saved profiles unchanged. Stop cancels capture without saving, disables tracking and clears zero. Numeric/jog/zero commands disable tracking when accepted at idle. Movement commands are ignored during capture; while moving only stop is accepted. A normal move finishes before another tracking target is sampled. Low directional contrast prevents a new move; it does not interrupt one already underway.

## Build without uploading

```sh
./bin/arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old --build-path build/motor_jog firmware/motor_jog
```

`sh run.sh /dev/ttyUSB0` builds, uploads, then runs the monitor. Close other monitors first. The script stops on build/upload failure and refuses to guess if more than one USB serial port is found.

## Native tests

```sh
mkdir -p build/native
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/motor_jog tests/jog_test.c firmware/motor_jog/jog.c -o build/native/jog_test
build/native/jog_test
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/motor_jog tests/position_test.c firmware/motor_jog/jog.c firmware/motor_jog/commands.c -o build/native/position_test
build/native/position_test
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/motor_jog tests/light_test.c firmware/motor_jog/light.c -lm -o build/native/light_test
build/native/light_test
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/motor_jog -c firmware/motor_jog/jog.c -o build/native/jog.o
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/motor_jog -c firmware/motor_jog/commands.c -o build/native/commands.o
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/motor_jog -c firmware/motor_jog/light.c -o build/native/light.o
g++ -std=c++11 -Wall -Wextra -Werror -Itests/fakes -Ifirmware/motor_jog tests/application_test.cpp firmware/motor_jog/arduino_bridge.cpp build/native/jog.o build/native/commands.o build/native/light.o -lm -o build/native/application_test
build/native/application_test
python3 tests/run_script_test.py
```

Validation in this change: Arduino CLI 1.6.0 / AVR core 1.8.6 old-bootloader Nano build passes (8852 bytes flash; 361 bytes static SRAM). Native jog, position/parser, light/calibration and application tests pass. Shell syntax and runner failure-propagation checks pass. No physical upload, EEPROM write or automatic tracking test was performed by the assistant; earlier manual motor and sensor calibration checks are user verified.

## Hardware acceptance

Use an unloaded shaft. On startup expect no motion and `Calibration: built-in verified defaults` on a blank EEPROM. `t` before `z` must refuse to track. After alignment, `z` then `t` should move toward a dominant light. With the same light held still, changes within 5 degrees should not trigger another move. Stop with `s`, press Enter and confirm coils off and no automatic restart. Reset requires another alignment/zero and explicit tracking command.

Optional persistence check: type `c`; cover and illuminate every sensor during 30 s. Expect `Calibration saved`, then reset and check `Calibration: saved` with coils off. Repeat a capture without changing illumination: it should reject insufficient ranges and keep prior settings. Cancel with `s`: no save. These are calibration/storage checks, not a claim of reliable bearings with all sensors facing up. Final direction accuracy waits for final sensor orientation in the housing; no new source edits are required for calibration there.

EEPROM behavior reference: [Arduino EEPROM library](https://docs.arduino.cc/learn/built-in-libraries/eeprom/).
