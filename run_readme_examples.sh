#!/bin/sh
examples="$(cat README.md | grep 'build/regexer')"

echo "$examples" | while IFS= read -r line
do
    echo "Running: $line"
    eval "$line"
    echo
done
