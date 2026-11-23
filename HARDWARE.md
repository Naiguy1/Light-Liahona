# Liahona hardware — approved Nano design

Design approved by the user on 2026-11-20. Every assignment below is approved for implementation. On 2026-11-20 the user reported wiring verified. Heartbeat and four-channel raw sensor results have been supplied; smooth unloaded motor jogs and stop/release are user verified. Controller: classic 5 V Nano-style USB-C board, confirmed ATmega328P from the supplied chip photographs; uploads confirmed using ATmega328P (Old Bootloader). Do not apply this power plan to a Nano ESP32 or another 3.3 V Nano variant.

## Approved pin map

| Signal | Nano pin | Connection | Bench verified |
| --- | --- | --- | --- |
| East sensor junction | A0 | LDR / 10 kΩ divider junction | User reported, 2026-11-20 |
| North sensor junction | A1 | LDR / 10 kΩ divider junction | User reported, 2026-11-20 |
| West sensor junction | A2 | LDR / 10 kΩ divider junction | User reported, 2026-11-20 |
| South sensor junction | A3 | LDR / 6.5 kΩ divider junction | User reported, 2026-11-20 |
| ULN2003 IN1 | D2 | Driver input | User reported, 2026-11-20 |
| ULN2003 IN2 | D3 | Driver input | User reported, 2026-11-20 |
| ULN2003 IN3 | D4 | Driver input | User reported, 2026-11-20 |
| ULN2003 IN4 | D5 | Driver input | User reported, 2026-11-20 |
| Optional home / zero input | D6 | Reserved; no switch installed | Not applicable |
| Divider and external home pull-up supply | 5V | Nano regulated 5 V rail | User reported, 2026-11-20 |
| Common ground | GND | Sensors, switch, driver and supply return | User reported, 2026-11-20 |

Keep D0/D1 available for serial. Use printed pin names, not physical pin counts or inferred orientation.

## Light sensors and switch

For each direction: **5V → photoresistor → signal junction → fixed resistor → GND**. Connect that junction to the assigned analog input. Use 10 kΩ for E/N/W and 6.5 kΩ for South. More light is expected to raise the junction voltage with this topology; confirm experimentally. Photoresistors and resistors are nonpolarized.

The user reports a 10 kΩ resistor in breadboard row 42 connected to ground. This does not establish which end is the signal junction. Connect the sensor and analog wire to the resistor's ungrounded end. Five-hole groups on opposite sides of the breadboard trench are separate; check rail continuity across any split.

No home switch is installed. D6 is reserved and must stay unused in the jog milestone. The ordinary on/off switch is for main power. A manual-zero button or an automatically actuated home switch may be added later as a separate milestone.

## Capacitors and available parts

- Installed sensor plan: three 10 kΩ resistors (E/N/W), one 6.5 kΩ (South). All three available 10 kΩ parts are allocated to sensors; no spare 10 kΩ home pull-up is confirmed.
- User-reported additional resistors: 5.6 kΩ, 3.3 kΩ and four 220 Ω. Capacitors: 100 nF and 470 µF; verify quantities, types and voltage ratings. Earlier inventory estimates are superseded.
- Optional sensor filters: one 100 nF ceramic from each analog junction to GND, near the input. Omit during the first simple sensor wiring check if not available.
- Driver supply: 470 µF electrolytic and 100 nF ceramic in parallel across driver +5V and GND, near the driver. Electrolytic positive goes to +5V; its marked negative goes to GND. These do not replace any boost-module capacitors required by its manufacturer.
- 100 nF = 0.1 µF, not 100 µF. Do not substitute an unidentified electrolytic for a 100 nF filter. A polarized sensor capacitor, if specifically selected, would have positive at the signal node and negative at GND; leakage and suitability need checking.

## Power

Current motor bench test: Nano on USB, its 5V rail feeding sensors and driver VCC; see the current arrangement below. A separately regulated motor supply with common ground is an alternative if USB power is inadequate. Keep motor current out of Nano GPIO and its onboard regulator path; motor plugs into the ULN2003 board's five-wire socket.

