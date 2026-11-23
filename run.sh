#!/bin/sh
# Build, upload, then open the monitor. Run from any directory: sh /path/to/run.sh [port]
set -eu
root=$(CDPATH= cd "$(dirname "$0")" && pwd)
cd "$root"
if [ -x "$root/bin/arduino-cli" ]; then
    cli="$root/bin/arduino-cli"
else
    cli=$(command -v arduino-cli) || { printf '%s\n' 'Arduino CLI not found. Install it in bin/ or add it to PATH.' >&2; exit 1; }
fi
if [ "$#" -gt 1 ]; then printf '%s\n' 'Usage: sh run.sh [serial-port]' >&2; exit 1; fi
port=${1:-}
if [ -z "$port" ]; then
    for device in /dev/ttyUSB* /dev/ttyACM*; do
        [ -c "$device" ] || continue
        if [ -n "$port" ]; then printf '%s\n' 'Multiple ports found. Run: sh run.sh /dev/ttyUSB0 (choose your Nano port).' >&2; exit 1; fi
        port=$device
    done
fi
if [ -z "$port" ]; then printf '%s\n' 'No USB serial port found. Connect the Nano with a data cable, or pass its port: sh run.sh PORT' >&2; exit 1; fi
board=arduino:avr:nano:cpu=atmega328old
"$cli" compile --fqbn "$board" --build-path build/motor_jog firmware/motor_jog
"$cli" upload --fqbn "$board" --port "$port" --input-dir build/motor_jog firmware/motor_jog
exec "$cli" monitor --port "$port" --config baudrate=115200
