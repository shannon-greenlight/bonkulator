#!/bin/zsh

source ./list_ports.sh

port=$(cat spank_port.txt)

while true; do
    # Ask the user to select a port
    echo "Please enter the number of the COM port you want to use: (x to exit)"
    read selected_port_number

    if [ "$selected_port_number" = "x" ]; then
        exit
    fi

    if [ "$selected_port_number" = "" ]; then
        selected_port=$port
        break
    else
        # Check if the input is a number and if the number corresponds to a port
        if [[ $selected_port_number =~ ^[0-9]+$ ]] && [ "$selected_port_number" -ge 1 ] && [ "$selected_port_number" -le ${#ports[@]} ]; then
            selected_port=${ports[$selected_port_number]}
            break
        else
            echo "Invalid selection. Please try again."
            continue
        fi
    fi
done
echo $selected_port > spank_port.txt
./bin/arduino-cli upload -p $selected_port -b arduino:mbed_nano:nanorp2040connect -i ./out/Bonkulator.ino.bin
