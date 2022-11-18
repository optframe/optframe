## Optimization Concepts - General

On general two concepts are considered on Search methods (they will be better explained in next section):
- XES and XSH: XES is base type for (primary) search type XSH, which is on general words "what users want"
- XES2 and XSH2: XES2 is base type for (secondary) search/exploration type XSH2, which is "what method uses to give what users want"

Typically, XSH2=XSH=XES for trajectory-based optimization, and XSH2 is population for population-based methods. On multi-objective scenarios, XSH may be some Pareto structure, while XSH2 some "Pareto Population" (see NSGA-II, for example).


## C++ Concepts (Intro)

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

###  Technical Notes on Initializers

Initializers (for XSolution, XESolution and X2ESolution)

- Constructive: generates XSolution object
  * good for initializing single solution without worrying about XEvaluation space
- InitialMultiSolution: generates multiple XSolution objects
  * good for initializing multiple solutions (or Population) without worrying about XEvaluation space
  * a typical output could be a MultiSolution<S>, Population<S> or VPopulation<S>
- InitialMultiESolution: generates multiple XESolution objects
  * good for initializing multiple solutions (or Population) already with evaluation values.
  * note that objective values may be valid or not, depending on X2ESolution type...
     - EPopulation, MultiESolution and VEPopulation allows for initial objective values to be in invalid state
- InitialSearch: generates XSearch object (XESolution or X2ESolution) 
  * It can be used to initialize both search spaces, although not imposing any "size" constraints on "multi" types (X2ESolution)... 
     - it works as a "transparency layer" that means: "I don't care how you generate, just generate for me!"
