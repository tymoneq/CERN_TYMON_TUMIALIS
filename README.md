# Proton-proton event simulation and analysis

This C++17 program generates minimum-bias (SoftQCD) proton-proton collisions with Pythia 8 at a center-of-mass energy of 13.6 TeV. It analyzes final-state particles, writes histograms to a ROOT file, and exports plots as PNG and PDF files.

## Multithreading

The simulation runs **8 worker threads concurrently**. Each thread has its own Pythia generator and analyzer, and processes 12,500 event attempts (100,000 attempts total). The worker histograms are merged after all threads finish. The program is intended to make use of up to 8 CPU cores; actual parallel execution depends on the cores available to the host and any limits imposed by the operating system or Docker. It does not reserve or pin specific cores.

Failed Pythia generation attempts are skipped, so fewer than 100,000 events may be analyzed.

## Run locally

### Requirements

- Linux with a C++17 compiler and CMake 3.15 or newer
- ROOT 6 with the Core, Hist, and RIO components; `root` must be available in the shell
- Pythia 8 with its headers, library, XML data, and `pythia8-config` available
- ROOT's environment initialized as required by its installation

Set `PYTHIA_PATH` if Pythia is not already discoverable. It should point to the Pythia installation directory containing `bin/pythia8-config`:

```bash
export PYTHIA_PATH=/path/to/pythia8
```

From the project root, run:

```bash
bash run_local.sh
```

The script configures and builds the project in `build/`, runs the simulation, then runs the ROOT plotting macro. It clears existing files in `plots/` before starting. On completion, `analysis.root` is in the project root, and the PNG and PDF plots are in `plots/`.

To build or run each step separately, from the project root:

```bash
cmake -S . -B build
cmake --build build
./build/app
root -l -b -q draw_plots.C
```

The executable accepts `-n` to set the number of event attempts and `-t` to set
the number of worker threads. Defaults are 100,000 attempts and 8 threads:

```bash
./build/app -n 100000 -t 8
```

Attempts are divided evenly among workers using a ceiling per-worker count, so
the total number of attempts can exceed `-n` by up to `-t` minus one when the
count is not evenly divisible. Failed generation attempts are skipped.

## Run with Docker

### Requirements

- Docker Engine running on the machine
- Internet access for the initial image build, which downloads and builds Pythia 8

From the project root, run:

```bash
bash run_docker.sh
```

The script builds the `cern_analysis` image, starts a container to run the simulation and plotting macro, and removes the container when it exits. The host's `plots/` directory is mounted into the container, so the generated PNG and PDF plots and a copy of `analysis.root` are available there. Existing files in `plots/` are cleared before the run.

The container uses the same 8-thread simulation. Docker may constrain the CPU resources available to it; this script does not set an explicit CPU limit or reserve 8 cores.

## What the program analyzes

For all charged particles and for π+, π−, K+, K−, protons, and antiprotons, the program records per-event multiplicity and particle-level transverse momentum, pseudorapidity, and azimuthal angle. Each thread fills its own histograms; the program combines them and writes the result to `analysis.root`. `draw_plots.C` creates one plot per category and variable, as PNG and PDF files (28 of each when all histograms are present).

The ROOT file contains these histograms for each category key `ch`, `pip`, `pim`, `Kp`, `Km`, `p`, and `pbar`:

| Variable | ROOT histogram name | Binning |
| --- | --- | --- |
| Multiplicity | `hN_<key>` | 400 bins, 0 to 400 |
| Transverse momentum | `hPt_<key>` | 100 bins, 0 to 10 GeV/c |
| Pseudorapidity | `hEta_<key>` | 100 bins, -5 to 5 |
| Azimuthal angle | `hPhi_<key>` | 64 bins, 0 to π radians |

The angle distribution is filled with the absolute value of Pythia's φ. Plot
filenames use the category key and ROOT variable prefix, for example
`plots/pip_hPt.png` and `plots/pip_hPt.pdf`.
