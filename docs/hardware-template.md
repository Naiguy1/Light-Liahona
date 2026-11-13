# Liahona Hardware Worksheet

This worksheet captures the hardware information needed before firmware starts configuring pins. Fill in values from the actual board, wiring, and component markings. Do not infer GPIO assignments from the order of the parts.

## Confirmed components

| Subsystem | Hardware recorded |
| --- | --- |
| Controller | 38-pin ESP32 development board, ESP32-WROOM-32 |
| Light sensing | Four photoresistors in E / N / W / S order |
| Home reference | Home switch |
| Motion | 28BYJ-48 stepper with ULN2003 driver board |

## Pin and wiring map

Record the physical ESP32 GPIO number and the corresponding board-header label only after verifying the connection. Note any voltage divider, pull-up/pull-down, or supply details.

| Signal | Board/header label | ESP32 GPIO | Wiring / electrical notes | Verified |
| --- | --- | --- | --- | --- |
| East photoresistor output | TBD | TBD | TBD | No |
| North photoresistor output | TBD | TBD | TBD | No |
| West photoresistor output | TBD | TBD | TBD | No |
| South photoresistor output | TBD | TBD | TBD | No |
| Home switch | TBD | TBD | Switch type and idle/active levels TBD | No |
| ULN2003 IN1 | TBD | TBD | TBD | No |
| ULN2003 IN2 | TBD | TBD | TBD | No |
| ULN2003 IN3 | TBD | TBD | TBD | No |
| ULN2003 IN4 | TBD | TBD | TBD | No |

## Power and electrical checks

- ESP32 board supply method: TBD
- Stepper / ULN2003 supply and voltage: TBD
- Ground shared between ESP32 and driver: TBD
- Photoresistor divider topology and resistor values: TBD
- Confirm that every signal connected to an ESP32 input stays within the ESP32's allowed input voltage.
- Confirm which ESP32 pins are suitable for the intended ADC or digital function before assigning them.
- Record the exact board revision and any pin restrictions from its documentation.

## Bring-up notes

| Date | Change / observation | Build result | Serial output | Hardware behavior |
| --- | --- | --- | --- | --- |
| TBD | TBD | TBD | TBD | TBD |

## Firmware development checklist

- [ ] Verify board model and ESP-IDF target.
- [ ] Complete and verify the pin/wiring map.
- [ ] Add and validate one peripheral at a time.
- [ ] For each change, record build/flash commands and the expected serial and hardware checks.
