#!/bin/bash


keyboard_device=$(grep -E "Handlers|EV=" /proc/bus/input/devices | \
grep -B1 "EV=120013" | \
grep -Eo "event[0-9]+" | \
head -n1)

if [[ -n $keyboard_device ]]; then
    input_device="/dev/input/$keyboard_device"
    echo "Detected keyboard input device: $input_device"
else
    echo "No keyboard input device found. Please check manually."
    exit 1
fi


echo "The keyboard input device is stored in the variable: \$input_device"
echo "Value: $input_device"