Planned battery arrangement: two AA cells in series → verified 5 V boost converter. For a confirmed classic Nano, regulated 5 V can feed the 5V pin; do not feed this 5 V into VIN. The final boost must support the combined measured Nano and motor load. Grounds of Nano, ULN2003 and boost output must be connected. Verify module input/output labels and whether its ground is common before connecting battery return.

Do not combine USB power and the external 5 V rail until the exact clone's power circuit and a suitable power-selection/isolation arrangement are established. No direct boosted 5 V connection to 3V3 or AREF. Battery life and boost load capability are unverified.

## Mechanical changes

Measure the actual Nano board, headers and USB-C connector. Revise the controller carrier, retention, cable clearance and USB access as needed; inspect clearance to the shaft, cam, switch and moving linkage. Preserve existing screw/hardware choices where practical. No CAD or printable parts have been revised for Nano yet.

## Superseded material

The ESP32 pin plan and `Liahona_Proposed_Wiring.png` are historical references, not current wiring instructions. The current wiring reference is [docs/nano-wiring.md](docs/nano-wiring.md). Never connect this design's 5 V sensor junctions to an ESP32 input.

## References

- [Official classic Nano documentation](https://docs.arduino.cc/hardware/nano)
- [Official Nano pinout](https://docs.arduino.cc/resources/pinouts/A000005-full-pinout.pdf)
- [Official Nano power specifications](https://store.arduino.cc/products/arduino-nano)

See [sensor-calibration.md](docs/sensor-calibration.md) for measured endpoints. Any future switch implementation must be recorded and tested in a separate milestone.

## Current bench arrangement — 2026-11-23 (supersedes earlier power/home assumptions)

- The damaged IN1 board is set aside; user obtained female jumper cables and is using the intact spare ULN2003. Original mapping D2/D3/D4/D5 → IN1/IN2/IN3/IN4 applies; no channel-5 reroute.
- Nano USB-fed 5V → breadboard positive rail → sensor dividers and driver VCC. Common GND joins Nano, sensor resistors and driver. The 470 µF capacitor is parallel across the driver supply, positive to +5V and striped negative to GND. Connect it near the driver supply wiring. Boost/battery supply stays disconnected. User reports assembly complete; smooth unloaded jogs and stop/release have been reported.
- USB, cable and clone-board current capacity are unverified. Stop testing on resets, USB disconnects or heating; use an adequately rated separate regulated motor supply if needed. When using a separate supply with Nano on USB, join grounds and keep the positive rails separate.
- No home switch is installed. D6 is reserved but unused. The ordinary switch is for main power; its installed placement is not confirmed. A manual-zero pushbutton or automatic home sensor is a future choice, not an existing feature.

## Motor calibration and manual reference — 2026-11-23

User reports `f` CCW and `r` CW viewed from the shaft end, and 32 × 128-half-step jogs per output revolution. Working calibration: 4096 half-steps/revolution. No hardware home switch or zero button is installed; manual zero is a serial `z` command at the East reference. D6 remains unused. The user verified manual zero and cardinal-angle positioning, including operation at a 2 ms interval; full assembled pointer travel and loaded behavior remain unknown. See [manual-position.md](docs/manual-position.md).

User confirmed the sensor calibration on 2026-11-23. Keep the recorded endpoints; no recalibration requested. The new diagnostic light-angle readout reuses A0–A3 and does not move the motor.

## General firmware

The current firmware uses the same pins, verified default endpoints and 2 ms speed. Optional serial c calibration saves sensor ranges in EEPROM without source edits; z/t/s provide manual zero, opt-in tracking and stop. No home switch is required or assumed. Final light-angle accuracy remains pending the housing, and automatic tracking/EEPROM behavior still require a hardware check. User-verified sensor and manual positioning results remain valid.
