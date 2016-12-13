#!/bin/bash
if [ $# -eq 0 ]; then
    echo "usage: $0 <PID>"
    exit 1
fi
strace ldd 2>&1 | grep '^open(".*\.so"'
cat /proc/$1/maps | awk '{print $6}' | grep '\.so' | sort | uniq
lsof -p $1 | awk '{print $9}' | grep '\.so'
