#!/bin/zsh

# Check if arduino-cli exists
if [[ ! -f ./bin/arduino-cli ]]; then
    echo "arduino-cli not found. Downloading..."
    curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
fi

# Get a list of connected boards
board_list=$(./bin/arduino-cli board list)

# Parse the board list and store the ports in an array
echo "Available boards:"
ports=()
index=1
echo "$board_list" | while read -r line; do
    if [[ $line == *"/dev/cu"* ]]; then
        port=$(echo $line | awk '{print $1}')
        if [[ $line == *"Arduino Nano RP2040 Connect"* ]]; then
            echo "$index: Bonkulator found on $port"
            ports+=("$port")
            ((index++))
        elif [[ $line == *"Arduino NANO 33 IoT"* ]]; then
            echo "$index: Spankulator found on $port"
            ports+=("$port")
            ((index++))
        fi
    fi
done
