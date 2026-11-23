# Motor bring-up

Current milestone: [standalone motor jog in VSCodium](vscodium.md). The intact spare ULN2003 uses D2 → IN1, D3 → IN2, D4 → IN3 and D5 → IN4. No home switch is installed; D6 remains unused. The on/off switch controls main power, not homing.

1. Build/upload the jog sketch; leave shaft unloaded and initialize with coils off.
2. Confirm no startup movement, short `f` and `r` jogs, stop/release with `s`, and no USB resets or abnormal heating. Record observed direction and movement; do not assume an exact gearbox ratio or shaft angle.
3. Decide between a manual starting reference and an installed home switch. Verify any new switch without movement first. Never run an automatic search with no home sensor.
4. Measure usable travel and motion per revolution before attaching the mechanism. Add suitable limits and fault handling.
5. Integrate direction estimation/tracking only after motor and position-reference milestones pass.
6. Validate battery/boost operation later under measured combined load.

The current USB-fed Nano 5V rail powers both driver VCC and sensors. Common GND and a 470 µF supply capacitor are required by this bench wiring plan; available current remains unverified. Boost/batteries stay disconnected. The jog has software step/time bounds, but no physical position feedback or hardware emergency stop. See vscodium.md for exact commands, output and checks.
