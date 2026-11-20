# Motor bring-up plan

These are planned, separate milestones, not implemented motor firmware. Approved mapping: D2 → IN1, D3 → IN2, D4 → IN3, D5 → IN4; home D6, active LOW.

1. Finish normalized sensor checks with motor power disconnected.
2. Add only home-switch logging and debounce. Use D6 INPUT_PULLUP if the spare external pull-up is unavailable; explicitly record this choice. Confirm released HIGH / pressed LOW before motion.
3. With power off, connect the 28BYJ-48 five-wire plug to the ULN2003 socket. Connect a verified regulated 5 V supply to driver power +/− and share driver GND with Nano GND. Nano stays on USB; leave driver +5 V separate from Nano 5V/VIN. Confirm driver labels and capacitor values/polarity. Use 470 µF (+ to +5V, − to GND) and 100 nF across the driver supply near the board. Verify supply voltage under load; boost capacity is unmeasured.
4. Add an explicit, short bounded jog command with all four outputs initialized LOW before energizing the driver. No automatic startup movement. Start unloaded, with pointer/linkage detached if practical. Record coil sequence, direction, smooth movement, stops and coil release. If it only buzzes, stop and check power, plug and phase sequence before increasing speed.
5. Measure motion per output revolution and available travel; do not assume an exact 2048/4096 step count or unrestricted rotation. Add bounded homing with maximum steps/time, switch debounce, release/backoff logic, stuck-switch handling and de-energize on fault. Test stop/fault behavior before attaching the mechanism.
6. Integrate tracking only after jog and homing pass. Check interference in sensor readings and resets during motion, then test the battery supply under combined load.

A future motor change must document its actual build/upload commands, serial prompts and acceptance checks. Current sensor firmware does not exercise the driver or switch.
