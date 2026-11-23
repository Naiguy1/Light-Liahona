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

## Position reference

No home switch is installed. Leave D6 unused. A main power switch is not a position sensor.

## Motor and power

| From | To |
| --- | --- |
| Nano D2 | ULN2003 IN1 |
| Nano D3 | ULN2003 IN2 |
| Nano D4 | ULN2003 IN3 |
| Nano D5 | ULN2003 IN4 |
| Nano 5V rail (current bench test) | ULN2003 board power + |
| Nano GND / breadboard ground rail | ULN2003 board power − and Nano GND |
| Motor five-wire plug | ULN2003 motor socket |
| 470 µF capacitor + / − | Driver supply +5V / GND |
| 100 nF ceramic | Driver supply +5V to GND |

Current Nano power is USB and the driver shares that 5V rail; its current capacity is unverified. Future two-AA boosted supply integration requires load testing and USB/external-power isolation review for the actual clone. Never supply coil current through a GPIO; the current USB-fed rail arrangement is not approval to drive the motor from the onboard VIN regulator. The previous ESP32 wiring image is superseded; do not use it for Nano wiring.

Actual sensor resistors: E/N/W = 10 kΩ each; South = 6.5 kΩ. Earlier four-identical-resistor plan is superseded. No home switch is installed; D6 remains unused.

## Current motor bench test (2026-11-23)

This supersedes the separate-motor-supply arrangement above for the current test: intact spare driver; Nano 5V powers the positive breadboard rail, sensors and driver VCC; Nano/driver/sensor grounds are common. 470 µF capacitor across supply, positive to +5V. Boost and batteries disconnected. Available USB current is unverified. D2–D5 → IN1–IN4; D6 unused because no home switch is installed. See [VSCodium motor instructions](vscodium.md) before applying motor commands.
