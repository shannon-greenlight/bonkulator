#!/bin/zsh

# List available serial ports
echo "Available COM ports:"
ls /dev/tty.*

while true; do
    # Ask the user to select a port
    echo "Please enter the COM port you want to use: "
    read selected_port

    # Confirm the user's selection
    echo "You selected $selected_port. Is this correct? (y/n) "
    read confirm

    if [[ $confirm == [yY] || $confirm == [yY][eE][sS] ]]
    then
        echo "You confirmed: $selected_port"
        break
    else
        echo "Selection not confirmed. Please try again."
    fi
done