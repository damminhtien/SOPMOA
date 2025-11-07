# SOPMOA*: Shared-OPEN Parallel Multi-Objective Search

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](#)
[![CMake](https://img.shields.io/badge/CMake-3.16%2B-blue.svg)](#)
[![OpenMP](https://img.shields.io/badge/OpenMP-enabled-brightgreen.svg)](#)
[![oneTBB](https://img.shields.io/badge/oneTBB-enabled-brightgreen.svg)](#)
[![License: AGPL v3](https://img.shields.io/badge/License-AGPLv3-orange.svg)](#license)

**SOPMOA*** is a high-performance framework for **exact multi-objective shortest-path (MOSP)** search on large graphs. It uses a **shared concurrent OPEN** with **per-node Pareto sets**, lightweight dominance checks, and a thread-safe update protocol to scale across many cores.

The repository bundles a suite of exact MOSP solvers:

* **SOPMOA*** (this work) and **SOPMOA-bucket**
* **LTMOA*** (+ array & lazy variants)
* **EMOA***, **NWMOA**, **BOA***, **ABOA***

> Target users: search researchers, OR/AI practitioners, and systems engineers who need reproducible MOSP baselines and strong parallel performance.



## Table of contents

* [Features](#features)
* [Quick start](#quick-start)
* [Requirements](#requirements)
* [Install](#install)
* [Build](#build)
* [Data & scenarios](#data--scenarios)
* [Usage](#usage)
* [Outputs](#outputs)
* [Reproducibility](#reproducibility)
* [Performance tips](#performance-tips)
* [Project layout](#project-layout)
* [Cite](#cite)
* [License](#license)
* [Acknowledgments](#acknowledgments)



## Features

* **Exact Pareto-optimal search** for 3–4 objectives on road-like graphs
* **Parallel**: shared concurrent priority queue + per-node locked Pareto sets
* **Pluggable algorithms**: SOPMOA*, LTMOA*, EMOA*, NWMOA, BOA*, ABOA*
* **Scenario runner**: batch queries from JSON, wall-time limits, logging
* **Reproducible**: CSV logs, deterministic seeds, hardware/threads control



## Quick start

```bash
# 1) Build (Release)
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j

# 2) Run a demo (replace with your actual files)
./main \
  -m maps/NY-d.txt maps/NY-t.txt maps/NY-c.txt \
  --alg SOPMOA \
  --scenario scen/good.json \
  --timelimit 3600 \
  --threads 16 \
  --outdir out/sopmoa \
  --logcsv out/sopmoa/results.csv \
  --logsols out/sopmoa/solutions \
  --seed 42
```

> Tip: `./main --help` prints the exact option names supported by your build.



## Requirements

* **OS**: Ubuntu 22.04+/Debian 12+, or macOS 14+ (Apple Silicon or x86_64)
* **Toolchain**: CMake ≥ 3.16, **g++ ≥ 11** (or Clang ≥ 14), C++17
* **Libraries**:

  * **Boost**: `program_options`, `filesystem`, `system`, `thread`, `log`
  * **OpenMP** (runtime + headers)
  * **oneTBB** (Threading Building Blocks)

### Ubuntu/Debian

```bash
sudo apt update
sudo apt install -y build-essential cmake \
  libboost-all-dev libomp-dev libtbb-dev
```

### macOS (Homebrew)

```bash
brew install cmake boost llvm tbb
# If using Apple Clang, OpenMP comes via llvm:
echo 'export CC=/opt/homebrew/opt/llvm/bin/clang'   >> ~/.zshrc
echo 'export CXX=/opt/homebrew/opt/llvm/bin/clang++'>> ~/.zshrc
echo 'export LDFLAGS="-L/opt/homebrew/opt/llvm/lib"'>> ~/.zshrc
echo 'export CPPFLAGS="-I/opt/homebrew/opt/llvm/include"'>> ~/.zshrc
```



## Build

```bash
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j
# Optional: run a quick smoke test
./main --help
```

> **Tip:** Avoid hard-coding `/usr/bin/g++`; let CMake detect your toolchain.



## Data & scenarios

### Graph & weight files

Each objective is provided as a separate weight file over the same graph. A typical **edge list** format is:

```
# Optional comments begin with '#'
<from> <to> <w>           # single-objective, or
<from> <to> <w1> <w2> ... # multi-objective per line (alt. format)
```

In this repository, MOSP commonly uses **one file per objective**, e.g.:

```
maps/NY-d.txt   # objective 1: distance
maps/NY-t.txt   # objective 2: travel time
maps/NY-c.txt   # objective 3: cost (or risk)
```

> See the `maps/` folder for concrete samples used in our experiments.

### Scenario JSON

Batch queries are defined in a JSON array:

```json
[
  { "name": "q001", "start_data": 96000, "end_data": 38000 },
  { "name": "q002", "start_data": 112233, "end_data": 445566 }
]
```

Place your scenarios in `scen/` and pass via `--scenario`.



## Usage

**Common options** (final names may differ; check `--help`):

```
-m,  --maps <file1> [file2 file3 ...]   Weight files (one per objective)
-a,  --alg  <NAME>                      SOPMOA | SOPMOA_bucket | LTMOA | LazyLTMOA | EMOA | NWMOA | BOA | ABOA
-s,  --scenario <file.json>             Batch of queries (start/end ids)
-q,  --query <u v>                      Single query override (no scenario)
-t,  --threads <N>                      Number of worker threads
     --timelimit <sec>                  Wall-clock time limit per query
     --seed <int>                       RNG seed (for sampling, tie-breaking)
     --outdir <dir>                     Directory for logs/solutions
     --logcsv <file.csv>                CSV summary of runs
     --logsols <dir>                    Dump Pareto fronts per query
     --loglevel <trace|debug|info|warn|error>
```

**Algorithm-specific knobs** (enabled only for relevant solvers):

```
# SOPMOA*
     --bucket-width <W>                 (for SOPMOA_bucket)
     --pq <shared|multi>                queue strategy (if compiled)
     --strict-dominance                 use strict instead of weak

# LTMOA variants
     --lazy                             enable lazy update variant
     --array                            array-based container for fronts
```

**Examples**

Run SOPMOA* on 3-objective NYC graph, 1-hour limit, 16 threads:

```bash
./main -m maps/NY-d.txt maps/NY-t.txt maps/NY-c.txt \
       --alg SOPMOA \
       --scenario scen/good.json \
       --timelimit 3600 \
       --threads 16 \
       --outdir out/sopmoa \
       --logcsv out/sopmoa/results.csv \
       --logsols out/sopmoa/solutions
```

Compare against LTMOA* and EMOA*:

```bash
for ALG in LTMOA EMOA; do
  ./main -m maps/NY-d.txt maps/NY-t.txt maps/NY-c.txt \
         --alg "$ALG" \
         --scenario scen/good.json \
         --timelimit 3600 \
         --threads 16 \
         --outdir "out/$ALG" \
         --logcsv "out/$ALG/results.csv" \
         --logsols "out/$ALG/solutions"
done
```



## Outputs

* **CSV (`--logcsv`)**: one row per query/algorithm with at least:

  * `alg, name, threads, timelimit, seed, generated, expanded, open_max, gcl_max, runtime_ms, solved`
* **Solutions (`--logsols`)**: one file per query containing the Pareto front

  * Typical columns: `f1,f2,f3[,f4]` and optional path/length stats
* **Console logs**: progress, timing, and end-of-run summary

> Column names can be adapted to your current logger; keep them consistent across solvers for easy plotting.



## Reproducibility

To match paper runs:

* **Hardware**: record CPU model, sockets, cores, L3 cache, RAM
* **Software**: OS, compiler version, Boost/OpenMP/TBB versions
* **Threading**:

  * `OMP_PROC_BIND=close` and `OMP_PLACES=cores` (OpenMP)
  * `TBB_NUM_THREADS=<N>` (if using TBB thread control)
* **Seeds**: fix `--seed`, and freeze scenario files
* **Commands**: check in your exact command lines in `scripts/`

Example (16 cores, pinned):

```bash
export OMP_PROC_BIND=close
export OMP_PLACES=cores
export TBB_NUM_THREADS=16
./main ... --threads 16 ...
```



## Performance tips

* Prefer **Release** builds (`-O3 -DNDEBUG`) for all runs.
* Scale threads in {1, 2, 4, 8, 16, 20, 24, 32} and plot speedup curves.
* Use **separate weight files** per objective to simplify I/O and caching.
* For heavy batches, set `--outdir` to a local SSD path to avoid I/O stalls.
* If you enable the *multi-queue* PQ (when available), expect reduced contention at ≥16 threads; otherwise, the shared PQ is simpler and strong up to mid-teens.



## Project layout

```
.
├── inc/algorithms/         # headers for SOPMOA*, LTMOA*, EMOA*, ...
├── src/algorithms/         # implementations
├── src/problem/            # graph, loaders, heuristics
├── src/utils/              # logging, timing, argument parsing
├── maps/                   # example weight files (see formats above)
├── scen/                   # scenario JSONs (batch queries)
├── scripts/                # helper scripts (plotting, benchmarks)
├── query_generator.py      # (optional) scenario helpers
├── query_random.py         # (optional) scenario helpers
├── CMakeLists.txt
└── README.md
```



## Cite

If you use SOPMOA* or this codebase in academic work, please cite:

```bibtex
@inproceedings{YourSOPMOA2025,
  title     = {SOPMOA*: Shared-OPEN Parallel A* for Multi-Objective Shortest Paths},
  author    = {Your Name and Collaborators},
  booktitle = {Proceedings of ...},
  year      = {2025},
  note      = {Code: https://github.com/<your_org>/<your_repo>}
}
```

> You can also add a `CITATION.cff` file to enable “Cite this repository” on GitHub.



## License

This project is licensed under **AGPL-3.0**. See [`LICENSE`](LICENSE) for details.



## Acknowledgments

We acknowledge foundational MOSP solvers in the literature (LTMOA*, EMOA*, NWMOA, BOA*, ABOA*) and the open-source ecosystem around **Boost**, **OpenMP**, and **oneTBB**.



### Changelog (optional)

Maintain a `CHANGELOG.md` with semantic versioning once you publish your first release.



### Contributing (optional)

* Use feature branches and conventional commit messages.
* Run CI locally: `cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo && cmake --build build -j`
* Open a PR with a clear problem statement, baseline command, and results snippet.

