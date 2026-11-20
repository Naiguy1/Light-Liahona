# Liahona hardware — approved Nano design

Design approved by the user on 2026-11-20. Every assignment below is approved for implementation. On 2026-11-20 the user reported wiring verified. No firmware or motion test results have been supplied. Controller: classic 5 V Nano-style USB-C board, confirmed ATmega328P from the supplied chip photographs; bootloader selection remains to be established by upload. Do not apply this power plan to a Nano ESP32 or another 3.3 V Nano variant.

## Approved pin map

| Signal | Nano pin | Connection | Bench verified |
| --- | --- | --- | --- |
| East sensor junction | A0 | LDR / 10 kΩ divider junction | User reported, 2026-11-20 |
| North sensor junction | A1 | LDR / 10 kΩ divider junction | User reported, 2026-11-20 |
| West sensor junction | A2 | LDR / 10 kΩ divider junction | User reported, 2026-11-20 |
| South sensor junction | A3 | LDR / 10 kΩ divider junction | User reported, 2026-11-20 |
| ULN2003 IN1 | D2 | Driver input | User reported, 2026-11-20 |
| ULN2003 IN2 | D3 | Driver input | User reported, 2026-11-20 |
| ULN2003 IN3 | D4 | Driver input | User reported, 2026-11-20 |
| ULN2003 IN4 | D5 | Driver input | User reported, 2026-11-20 |
| Home switch | D6 | Normally-open contact to GND; active LOW | User reported, 2026-11-20 |
| Divider and external home pull-up supply | 5V | Nano regulated 5 V rail | User reported, 2026-11-20 |
| Common ground | GND | Sensors, switch, driver and supply return | User reported, 2026-11-20 |

Keep D0/D1 available for serial. Use printed pin names, not physical pin counts or inferred orientation.

## Light sensors and switch

For each direction: **5V → photoresistor → signal junction → 10 kΩ resistor → GND**. Connect that junction to the assigned analog input. More light is expected to raise the junction voltage with this topology; confirm experimentally. Photoresistors and resistors are nonpolarized.

The user reports a 10 kΩ resistor in breadboard row 42 connected to ground. This does not establish which end is the signal junction. Connect the sensor and analog wire to the resistor's ungrounded end. Five-hole groups on opposite sides of the breadboard trench are separate; check rail continuity across any split.

Home switch: D6 to switch COM, switch NO to GND. An external 10 kΩ resistor from D6 to 5V is the documented arrangement. If using internal `INPUT_PULLUP` instead, record that implementation choice; do not silently change the wiring record. Verify idle HIGH and pressed LOW before motion.

## Capacitors and available parts

- Four photoresistors; four 10 kΩ sensor resistors; one 10 kΩ home pull-up.
- Planned inventory: eight 10 kΩ resistors, two 100 kΩ resistors, one 470 µF capacitor rated at least 10 V, five 100 nF capacitors. Actual markings still need checking.
- Optional sensor filters: one 100 nF ceramic from each analog junction to GND, near the input. Omit during the first simple sensor wiring check if not available.
- Driver supply: 470 µF electrolytic and 100 nF ceramic in parallel across driver +5V and GND, near the driver. Electrolytic positive goes to +5V; its marked negative goes to GND. These do not replace any boost-module capacitors required by its manufacturer.
- 100 nF = 0.1 µF, not 100 µF. Do not substitute an unidentified electrolytic for a 100 nF filter. A polarized sensor capacitor, if specifically selected, would have positive at the signal node and negative at GND; leakage and suitability need checking.

## Power

Initial bring-up: Nano on USB; motor disconnected. For motor tests, use a separately verified 5 V motor supply with a common ground. Keep motor current out of Nano GPIO and its onboard regulator path; motor plugs into the ULN2003 board's five-wire socket.

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
