# Motor bring-up

Current milestone: [manual zero and angle positioning](manual-position.md) in the existing motor_jog sketch. Unloaded jogs and direction are user verified; this position-control extension is pending verification. The intact spare ULN2003 uses D2 → IN1, D3 → IN2, D4 → IN3 and D5 → IN4. No home switch is installed; D6 remains unused. The on/off switch controls main power, not homing.

1. Build/upload the jog sketch; leave shaft unloaded and initialize with coils off.
2. Confirm no startup movement, short `f` and `r` jogs, stop/release with `s`, and no USB resets or abnormal heating. Record observed direction and movement; do not assume an exact gearbox ratio or shaft angle.
3. Test the serial manual reference: align with East, send z, then test cardinal angles unloaded. Explicit stop/timeout/reset requires re-zero. There is no automatic home search.
4. Measure usable travel and motion per revolution before attaching the mechanism. Add suitable limits and fault handling.
5. Integrate direction estimation/tracking only after motor and position-reference milestones pass.
6. Validate battery/boost operation later under measured combined load.

The current USB-fed Nano 5V rail powers both driver VCC and sensors. Common GND and a 470 µF supply capacitor are required by this bench wiring plan; available current remains unverified. Boost/batteries stay disconnected. The jog has software step/time bounds, but no physical position feedback or hardware emergency stop. See vscodium.md for exact commands, output and checks.

Calibration: user measured 32 × 128 half-steps/revolution, f=CCW and r=CW viewed from the shaft end. Numeric commands use the shortest angular route; assembled travel must be established before attaching the linkage.
