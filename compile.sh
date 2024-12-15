#!/bin/bash
mkdir -p build
gcc -o build/c6502 src/main.c src/cpu.c -I./src
