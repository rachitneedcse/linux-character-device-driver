#!/bin/bash

# Build script for the user-space test program

echo "Building test_user program..."

# Compile the test program
gcc -o ../test_user ../src/test_user.c

if [ $? -eq 0 ]; then
    echo "Successfully compiled test_user"
    echo "You can now run: ./test_user"
else
    echo "Compilation failed!"
    exit 1
fi 