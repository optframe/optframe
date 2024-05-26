OptFrame
========

<p align="center">
  <a href="https://github.com/optframe/optframe">
    <img src="https://tokei.rs/b1/github/optframe/optframe?category=lines" alt="Current total lines.">
  </a>
  <a href="https://github.com/optframe/optframe/blob/master/COPYING.LESSERv3">
    <img src="https://img.shields.io/badge/License-LGPL%20v3-blue.svg" alt="LGPLv3 License.">
  </a>
  <a href="https://github.com/optframe/optframe/blob/master/LICENSE">
    <img src="https://img.shields.io/badge/License-MIT-blue.svg" alt="MIT License.">
  </a>
</p>

[![Documentation Status](https://readthedocs.org/projects/optframe/badge/?version=latest)](https://optframe.readthedocs.io/en/latest/?badge=latest)

![C++17](https://img.shields.io/badge/std-c%2B%2B17-blue) ![C++20](https://img.shields.io/badge/std-c%2B%2B20-blue)


## OptFrame - C++ Optimization Framework

OptFrame `5.1.0` is released.

For optframe python version, see [github.com/optframe/pyoptframe-dev](https://github.com/optframe/pyoptframe-dev)

Project uses semantic versioning, so to patch it, run: `bumpver update --patch`

Now, with newest C++20 features and integrated Unit Testing for user projects and examples (still ongoing work). Scanner++ library is also dettached independent project now.
Remember to clone this repo using `--recursive` option, so you don't miss anything ;)

Major changes happened from v3 to v4, so we will need to update our tutorials...

-----

**Official Documentation on [ReadTheDocs](https://optframe.readthedocs.io)**


#### Try the Demos on [ReadTheDocs Quickstart](https://optframe.readthedocs.io/en/latest/quickstart.html)

- [00_SimpleBuild](./demo/00_SimpleBuild/README.md): 00_SimpleBuild  - [![00_SimpleBuild](https://github.com/optframe/optframe/actions/workflows/demo_00_simplebuild.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/demo_00_simplebuild.yml)
- [01_QuickstartWelcome](./demo/01_QuickstartWelcome/README.md): 01_QuickstartWelcome  - [![01_QuickstartWelcome](https://github.com/optframe/optframe/actions/workflows/demo_01_quickstartwelcome.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/demo_01_quickstartwelcome.yml)
- [02_QuickstartKP_SA](./demo/02_QuickstartKP_SA/README.md): 02_QuickstartKP_SA  - [![02_QuickstartKP_SA](https://github.com/optframe/optframe/actions/workflows/demo_02_quickstartkp_sa.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/demo_02_quickstartkp_sa.yml)
- [03_QuickstartTSP_VNS_BRKGA](./demo/03_QuickstartTSP_VNS_BRKGA/README.md): 03_QuickstartTSP_VNS_BRKGA  - [![03_QuickstartTSP_VNS_BRKGA](https://github.com/optframe/optframe/actions/workflows/demo_03_quickstart_tsp_vns_brkga.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/demo_03_quickstart_tsp_vns_brkga.yml)
- [05_Advanced_TSP_checkmodule](./demo/05_Advanced_TSP_checkmodule/README.md): 05_Advanced_TSP_checkmodule  - [![05_Advanced_TSP_checkmodule](https://github.com/optframe/optframe/actions/workflows/demo_05_advanced_tsp_checkmodule.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/demo_05_advanced_tsp_checkmodule.yml)

Also we provide a C library for Windows, Linux and Mac:
- [OptFrameLib](./include/OptFrameLib/README.md): OptFrameLib  - [![Test Library on windows and linux](https://github.com/optframe/optframe/actions/workflows/library.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/library.yml)


Note that OptFrame website may have obsolete documentation and examples:

~Check [https://optframe.github.io/docs/quick-start-guide/](https://optframe.github.io/docs/quick-start-guide/)~

### Intended changes for Major 5

- support global library for multiple languages
- fix multiple inheritance (DONE!)
- rename src/ to include/ and follow C++ package standards (DONE!)
- no 'using namespace std' 
- change to more compact and consistent formatting style (using cpplint tool)

## Project Structure

Project is organized in three main folders: `src`, `libs`, `tests` and `bin`.
* On `src/OptFrame` and `tests/OptFrame` are code and testing for OptFrame framework itself.
* On `Examples` you can find code and tests for the provided Examples.
* On `MyProjects` you can find code and tests for your own codes.
* On `bin`, you can find classic `optframe-mct.sh` script, that will generate a new project `X` on `MyProjects/X/src` folder (and tests on `MyProjects/X/tests`).
* On `libs` you can find third-party libraries (including testing and benchmarking, as they are used widely on examples and subprojects)
Tests are provided via `catch2`, benchmaking via [benchmark](https://github.com/google/benchmark) library, and also `scannerpplib` (Scanner++ library) for stream/file processing.

OptFrame Functional Core (FCore) is also provided (strict `c++20`):

* On `src/OptFCore` and `Examples/FCore-Examples` are code and testing for `FCore` framework itself.


## C++ and Development Requirements

The following requirements apply for projects (and subprojects):

* OptFrame
  - `c++17` - tested on `gcc-7` (without flags `-fconcepts`)
  - `c++20` - **BEST OPTION!**
* FCore - `c++20` - tested on `gcc-10.1` with flags `-fcoroutines`
* Scanner++ - `c++11` - tested on `gcc-7`

To install gcc-10.1, see these [Instructions](libs/gcc-10-install/INSTRUCTIONS.md).

To learn more about bazel build, versioning and general development requirements, see [DEVEL.md](DEVEL.md).

**IMPORTANT:** *Run `make check` for full check (need Bazel Build).*

## Concepts General

To learn a little bit of Optimization Concepts and C++ Concepts behind this project, see [OPTCONCEPTS.md](OPTCONCEPTS.md)


### Examples

Please take a deep look at `Examples` folder, and build them by simply typing `make` there.
Each example is connected to some previous literature work, and can demonstrate how each metaheuristic work on practice.
We know some detailed tutorial is still missing, but we hope to provide more info as soon as possible. Feel free to open issues and new discussions on GitHub ;)

#### Using Bazel

Each example is itself an independent workspace.
For this reason, each example has a reference to `@OptFrame` (as `local_repository`), 
while optframe root itself has a self reference for `@OptFrame` (as `local_repository`).

This allows building all examples from root, with `bazel build ...` or `bazel build //Examples/...`
One can also jump into each independent example and build it locally.

See available examples on bazel: `bazel query //Examples/...`

*Here are some examples and build advices for bazel.*

#### FCore-Examples

From root:
- `bazel build @FCore-Examples//...`
- `bazel run @FCore-Examples//src:app_TSPfcore`

Binary will be located at `./bazel-bin/external/FCore-Examples/src/app_TSPfcore`

From example (`cd Examples/FCore-Examples/`):
- `bazel clean`
- `bazel build ...`
- `bazel run //src:app_TSPfcore`

Binary will be located at `./Examples/FCore-Examples/bazel-bin/src/app_TSPfcore`

#### EternityII Example

From root:
- `bazel build @EternityII//...`
- `cd ./bazel-bin/external/EternityII/app_Example.runfiles/EternityII/`
- `./app_Example`

Binary and data will be located at `./bazel-bin/external/EternityII/app_Example.runfiles/EternityII/`

From example (`cd Examples/EternityII/`):
- `bazel clean`
- `bazel build ...`
- `bazel run //:app_Example`

Binary and data will be located at `./Examples/EternityII/bazel-bin/app_Example.runfiles/__main__/`

## Submodules

Getting submodules: `git submodule update --init --recursive` and `git pull --recurse-submodules`.

## Using MCT script - Make a Compilable Thing!

Just type `cd ./bin && ./mct.sh` (or `make mct`) and follow the instructions on screen.
You can find your new project on `src/MyProjects/` (and tests on `tests/MyProjects/`).


## A brief history of OptFrame

For a brief history of OptFrame v1, v2, v3, v4 and v5 (current), see [HISTORY.md](HISTORY.md).


## Examples

Interesting examples are available (see `Examples` folder):

- [EternityII](./Examples/EternityII/README.md): Eternity II Puzzle  - [![Example Eternity II](https://github.com/optframe/optframe/actions/workflows/example_eternityii.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/example_eternityii.yml)
- [HFM](./Examples/HFM/README.md): HFM forecast framework by @vncoelho - [![Example HFM](https://github.com/optframe/optframe/actions/workflows/example_hfm.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/example_hfm.yml)
- [HFMVRP](./Examples/HFMVRP/README.md): Heterogeneous Fleet Vehicle Routing Problem - [![Example HFMVRP](https://github.com/optframe/optframe/actions/workflows/example_hfmvrp.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/example_hfmvrp.yml)
- [KP](./Examples/KP/README.md): Knapsack Problem (v1) - [![Example KP](https://github.com/optframe/optframe/actions/workflows/example_kp.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/example_kp.yml)
- [KP2](./Examples/KP2/README.md): Knapsack Problem (v2) 
- [MITSP](./Examples/MITSP/README.md): Multi Improvement Dynamic Programming for Traveling Salesman Problem - [![Example MITSP](https://github.com/optframe/optframe/actions/workflows/example_mitsp.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/example_mitsp.yml)
- [MODM](./Examples/MODM/README.md): Multi Objective Direct Marketing Problem - [![Example MODM](https://github.com/optframe/optframe/actions/workflows/example_modm.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/example_modm.yml)
- [OPM](./Examples/OPM/README.md): Open Pit Mining Operational Planning Problem 
- [OptHS](./Examples/OptHS/README.md): OptHouse Cleaning Scheduling - [![Example OptHS](https://github.com/optframe/optframe/actions/workflows/example_opths.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/example_opths.yml)
- [PMedCap](./Examples/PMedCap/README.md): Capacitated P-Median Problem - [![Example PMedCap](https://github.com/optframe/optframe/actions/workflows/example_pmedcap.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/example_pmedcap.yml)
- [PN](./Examples/PN/README.md): Number Partition Problem - [![Example PN](https://github.com/optframe/optframe/actions/workflows/example_pn.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/example_pn.yml)
- [SVRPDSP](./Examples/SVRPDSP/README.md): Single Vehicle Routing Problem with Deliveries and Selective Pickups 
- [TSP](./Examples/TSP/README.md): Traveling Salesman Problem (v1) - [![Example TSP](https://github.com/optframe/optframe/actions/workflows/example_tsp.yml/badge.svg)](https://github.com/optframe/optframe/actions/workflows/example_tsp.yml)
- [TSP2](./Examples/TSP2/README.md): Traveling Salesman Problem (v2)  

### Building Examples

To build examples, just type `make` on `Examples` folder.

The following examples are currently prioritary, due to heavy burden for maintaining all of them:

- pmedcap 
- kp 
- tsp 
- hfvrpmt
- mitsp
- etii
- opths 
- gfp (?)
- hfm 
- modm

For more information on the others, please file an Issue.

### Metaheuristic Implementations on Examples

These examples explore several (meta)-heuristic implementations, such as:

- Simulated Annealing
- Genetic Algorithm
- Memetic Algorithm
- Variable Neighborhood Search
- Iterated Local Search
- Tabu Search
- ...

Multi-Objective metaheuristics:

- NSGA-II
- MOVNS
- 2PPLS
- ...


## Installation

OptFrame is organized in several C++ headers.
To install it system-wide (in linux), just type `make install` (`sudo` will be necessary here):

- headers will be put on `/usr/local/include`
- examples, src and tests will be put on `/usr/local/optframe`

We recommend to run `make test-install` after that, to ensure everything is fine!

## VSCode Settings

We recommend the following settings for vscode (`.vscode/settings.json`):
```json
{
    "[cpp]": {
        "editor.tabSize": 3,
        "editor.detectIndentation": false
    },
    "C_Cpp.intelliSenseEngine": "Tag Parser",
    "C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle : Google , ColumnLimit : 0, IndentWidth: 2, AccessModifierOffset: -1}",
    "testMate.cpp.test.executables": "{tests,build,Build,BUILD,out,Out,OUT}/**/*{test,Test,TEST}*"
}
```

We recommend Microsoft `C/C++` and `C++ TestMate` extensions.

## Citation

Cite this in your paper as:

Coelho, I.M., Ribas, S., Perché, M.H.P., Munhoz, P., Souza, M.J.F., Ochi, L.S. (2010). 
OptFrame: a computational framework for combinatorial optimization problems. 
In Anais do XLII Simpósio Brasileiro de Pesquisa Operacional (SBPO). 
Bento Gonçalves-RS, pp 1887-1898.

```bibtex
@article{optframe2010,
    author = {Igor Coelho and Sabir Ribas and Mário Henrique de Paiva Perché and Pablo Munhoz and Marcone Souza and Luiz Ochi},
    year = {2010},
    month = {08},
    pages = {1887-1898},
    title = {OptFrame: a computational framework for combinatorial optimization problems},
    journal = "Simpósio Brasileiro de Pesquisa Operacional"
}
@article{optframe2020,
    author = {Coelho, Igor M. and N. Coelho, Vitor and  Zudio, Anderson and  Araújo, Rodolfo and Haddad, Matheus N. and Munhoz, Pablo Luiz A. and Maia, Breno S. M. and Ochi, Luiz Satoru and Souza,  Marcone Jamilson F.},
    year = {2020},
    title = {Microbenchmark Studies in OptFrame: a 10-Year Anniversary},
    journal = "Simpósio Brasileiro de Pesquisa Operacional"
}
```

 
## License

- `<= 4.1`: LICENSE [GNU Lesser General Public License v3 (LGPLv3+)](COPYING.LESSERv3)

- `>= 4.2`: Dual License: [MIT License](LICENSE) OR [LGPLv3+](COPYING.LESSERv3)

Please use the following SPDX identifier:
`SPDX-License-Identifier: MIT OR LGPL-3.0-or-later`

*OptFrame maintainers* [@igormcoelho](https://github.com/igormcoelho) and [@vncoelho](https://github.com/vncoelho)

Copyright (C) 2007-2023

The OptFrame team
