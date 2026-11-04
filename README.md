# Proton-proton event simulation and analysis

This project generates proton-proton collisions with Pythia 8, analyzes final-state particles, saves the histograms to a ROOT file, and exports plots as PNG and PDF files. It is configured for minimum-bias (SoftQCD) collisions at a center-of-mass energy of 13.6 TeV.

## Requirements

- A C++17 compiler and CMake 3.15 or newer
- ROOT 6, including the Core, Hist, and RIO components
- Pythia 8, including its headers, library, and `pythia8-config` utility
- `root` and `pythia8-config` available in the shell environment, with ROOT initialized as required by its installation

The Pythia installation path is currently set in `CMakeLists.txt` to `/home/tymon/coding/Pythia/pythia8318`. Change `PYTHIA_PATH` there if Pythia is installed elsewhere. CMake uses `pythia8-config --xmldoc` to find Pythia's XML settings directory.

## Run

From the project root, run the supplied script with Bash:

```bash
bash run.sh
```

The script cleans existing plot images, configures and builds the CMake project in `build/`, runs the simulation, and invokes the ROOT plotting macro. Running it with `bash` avoids relying on the script's current shebang line.

The equivalent steps can be run separately:

```bash
cmake -S . -B build
cmake --build build
./build/app
root -l -b -q draw_plots.C
```

Run these commands from the project root. The simulation writes `analysis.root` there, and the plotting macro writes its output under `plots/`. The `plots/` directory must exist before running the macro; it is included in this repository.

## What the program does

1. `main.cpp` creates eight worker threads. Each worker initializes its own Pythia instance for proton beams (`2212`, `2212`), minimum-bias SoftQCD, and `eCM = 13600` GeV. Each processes 12500 event attempts, for 100,000 attempts in total. A distinct deterministic Pythia seed is assigned to each worker (1 through 8).
2. `EventGenerator` skips attempts for which `pythia.next()` fails. For each successfully generated event, it collects only particles marked as final state by Pythia's `isFinal()` and passes them to the analyzer.
3. `Analyzer` fills distributions for all charged particles and for π+, π−, K+, K−, protons, and antiprotons. For each category it records the per-event multiplicity and the particle-level transverse momentum, pseudorapidity, and azimuthal angle.
4. Each thread accumulates its own histograms. After all threads finish, `main.cpp` adds the worker histograms and saves the combined output to `analysis.root`.
5. `draw_plots.C` reads the ROOT file and saves one canvas per available category/variable combination as both PNG and PDF in `plots/` (28 plots of each format when all histograms are present).

## Output histograms and plots

The ROOT file contains these histograms for each category key `ch`, `pip`, `pim`, `Kp`, `Km`, `p`, and `pbar`:

| Variable | ROOT histogram name | Binning in the implementation |
| --- | --- | --- |
| Multiplicity | `<key>_mult` | 400 bins, 0 to 400 |
| Transverse momentum | `<key>_pt` | 100 bins, 0 to 10 GeV/c |
| Pseudorapidity | `<key>_eta` | 1,100 bins, -5 to 5 |
| Azimuthal angle | `<key>_phi` | 64 bins, 0 to π radians |

The angle distribution is filled with the absolute value of Pythia's φ. Plot filenames follow the same keys, for example `plots/pip_pt.png` and `plots/pip_pt.pdf`. The plotting macro sets axis labels and draws the histogram; it does not add legends because each canvas contains one distribution.

## Notes

- The PDF specification requests 100 η bins, but the current implementation uses 1,100. Change the η histogram constructor in `src/Analyzer.cpp` to 100 bins if strict compliance with that requested bin count is needed.
- The PDF gives example histogram names such as `hN_pip`; the current ROOT names instead use `<key>_mult`, `<key>_pt`, `<key>_eta`, and `<key>_phi`.
- The requested output count is 100,000 event attempts. Since failed Pythia generation attempts are skipped, the number of events actually analyzed can be lower.
- The supplied run script removes existing plot files before running. It does not remove `analysis.root` explicitly; the analyzer recreates that file when the application completes.