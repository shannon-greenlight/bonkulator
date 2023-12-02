#!/bin/zsh

# List available serial ports
echo "Available COM ports:"
ls /dev/tty.*

while true; do
    # Ask the user to select a port
    read -p "Please enter the COM port you want to use: " selected_port

    # Confirm the user's selection
    read -p "You selected $selected_port. Is this correct? (y/n) " confirm

    if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]
    then
        echo "You confirmed: $selected_port"
        break
    else
        echo "Selection not confirmed. Please try again."
    fi
done