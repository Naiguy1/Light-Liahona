# Manual zero and angle positioning

**Current operation:** see [README](../README.md): `sh run.sh` uploads and opens the monitor; `z` sets East, `t` starts tracking, `s` stops, optional `c` saves calibration. The milestone-specific instructions below are historical; current implementation/checks are in [development.md](development.md).


This milestone extends the existing `firmware/motor_jog` sketch. It uses no sensor tracking and no home switch. All wiring remains D2/D3/D4/D5 → intact ULN2003 IN1/IN2/IN3/IN4; D6 is unused.

## Measured calibration

On 2026-11-23 the user reported smooth jogs and stop/release, `f` counterclockwise and `r` clockwise viewed from the shaft end. Thirty-two 128-half-step jogs completed one output-shaft revolution, giving a working calibration of **4096 half-steps/revolution** and **11.25 degrees/jog**. This is a user-measured working value, not a precision gearbox specification. Backlash and missed steps can affect physical accuracy.

Angles increase counterclockwise from an operator-chosen East reference: E=0°, N=90°, W=180°, S=270°. Verify that the sensor positions follow that order from the same viewing side before any future tracking integration. No compass or absolute heading is measured.

## Build and upload on the current Linux laptop

Close the serial monitor with Ctrl+C before uploading. From the repository root, use the project-local CLI already installed in `bin/`:

```sh
./bin/arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old --build-path build/motor_jog firmware/motor_jog
./bin/arduino-cli upload --fqbn arduino:avr:nano:cpu=atmega328old --port /dev/ttyUSB0 --input-dir build/motor_jog firmware/motor_jog
./bin/arduino-cli monitor --port /dev/ttyUSB0 --config baudrate=115200
```

Recheck `./bin/arduino-cli board list` if the port changes. Linux VSCodium tasks now use `${workspaceFolder}/bin/arduino-cli`; other platforms use `arduino-cli` on PATH. The sketch path and existing task names remain unchanged. If an editor sandbox cannot see the port, use the regular laptop terminal as in the successful jog test.

## Serial protocol

Send **one command per line**, pressing Enter. CR, LF and CRLF are supported. Lowercase `s` is also acted on immediately, without waiting for Enter, and discards the rest of its line. Commands are case-sensitive; numeric angles are whole decimal degrees, without spaces or signs. A partial numeric line never moves the motor. Overlong or malformed lines are rejected in full.

| Command | Behavior |
| --- | --- |
| `z` | While idle, records the current physical reference as East / 0° without moving |
| `0` or `360` | Go to East; 360 is an alias for zero, not an extra revolution |
| `90` | Go to North |
| `180` | Go to West |
| `270` | Go to South |
| Any integer `0`–`360` | Go to that absolute angle relative to manual zero |
| `f` | One 128-half-step counterclockwise jog, even before zeroing |
| `r` | One 128-half-step clockwise jog, even before zeroing |
| `p` | Report the estimated position or explain that zero is required |
| `l` | Read E/N/W/S and print the calculated light angle; no motor movement or zero required |
| `s` | Stop/release immediately and clear the zero reference |
| `?` | Help |

A numeric move requires `z` after startup/reset. Busy commands, including `z`, are rejected rather than queued; only `s` is accepted during a move. Numbers outside the range and other invalid lines do not initiate movement. Successful `f`/`r` jogs after zero update the estimate. A normal completion releases all coils while preserving the estimate. Stop or timeout clears zero and requires physical realignment and another `z` before angle commands.

The code chooses the shortest angular route, at most 2048 half-steps. An exact half-turn chooses counterclockwise. Targets round to the nearest half-step using 32-bit intermediate arithmetic. At the user-verified 2 ms per half-step, a quarter-turn is about 2.05 seconds and a half-turn about 4.1 seconds. The elapsed-time bound scales with the requested move, plus margin; it is a software guard, not a hardware emergency stop or watchdog.

## Unloaded acceptance check

The assembled pointer's mechanical travel and wire clearance have **not** been established. Keep the output shaft detached from the linkage for these tests. Shortest-path movement assumes unrestricted rotation on this unloaded bench motor; do not attach an unknown mechanism or start tracking yet.

