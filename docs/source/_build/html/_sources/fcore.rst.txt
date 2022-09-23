FCore Library
=============

FCore stands for OptFrame Functional Core, which is a simplified version of OptFrame Classic.
FCore was introduced in OptFrame v4.1, with functional-style abstractions for mostly used OptFrame components.
It is fully compatible with OptFrame classic, so users are able to interchange components (including all available metaheuristics).


Design Principles
-----------------

The FCore library has the following design principles:

- 1) User will write the least possible number of lines of code for its components
- 2) User will never explicitly use class inheritance, while still being able to explore fundamental code optimization strategies
- 3) User will be able to execute any existing (meta)-heuristic, only using FCore

Principle (1) is **accomplished**, and it also allows us to break compatibility in future releases (as long as it simplifies the FCore)
Principle (2) is **partially accomplished**, as Move costs are still not available (and considered fundamental)
Principle (3) is **partially accomplished**, since GRASP, GA and other metaheuristics are still not accessible (without class inheritance)

FCore assumes largely supported compilers, at least for Windows and Linux, and for this reason, C++17 is the current standard.

Extended FCore: the FxCore
--------------------------

Some interesting programming techniques can help to fulfill Principle (1), but they require C++20 and newer/experimental features.
For this reason, we provide FxCore library, as an Extended version of FCore.
We also assume that users of FxCore are familiar with FCore, and for this reason, we may allow greater number of configurations for Principle (2).
Users should be able to use the same number of metaheuristics, for both FCore and FxCore, regarding Principle (3).


Read more
---------


.. danger::
    This section is incomplete!

Project is released under LGPLv3 copyleft license.