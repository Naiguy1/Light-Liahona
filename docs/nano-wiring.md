# Nano wiring reference

Current approved design, 2026-11-20. Use [HARDWARE.md](../HARDWARE.md) for power restrictions and part details. The user reported wiring verified on 2026-11-20; this is user-reported verification, not an independent electrical inspection. Disconnect power while changing wiring.

## Each sensor (repeat four times)

| Component terminal | Connect to |
| --- | --- |
| Photoresistor leg 1 | Nano 5V sensor rail |
| Photoresistor leg 2 | Its own signal junction |
| Fixed resistor end 1 | Same signal junction |
| Fixed resistor end 2 | Common GND |
| Optional 100 nF filter | Between that junction and GND |

| Signal junction | Nano input |
| --- | --- |
| East | A0 |
| North | A1 |
| West | A2 |
| South | A3 |

All four dividers share supply and ground, but their signal junctions remain separate.

## Home switch

Nano 5V → 10 kΩ pull-up → D6 junction → switch COM. Switch NO → GND. Released is HIGH; pressed is LOW. Verify switch terminals electrically rather than inferring them from location.

## Motor and power

| From | To |
| --- | --- |
| Nano D2 | ULN2003 IN1 |
| Nano D3 | ULN2003 IN2 |
| Nano D4 | ULN2003 IN3 |
| Nano D5 | ULN2003 IN4 |
| Verified motor supply +5V | ULN2003 board power + |
| Motor supply GND | ULN2003 board power − and Nano GND |
| Motor five-wire plug | ULN2003 motor socket |
| 470 µF capacitor + / − | Driver supply +5V / GND |
| 100 nF ceramic | Driver supply +5V to GND |

Initial Nano power is USB. Future two-AA boosted supply integration requires load testing and USB/external-power isolation review for the actual clone. The Nano must not supply coil current through GPIO or its onboard regulator. The previous ESP32 wiring image is superseded; do not use it for Nano wiring.

Actual sensor resistors: E/N/W = 10 kΩ each; South = 6.5 kΩ. Earlier four-identical-resistor plan is superseded. No spare external 10 kΩ home pull-up is confirmed; resolve using D6 INPUT_PULLUP in the home-switch milestone and record the installed arrangement.
