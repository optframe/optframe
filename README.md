OptFrame
========

<p align="center">
  <a href="https://github.com/optframe/optframe">
    <img src="https://tokei.rs/b1/github/optframe/optframe?category=lines" alt="Current total lines.">
  </a>
  <a href="https://github.com/optframe/optframe/blob/master/LICENSE">
    <img src="https://img.shields.io/badge/License-LGPL%20v3-blue.svg" alt="License.">
  </a>
</p>

## OptFrame 4.0-alpha - Optimization Framework

OptFrame v4.0 is here!

Now, with newest C++20 features and integrated Unit Testing for user projects and examples (still ongoing work). Scanner++ library is also dettached independent project now.
Remember to clone this repo using `--recursive` option, so you don't miss anything ;)

Major changes happened from v3 to v4, so we will need to update our tutorials...

Check [https://optframe.github.io/docs/quick-start-guide/](https://optframe.github.io/docs/quick-start-guide/)

## Project Structure

Project is organized in three main folders: `src`, `libs`, `tests` and `bin`.
* On `src/OptFrame` and `tests/OptFrame` are code and testing for OptFrame framework itself.
* On `src/Examples` and `tests/Examples` you can find code and tests for the provided Examples.
* On `src/MyProjects` and `tests/MyProjects` you can find code and tests for your own codes.
* On `bin`, you can find classic `mct.sh` script, that will generate a new project `X` on `src/MyProjects/X` folder (and tests on `tests/MyProjects/X`).
* On `libs` you can find third-party libraries.
Tests are provided via `gtestlib` (on root folder), and you can also find `scannerpplib` (Scanner++ library) there.
To build `gtestlib`, just `cd libs/gtestlib/build && cmake .. && make`.

## Concepts (Lite)

A major change happened from OptFrame v3 to v4, where C++ Concepts were finally adopted (before official release of C++20 we use `concepts lite` on `gcc-7`).
This allowed a further simplification of project, by dividing most templates into two basic categories:
- Solution Space: `XS` template concept
- Objective Space: `XEv` template concept

On general, methods will work over a simplified Search Space, composed by a pair `<XS, XEv>` (or equivalent structure). Users will likely just use a pair of classical `Solution` and `Evaluation` classes, or inherit from `IESolution` to make its own personalized class ;)

Note that this will be valid for both Single- and Multi-Objective problems, unifying even more all kinds of techniques, into basic containers.

Specially for Multi-Objective problems, the standard Search Space will consist of `2^XES` space, where `XES=<XS, XEv>`. A classic member of `2^XES` is the `Pareto` class (together with a `MultiEvaluator`).

### Solution and Evaluation containers

Basic containers are still default `Solution` and `Evaluation`. Note that both are now not mandatory (thanks to concepts). It's still a very good deal to use them, as they allow templated primitive types like `int` and `double` to be directly used on `Evaluation`, also using basic std structures like `vector` directly on `Solution`.
If you don't like them, feel free to just replace them (`XS` and `XEv` can become virtually anything you want).

One can also personalize `Evaluation` class with a `MultiObjValue` tuple. This theoretically allows a unification between SingleObj and MultiObj methods, without further changes.

OptFrame 3 relied strongly on templates `R` and `ADS`, but now those only exist for basic Solution containers, and are not mandatory anymore (yet, it may still be good to follow this amazing optimization pattern).

Things are still moving, more big changes are coming.

### Search Space details

Search Space is defined on OptFrame as the pair of Solution* Space and Objective Space.
It also accepts a powerset of Solution Space together with powerset of Objective Space (for population/multiobjective search).

(*) Solution Space is also called Decision Space by some authors: `Lust, T., & Teghem, J. (2009). Two-phase Pareto local search for the biobjective traveling salesman problem. Journal of Heuristics, 16(3), 475–510. doi:10.1007/s10732-009-9103-9`.

### InitialSearch

`InitialSearch` corresponds to (OptFrame v3) `Constructive`, in the same way, Solution Space corresponds to Search Space.
Thus it can be used to generate initial valid "solution" that also comprises an objective value.

### Examples

Please take a deep look at `Examples` folder, and build them by simply typing `make` there.
Each example is connected to some previous literature work, and can demonstrate how each metaheuristic work on practice.
We know some detailed tutorial is still missing, but we hope to provide more info as soon as possible. Feel free to open issues and new discussions on GitHub ;)

## Submodules

Getting submodules: `git submodule update --init --recursive` and `git pull --recurse-submodules`.

## Using MCT script - Make a Compilable Thing!

Just type `cd ./bin && ./mct.sh` (or `make mct`) and follow the instructions on screen.
You can find your new project on `src/MyProjects/` (and tests on `tests/MyProjects/`).

## Special thanks for SourceForge.net

The OptFrame project was hosted in SourceForge.net for 8 years and we are
very grateful for all support during this time. Thanks to this support, we
have published several academic papers and solved many large scale problems
during these years. In order to easily interact with new project collaborators
the project is now moved to GitHub platform.

Thanks a lot SourceForge/GitHub!

## A brief history of OptFrame

### OptFrame v1
* Born on 2007 at Universidade Federal de Ouro Preto, Ouro Preto/MG, Brazil
* Very early phases of project, mostly used for Open Pit Mining optimization and VNS/ILS
* Too many type casts (no templates yet!)
* No efficient version control for source files

### OptFrame v2
* Adopted templates and nice metaprogramming design
* Adopted SVN version control
* Evolution on many aspects, due to collaborations on Universidade Federal Fluminense, Niterói/RJ, Brazil
* Mostly maintained by four initial OptHouse members: Igor Machado, Sabir Ribas, Pablo Munhoz and Mário Perché

### OptFrame v3
* Stable release with many techniques for Single and Multi-objective
* Automated runtime testing with CheckCommand
* Practical implementations on several Master and PhD thesis
* Professional implementations on industry software
* Moved to Git source control
* Mostly maintained by Igor Machado and Vitor Nazário Coelho

### OptFrame v4
* Just born with C++ Concepts in mind!
* Byproduct of several worldwide collaborations, with best known software management practices
* Focusing on achieving maximum C++ performance, solving too many memory management complex strategies (fully using self managed memory by smart pointers and move semantics)
* Maximum personalization by the user

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
```

 
--

[**LICENSE GNU Lesser General Public License v3**](https://github.com/optframe/optframe/blob/master/LICENSE)

*OptFrame maintainers* [@igormcoelho](https://github.com/igormcoelho) and [@vncoelho](https://github.com/vncoelho)

Copyright (C) 2007-2020

The OptFrame team
