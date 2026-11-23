# Sensor calibration — measured 2026-11-20, user verified 2026-11-23

User-reported measurements on the classic Nano. Each bright measurement used a phone flashlight about 2 cm away. Covered measurements are the current covered baseline, not a calibrated zero-lux reference.

| Direction | Input | Resistor | Covered range | Bright range | Endpoint midpoints |
| --- | --- | --- | --- | --- | --- |
| East | A0 | 10 kΩ | 160–170 | 966–968 | 165 / 967 |
| North | A1 | 10 kΩ | 150–160 | 955–957 | 155 / 956 |
| West | A2 | 10 kΩ | 160–170 | 970–972 | 165 / 971 |
| South | A3 | 6.5 kΩ | 70–80 | 931–932 | 75 / 932 |

Indoor baseline ranges: E 628–663, N 622–657, W 614–643, S 501–538. Heartbeat and raw sensor uploads worked with ATmega328P (Old Bootloader). The USB device reports FTDI VID 0403 / PID 6001. IDE/core versions remain unreported.

The sketch computes (raw - covered) * 100 / (bright - covered), clamps to 0–100%, and prints one decimal place. It uses 32-bit intermediate arithmetic to avoid AVR integer overflow. This is approximate endpoint normalization, not lux measurement or proof of matched response throughout the range. The verified endpoints are retained for this build.

Open firmware/liahona/liahona.ino in Arduino IDE. Select Arduino Nano, ATmega328P (Old Bootloader), and the current COM port. Close Serial Monitor, Verify, Upload, then reopen at 115200 baud. Motor power stays disconnected: the sketch does not actively drive the motor pins LOW.

Every 250 ms expect E:raw/percent followed by N, W and S. Cover one sensor: its percentage should approach 0%; illuminate near the reference conditions: it should approach 100%. Under equal illumination compare all four and record residual mismatch. Calibration has been confirmed by the user; no further calibration is required for this milestone. No direction, filtering or motion code is included.

2026-11-23: user confirms calibration verified. Retain these endpoints. The manual motor-control sketch now reuses them in a simple `l` diagnostic angle readout, with no motor movement; see manual-position.md.
