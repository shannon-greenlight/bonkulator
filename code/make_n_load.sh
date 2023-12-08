#!/bin/zsh

start=$(date +%s)

./make.sh
if [ $? -ne 0 ]; then
    echo "Compiler Error: $?"
    exit $?
fi

end=$(date +%s)

elapsed=$(( end - start ))
hours=$(( elapsed / 3600 ))
minutes=$(( (elapsed % 3600) / 60 ))
seconds=$(( elapsed % 60 ))

printf 'Elapsed time: %02d:%02d:%02d\n' $hours $minutes $seconds

./load_recent.sh