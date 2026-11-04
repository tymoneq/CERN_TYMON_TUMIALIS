#! bin/bash

make clean
echo "[+] Compiling Solution"
cmake -S . -B build
cmake --build build
echo "[+] Compilation finished successfully"
echo "[+] Starting simulation"
./build/app

echo "[+] Running root macro"
root -l -b -q draw_plots.C &> /dev/null
echo "[+] Finished"