1. Upload/open the monitor. Expect `Liahona manual position ready; coils OFF; zero required`. The motor must stay still.
2. Send `90`. Expect `Position unknown; align with East, then send z + Enter.` No movement.
3. Use a shaft flag/reference mark. Choose its current orientation to represent East (or align it using the existing jogs). Do not force the geared shaft by hand. Send `z`. Expect `Zero set: East 0.0 deg; coils OFF`, with no movement.
4. Send `90`, wait for completion, then `180`, `270`, and `0`, waiting between each. Each should advance about one quarter-turn CCW and finish with `Move complete; coils OFF` plus the estimated angle. The final flag should return close to its mark. No automatic queued motion should follow.
5. From zero, send `270`. Expect `Target 270 deg; CW; 1024 half-steps` and about one quarter-turn clockwise, not three quarters counterclockwise.
6. During a move send `s`. Expect `Stopped; coils OFF; zero cleared`. A following angle must be rejected until you realign and send `z` again. Test reset similarly: outputs stay off and position is unknown.
7. Send `361`, `-90`, `90x` or an overlong number while idle. Expect an invalid-command message and no movement.

All printed positions are **open-loop estimates**, not feedback. The motor releases its coils after each move and does not actively hold position. If the shaft is disturbed, the mechanism slips, the motor stalls or steps are missed, realign physically and re-zero. Do not use flash/EEPROM to restore an assumed position after reboot. A close final flag position under no load does not validate assembled clearance.

USB-fed 5V powers the Nano, sensors and motor driver in the user-reported bench arrangement. The 470 µF capacitor is across driver supply with striped negative to GND; batteries/boost stay disconnected. Stop on resets, disconnects or heating. Measured current capacity and loaded operation remain unverified.

## Source and checks

- `jog.c` / `jog.h`: C step controller, calibrated wraparound position, zero and shortest-path target planning.
- `commands.c` / `commands.h`: bounded C serial parser.
- `arduino_bridge.cpp`: Arduino I/O, command dispatch and status output.
- `tests/jog_test.c`: original bounded jog regression checks.
- `tests/position_test.c`: all integer angles, cardinal/wrap paths, rounding, zero requirement, stop/timeout, clock wrap and malformed serial input.

From the root on a machine with a C compiler:

```sh
mkdir -p build
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/motor_jog tests/jog_test.c firmware/motor_jog/jog.c -o build/jog_test
./build/jog_test
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/motor_jog tests/position_test.c firmware/motor_jog/jog.c firmware/motor_jog/commands.c -o build/position_test
./build/position_test
```

Build validation: Arduino CLI 1.6.0 / AVR core 1.8.6 / Nano old-bootloader target passed (4586 bytes flash, 227 bytes static SRAM). Original jog and new position/parser tests passed; address/undefined-behavior checks also passed with leak detection disabled because the test container uses ptrace. No heap allocation is used. This new positioning firmware has not been uploaded or hardware verified by the assistant.

## Light angle (simple diagnostic)

Calibration endpoints and manual positioning were confirmed by the user; no recalibration step is needed. At idle, send `l` + Enter to read the four existing A0–A3 channels and print `E:... N:... W:... S:... Light angle: ... deg CCW from East`. This command never moves the motor and works before `z`. Other manual commands are unchanged.

The C calculation in `light.c` clamps the verified endpoint normalization to 0–1000, computes x=East−West and y=North−South, then uses atan2(y,x). It prints a whole angle (East=0, North=90, West=180, South=270). Exactly balanced differences report `Light balanced; no direction`. This is a diagnostic angle, without filtering, confidence scoring or automatic movement. It does not establish a reliable direction under near-uniform illumination.

Build/upload commands above are unchanged. With a dominant light at one sensor, send `l` and check that the printed angle corresponds to that direction; the motor should stay still. Calibration values remain E 165/967, N 155/956, W 165/971, S 75/932. Only the sensor-to-angle calculation is new; no new pins are assigned.

Latest validation: Arduino CLI 1.6.0, AVR core 1.8.6, old-bootloader Nano target builds (6696 bytes flash, 251 bytes static SRAM). Jog and position/parser regression tests pass at 2 ms. New light-angle tests cover cardinal/diagonal inputs, clamping and balanced inputs; run `cc -std=c11 -Wall -Wextra -Werror -Ifirmware/motor_jog tests/light_test.c firmware/motor_jog/light.c -lm -o build/light_test` then `./build/light_test`. New light-angle output is pending a hardware check.
