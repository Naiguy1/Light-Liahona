# Liahona ESP-IDF Firmware

Starter firmware for the Liahona light-seeking device. This scaffold only starts the ESP32 and prints a serial message. It does not configure GPIO, read sensors, operate the home switch, or drive the stepper.

## Hardware recorded so far

- 38-pin ESP32 development board (ESP32-WROOM-32)
- Four photoresistors, arranged E / N / W / S
- Home switch
- 28BYJ-48 stepper motor with ULN2003 driver board

GPIO assignments and wiring details are intentionally left open. See [docs/hardware-template.md](docs/hardware-template.md) and fill it from the actual wiring before adding hardware code.

## Development setup

1. Install ESP-IDF for the classic ESP32 target and open a terminal where the ESP-IDF environment is available.
2. From this repository root, select the target once:

   ```sh
   idf.py set-target esp32
   ```

3. Build:

   ```sh
   idf.py build
   ```

4. Flash and open the serial monitor, replacing `PORT` with the board's serial port:

   ```sh
   idf.py -p PORT flash monitor
   ```

   Examples: `COM5` on Windows or `/dev/ttyUSB0` on Linux. The port name depends on the USB-to-serial adapter and operating system.

To leave the serial monitor, press `Ctrl+` then `]`.

## Expected startup output

After the ESP-IDF boot messages, the application should print a line similar to:

```text
I (..) main: Liahona firmware scaffold ready; no hardware outputs configured.
```

The timestamp varies. The motor should remain still, and no sensor or switch pins should be configured by this starter.

## Development sequence

Make one hardware-facing change at a time. Before enabling a peripheral, record its pin and wiring in the hardware worksheet. Build and flash after each change, then check the serial output and the specific hardware behavior described for that change. Keep pin choices grounded in the assembled wiring and ESP32 board documentation.
