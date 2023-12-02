#!/bin/zsh

# List available serial ports and store them in an array
echo "Available COM ports:"
ports=(/dev/tty.*)
for i in "${!ports[@]}"; do 
    echo "$((i+1)): ${ports[$i]}"
done

while true; do
    # Ask the user to select a port
    echo "Please enter the number of the COM port you want to use: "
    read selected_port_number

    # Check if the input is a number and if the number corresponds to a port
    if [[ $selected_port_number =~ ^[0-9]+$ ]] && [ "$selected_port_number" -ge 1 ] && [ "$selected_port_number" -le ${#ports[@]} ]; then
        selected_port=${ports[$((selected_port_number-1))]}
    else
        echo "Invalid selection. Please try again."
        continue
    fi

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