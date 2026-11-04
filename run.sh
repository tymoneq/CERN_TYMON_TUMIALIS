#! bin/bash

make clean
echo "[+] Compiling Solution"
mkdir -p build
cd build 
cmake ..
cd ../
cmake --build build
echo "[+] Compilation finished with success"
echo "[+] Starting simulation"
./build/app

echo "[+] Running root macro"
root -l -b -q draw_plots.C &> /dev/null
echo "[+] Finished"
