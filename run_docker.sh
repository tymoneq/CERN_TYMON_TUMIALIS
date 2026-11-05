#!/bin/bash

echo "[+] building docker" 
set -e
docker build -t cern_analysis .

if [ ! -d "plots" ]; then
    echo "[+] Creating plots folder"
    mkdir plots
elif [ "$(ls -A plots 2>/dev/null | wc -l)" -gt 0 ]; then
    echo "[+] Clearing plots folder"
    rm -fr plots/*.*
fi

echo "[+] running solution"
docker run --rm -v "$PWD/plots:/workspace/build/plots" cern_analysis /bin/bash -c "
    if [ ! -d \"plots\" ]; then
        mkdir plots
    elif [ \"\$(ls -A plots 2>/dev/null | wc -l)\" -gt 0 ]; then
        rm plots/*.png plots/*.pdf plots/*.root 2>/dev/null || true
    fi
    
    ./app -n 100000 -t 8
    cp analysis.root plots/
    root -l -b -q ../draw_plots.C
"
echo "[+] solution finished"

