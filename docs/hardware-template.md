# Nano hardware verification worksheet

The approved design is in [HARDWARE.md](../HARDWARE.md); this worksheet records measured assembly. Approval date: 2026-11-20. The user subsequently reported wiring verified on 2026-11-20. Numerical measurements and functional test results were not provided.

| Signal | Approved Nano pin | Actual wiring / measurement | Bench verified |
| --- | --- | --- | --- |
| East junction | A0 | TBD | User reported, 2026-11-20 |
| North junction | A1 | TBD | User reported, 2026-11-20 |
| West junction | A2 | TBD | User reported, 2026-11-20 |
| South junction | A3 | TBD | User reported, 2026-11-20 |
| ULN2003 IN1 | D2 | TBD | User reported, 2026-11-20 |
| ULN2003 IN2 | D3 | TBD | User reported, 2026-11-20 |
| ULN2003 IN3 | D4 | TBD | User reported, 2026-11-20 |
| ULN2003 IN4 | D5 | TBD | User reported, 2026-11-20 |
| Optional home / zero | D6 | No switch installed; unused | Not applicable |
| Sensor / pull-up rail | 5V | Measured voltage TBD | User reported, 2026-11-20 |
| Common return | GND | Continuity TBD | User reported, 2026-11-20 |

- Processor marking: ATMEGA328P-AU (photographs supplied 2026-11-20); board revision: TBD
- USB serial chip / bootloader: TBD
- Arduino IDE / AVR core versions: TBD
- Switch COM and NO verification: TBD
- Resistors and capacitor values / polarity: TBD
- Boost model, input range, measured output at motor load: TBD
- USB/external power selection method: TBD
- Nano mount / moving-part clearance: TBD

| Date | One change | Build/upload result | Serial output | Measured hardware behavior |
| --- | --- | --- | --- | --- |
| TBD | TBD | TBD | TBD | TBD |

Current motor bench arrangement (2026-11-23): intact spare ULN2003, D2–D5 → IN1–IN4, Nano USB-fed 5V supply and common GND. Motor jog results and supply capacity remain unverified. See vscodium.md.

2026-11-23 user report: smooth unloaded f/r jogs and stop/release; f=CCW, r=CW viewed from shaft end. 32 jogs ×128 half-steps per revolution; use 4096 as working calibration. Serial manual-zero/numeric-position tests and assembled clearance remain pending.
