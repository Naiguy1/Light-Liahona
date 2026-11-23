# VSCodium: motor control workflow

**Current firmware:** manual zero and angle commands extend this same sketch. Follow [manual-position.md](manual-position.md) for the current protocol, expected output and acceptance checks. The initial jog procedure below records the prior milestone.

VSCodium edits the source; Arduino CLI compiles, uploads and opens the serial port. No Arduino editor extension is required. The pure C stepping module is `firmware/motor_jog/jog.c`; `arduino_bridge.cpp` supplies Arduino pin/serial calls and setup()/loop(). The minimal `.ino` satisfies Arduino's sketch-folder naming convention.

## Install once on your laptop

Install [Arduino CLI](https://docs.arduino.cc/arduino-cli/installation/) and make `arduino-cli` available in your terminal PATH. Open a new VSCodium terminal after installation.

```sh
arduino-cli version
arduino-cli core update-index
arduino-cli core install arduino:avr@1.8.6
arduino-cli board list
```

The selected target is **arduino:avr:nano:cpu=atmega328old**, matching your successfully tested ATmega328P (Old Bootloader) setting. FTDI clones may list an unknown board next to a usable serial port; do not substitute another target just because discovery cannot identify it.

## Open and update the project

Open your existing Light-Liahona checkout in VSCodium. Commit or otherwise preserve any local work before updating it. With a clean checkout, run `git pull --ff-only` from its root. Do not paste the jog sketch into the sensor sketch: they are separate milestones.

## Build and upload from the repository root

Keep the motor unloaded, with no linkage attached. Upload with driver motor power disconnected where practical; the sketch initializes D2–D5 LOW once it starts. No movement is commanded on boot or when the serial monitor opens. D6 and sensor inputs are not configured by the jog sketch.

```sh
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old --build-path build/motor_jog firmware/motor_jog
arduino-cli upload --fqbn arduino:avr:nano:cpu=atmega328old --port PORT --input-dir build/motor_jog firmware/motor_jog
arduino-cli monitor --port PORT --config baudrate=115200
```

Replace **PORT** in both commands with the actual result from `board list`: Windows example `COM3`, Linux example `/dev/ttyUSB0`, macOS example `/dev/cu.usbserial-...`. The laptop's verified port is `/dev/ttyUSB0`; it may change after reconnecting. Compile first: `upload` does not compile. Use Ctrl+C to close the monitor before another upload. A busy/access-denied port usually means another monitor or IDE still has it open.

## VSCodium task menu

Once Arduino CLI and the AVR core are installed, use **Terminal → Run Task**:

- **Nano: Build motor jog** (also Ctrl+Shift+B).
- **Nano: Upload motor jog**: builds first, then prompts for your actual serial port.
- **Nano: Serial monitor**: prompts for the port and opens the 115200-baud terminal. Click the terminal and type a command; press Enter if needed. Close with Ctrl+C before uploading.

These tasks are checked into `.vscode/tasks.json` and use no editor extension. They build only the motor-jog sketch. On Linux, tasks use `${workspaceFolder}/bin/arduino-cli`, matching the laptop installation. Other platforms use `arduino-cli` on PATH. If the editor cannot access the serial device, use a regular host terminal.

## Serial commands and acceptance checks

Initial jog milestone startup (superseded by manual-position.md):

```text
Liahona motor jog ready; coils OFF
f = forward jog, r = reverse jog, s = stop/release, ? = help
128 half-steps per jog; direction and shaft angle uncalibrated.
```

Send one lowercase character (press Enter if your terminal buffers input):

| Command | Expected result |
| --- | --- |
| `f` | `Jog forward`, a small movement, then `Jog complete; coils OFF` |
| `r` | Same-sized command in the opposite sequence direction |
| `s` | `Stopped; coils OFF`; cancels an active jog |
| `?` | Help when idle |

Each jog issues 128 half-step phase updates spaced at least 5 ms apart. The last phase is held for an interval and then all outputs are LOW. This is roughly 0.65 seconds in normal operation, not a guaranteed shaft angle or physical position. Busy jog requests are ignored rather than queued by the controller. Send commands individually. The stop command is software, not an emergency stop; remove power if movement or heating is unexpected. A 1.5 s elapsed-time guard releases the coils when the loop is running; this is not a hardware watchdog.

Check that no driver channel LEDs are on at idle, the phase LEDs sequence during a jog, both directions move smoothly, and the LEDs go out afterward. A status LED may remain lit. If the motor buzzes without rotating, stop and check wiring, connector seating and supply before increasing speed. Stop for USB disconnects, repeated startup messages/resets, or heating. USB current capability and the clone power path remain unverified; the capacitor does not increase the supply's current capacity.

The user reports using an intact spare ULN2003 driver and female jumper cables. Wiring is D2/D3/D4/D5 → IN1/IN2/IN3/IN4. Nano USB-fed 5V feeds the breadboard positive rail, sensors and driver VCC; GND is common. 470 µF capacitor is across +5V/GND, striped lead to GND. Boost and battery supply stay disconnected in this test. There is no installed home switch; D6 is unused. The on/off switch is for main power, not a position reference.

Record actual movement direction, smoothness, stops, any resets, and CLI/core versions. Successful source tests or compilation do not establish hardware operation. After this milestone, choose and verify manual zero or an installed home input before automatic positioning. Sensor calibration, tracking and homing are separate changes.

## Native controller checks (optional on a machine with a C compiler)

Create `build/` first if it does not already exist.

```sh
cc -std=c11 -Wall -Wextra -Werror -Ifirmware/motor_jog tests/jog_test.c firmware/motor_jog/jog.c -o build/jog_test
```

Run the produced executable (`./build/jog_test` on Linux/macOS). These checks cover bounded moves, reverse stepping, stop, timeout release and clock wrap. They do not test physical GPIO or motor wiring.

## References

- [Arduino CLI setup](https://docs.arduino.cc/arduino-cli/getting-started/)
- [Compile](https://docs.arduino.cc/arduino-cli/commands-reference/arduino-cli_compile)
- [Upload](https://docs.arduino.cc/arduino-cli/commands-reference/arduino-cli_upload)
- [Serial monitor](https://docs.arduino.cc/arduino-cli/commands-reference/arduino-cli_monitor)

## Validation performed for this change

- Arduino CLI 1.6.0, Arduino AVR Boards 1.8.6, target arduino:avr:nano:cpu=atmega328old: compiled successfully. Flash 2806 / 30720 bytes; static SRAM 214 / 2048 bytes.
- Native C controller checks passed with warnings treated as errors.
- No physical upload or motor test was performed by the assistant.
