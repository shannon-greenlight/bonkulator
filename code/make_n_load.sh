
#!/bin/zsh

./make.sh
if [ $? -ne 0 ]; then
    echo "Compiler Error: $?"
    exit $?
fi
./load_recent.sh