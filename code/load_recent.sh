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

port=$(cat spank_port.txt)
echo "Using: $port"
read -p "What's the port? (please enter a number or press ENTER to use $port) 'x' to exit " port

if [ "$port" == "x" ]; then
    exit
fi

echo $port > spank_port.txtwhile true; do
    # Ask the user to select a port
    echo "Please enter the number of the COM port you want to use: "
    read selected_port_number

    if [ "$selected_port_number" == "x" ]; then
        exit
    fi

    if [ "$selected_port_number" == "" ]; then
        selected_port=$port
    else
        # Check if the input is a number and if the number corresponds to a port
        if [[ $selected_port_number =~ ^[0-9]+$ ]] && [ "$selected_port_number" -ge 1 ] && [ "$selected_port_number" -le ${#ports[@]} ]; then
            selected_port=${ports[$selected_port_number]}
        else
            echo "Invalid selection. Please try again."
            continue
        fi
    fi
done
./bin/arduino-cli upload -p $selected_port -b arduino:mbed_nano:nanorp2040connect -i ./out/Bonkulator.ino.bin
