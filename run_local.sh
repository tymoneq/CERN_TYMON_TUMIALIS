#!/bin/bash

if [ ! -d "plots" ]; then
    echo "[+] Creating plots folder"
    mkdir plots
elif [ "$(ls -A plots 2>/dev/null | wc -l)" -gt 0 ]; then
    echo "[+] Clearing plots folder"
    rm -fr plots/*.*
fi
set -e
echo "[+] Compiling Solution"
cmake -S . -B build
cmake --build build
echo "[+] Compilation finished successfully"
echo "[+] Starting simulation"
./build/app -n 100000 -t 8

echo "[+] Running root macro"
root -l -b -q draw_plots.C 
echo "[+] Finished"
