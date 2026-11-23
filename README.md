# Light Liahona

Arduino Nano (ATmega328P, old bootloader), four light sensors and a 28BYJ-48 motor through an intact ULN2003 driver. The firmware includes the verified sensor calibration, 4096 half-steps/revolution and 2 ms stepping. You do not need to edit source files.

## Flash and connect

From this project folder on your Linux laptop, close any existing serial monitor with **Ctrl+C**, then run:

```sh
sh run.sh
```

This builds, uploads and opens the serial monitor at 115200 baud. It uses `bin/arduino-cli` if installed there, otherwise the CLI on PATH. With one USB serial device connected, it selects the port automatically. If multiple devices are connected, specify the Nano: `sh run.sh /dev/ttyUSB0`.

In VSCodium the same action is **Ctrl+Shift+B**, or **Tasks: Run Task → Liahona: Upload and monitor**.

One-time setup, if needed: [Arduino CLI](https://docs.arduino.cc/arduino-cli/installation/), then `./bin/arduino-cli core update-index` and `./bin/arduino-cli core install arduino:avr@1.8.6`. No extra libraries are needed; EEPROM is included with the AVR core.

## Use it

Keep the shaft unloaded for the first tracking test; the printed mechanism's travel clearance is not yet established.

1. Align the shaft reference/pointer with the East sensor (use the existing jog commands if needed; do not force the geared shaft).
2. Type **`z`**, Enter: record East zero without moving.
3. Type **`t`**, Enter: start following the light.
4. Type **`s`** to stop. This also clears zero, so use `z` again before restarting.

No motion starts automatically on boot. Startup prints `Liahona ready; coils OFF; zero required` and whether it loaded built-in or saved calibration. Tracking prints `Tracking ON; s stops`, then `Tracking target: ... deg` when it moves. It checks the light every 250 ms while idle, uses the shortest route, and ignores changes of 5 degrees or less. Weak/balanced directional readings hold the current position; they do not return the pointer to East. Coils release after a completed move.

## Optional calibration later — no source edits

Your verified defaults are already built in. After mounting the sensors, you can optionally type **`c`**, Enter, while idle:

- For 30 seconds, cover each sensor and then illuminate each using the same light distance.
- The motor stays off. Each sensor must see at least a 100-count raw range.
- Valid calibration is saved in EEPROM for future power-ups. Invalid/cancelled calibration keeps the prior settings. **`s`** cancels.
- Watch for `Calibration saved`. Afterward align with East, then use `z` and `t`.

Calibration is stored; physical position and tracking mode are not. After power-up/reset, manual realignment and zero are still required. No home switch is installed.

## Other commands

| Command + Enter | Action |
| --- | --- |
| `l` (lowercase L) | Print sensor readings and calculated direction |
| `p` | Print estimated pointer position |
| `f` / `r` | Counterclockwise / clockwise jog, 128 half-steps |
| `0`–`360` | Move to an angle after zeroing; E=0, N=90, W=180, S=270 |
| `?` | Help |

Send one command at a time and wait for a move to complete. During a move, only `s` is accepted. `s` acts immediately without Enter; press Enter afterward to clear its discarded line. Manual jog, zero and angle commands turn tracking off. Any calibration capture also turns tracking off. A movement timeout stops and clears zero.

## Hardware and limits

Sensors E/N/W/S use A0/A1/A2/A3. Driver IN1–IN4 use D2–D5. D6 is unused. See [HARDWARE.md](HARDWARE.md) for wiring/power. Sensors must have distinct views in E/N/W/S order to estimate direction; the upward-facing breadboard arrangement does not yet give reliable physical bearings. Calibration cannot remove shared light exposure. Final direction tuning and loaded operation wait for the housing.

The current user-reported bench supply is Nano USB-fed 5V to sensors and driver VCC, common GND, and a 470 µF supply capacitor; boost/batteries are disconnected. Current capacity is unverified. Stop if the board resets, USB disconnects or anything overheats. Position is estimated from commanded steps, not measured feedback. If it stalls or is moved, realign and zero again. Do not use continuous shortest-path tracking with a mechanism or wiring that cannot rotate freely.

## Project files

- `firmware/motor_jog/`: current complete firmware (C logic with an Arduino C++ bridge; the folder name is retained for compatibility).
- `firmware/liahona/`: earlier standalone sensor diagnostic, not required for normal use.
- `tests/`: native controller, calibration and application tests.
- [Current development checks](docs/development.md), [hardware](HARDWARE.md), [measured calibration](docs/sensor-calibration.md).
- Earlier milestone notes: [manual position](docs/manual-position.md), [VSCodium setup](docs/vscodium.md). The instructions above supersede older command/task lists.

The root ESP-IDF CMake scaffolding is historical, not the Nano build system.
