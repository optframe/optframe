Quick start
=============


OptFrame is a C++ framework to solve challenging optimization problems, specially by means
of metaheuristic techniques.
It has no external dependencies, except a C++ compiler compatible with C++17 (or C++20).

After `installing <../install>`_ OptFrame (or just cloning it), user can start building
a solution to the problem at hand.


Welcome Message
---------------

A useful abstraction was introduced since OptFrame 4.1, called OptFrame Functional Core (FCore).
The idea of FCore is to provide a simplified access to OptFrame programming classes, by means
of closures and lambdas. This way, a project that typically had 8 to 10 files is reduced to a single
file (all included in headers!).

The first test on FCore is to see if it's correctly compiling. So let's just include its main header,
and print its `welcome()` message:

..
    // THIS IS NOT PRINTED: COMMENT IN RESTRUCTURED TEXT (SEE REAL FILE INCLUSION BELOW)
    // file: 'mytest.cpp'
    #include<iostream>
    #include<OptFCore/FCore.hpp>
    int main() {
        std::cout << optframe::FCore::welcome() << std::endl;
        return 0;
    }

:code:`File 'mytest.cpp' located in 'demo/01_QuickstartWelcome/src/'`

.. literalinclude:: ../../demo/01_QuickstartWelcome/src/mytest.cpp
    :linenos:
    :language: c++

Build with make
^^^^^^^^^^^^^^^

One way to compile it locally is just copy src/ folder inside your project and see the results (using GCC C++ compiler)::

    g++ --std=c++20 mytest.cpp -o fcore_mytest
    ./fcore_mytest
    # Welcome to OptFrame Functional Core (FCore) - version 4.1-dev

If a local copy of OptFrame is being used in other folder (via git clone), one needs to pass its location via flag `-I`
to the compiler (*considering relative location of ./optframe/src/*):

..
    # COMMENTED!!
    g++ --std=c++20 -I./optframe/src mytest.cpp -o fcore_localtest
    ./fcore_localtest
    # Welcome to OptFrame Functional Core (FCore) - version 4.1-dev

:code:`File 'makefile' located in 'demo/01_QuickstartWelcome/'`

.. literalinclude:: ../../demo/01_QuickstartWelcome/makefile
    :linenos:
    :language: make


Build with bazel
^^^^^^^^^^^^^^^^

If one wants to build in Windows or any other system, easiest manner is to use Bazel.
Just create a *WORKSPACE.bazel* file that points to remote OptFrame git repository:

..
   # COMMENTED!!
   load('@bazel_tools//tools/build_defs/repo:git.bzl', 'git_repository')
   git_repository(
       name='OptFrame', 
       remote='https://github.com/optframe/optframe.git',
       branch='master'
   )

:code:`File 'WORKSPACE.bazel' located in 'demo/01_QuickstartWelcome/'`

.. literalinclude:: ../../demo/01_QuickstartWelcome/WORKSPACE.bazel
    :linenos:
    :language: python


And use the following *BUILD.bazel* file (with dependency to @OptFrame):

..
    # COMMENTS!!
    load("@rules_cc//cc:defs.bzl", "cc_library", "cc_binary")
    cc_binary(
        name = "fcore_localtest",
        srcs = ["mytest.cpp"],
        deps=["@OptFrame//include:OptFCore"],
        copts = ['--std=c++20']
    )

:code:`File 'BUILD.bazel' located in 'demo/01_QuickstartWelcome/'`

.. literalinclude:: ../../demo/01_QuickstartWelcome/BUILD.bazel
    :linenos:
    :language: python

Just invoke bazel build system (assuming MinGW C/C++ toolchain on Windows or just remove option :code:`--compiler` for Linux)::

    bazel build --compiler=mingw-gcc ... 
    bazel run :demo_welcome
    # Welcome to OptFrame Functional Core (FCore) - version 4.1-dev


A deeper explanation of OptFrame theoretical foundations can be found on `Concepts <./concepts.html>`_
section, so we will move fast here!


First Example: 0-1 Knapsack Problem and Simulated Annealing
-----------------------------------------------------------

Let's consider a classic problem: the 0-1 Knapsack Problem (KP).


.. only:: html

    |knapsack1|
    By: Dake `CC BY-SA 2.5 <https://commons.wikimedia.org/wiki/File:Knapsack.svg>`_

    .. |knapsack1| image:: _figs/Knapsack.svg
        :width: 300
        :alt: A knapsack and some items (from wikipedia)

.. only:: pdf

    |knapsack2|
    By: Dake `CC BY-SA 2.5 <https://commons.wikimedia.org/wiki/File:Knapsack.svg>`_

    .. |knapsack2| image:: _figs/Knapsack.pdf
        :width: 300
        :alt: A knapsack and some items (from wikipedia)


Given a set of items :math:`I`, the KP consists in selecting some items :math:`S \subseteq I`,
such that the sum of weights :math:`w_i` (for each selected item) do not exceed knapsack
capacity :math:`Q`, and profit :math:`p_i` of the selected items is maximized.

$$maximize\\; \\sum_{i \\in S} p_i$$
$$\\sum_{i \\in S} w_i \\leq Q$$
$$S \\subseteq I$$

Solution and Evaluation Types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. role:: cppcode(code)
  :language: cpp

.. role:: pyth(code)
  :language: python

Users must first define a *Representation*, which is a data structure that represents
an element in the *Solution Space* for the KP. A natural candidate here is an *array of booleans*, 
since we need to decide if we are going to carry each item or not. In C++, an interesting
approach is to use stl containers, such as a :cppcode:`std::vector<int>`.

User also needs to specify the data type for the *Objective Space*, in general a numeric type.
In this case, we will simply choose 
an :cppcode:`Evaluation<int>` (just ignore the *Evaluation* container for now...).

We declare a `XESolution <./concepts.html>`_ pair that aggregates both spaces as a single 
type :code:`ESolutionKP` (meaning an *evaluated solution for the knapsack problem*):

.. 
    // COMMENTS!!!
    // file: 'example.cpp'
    #include<iostream>
    #include<vector>
    #include<OptFCore/FCore.hpp>
    
    using namespace optframe;
    
    using ESolutionKP = std::pair<
        std::vector<bool>, // (representation)
        Evaluation<int> // (objective value)
    >;

.. literalinclude:: ../../demo/02_QuickstartKP_SA/KP-fcore-ex-part1.hpp
    :linenos:
    :language: c++

.. this is called an 'admonition'
.. https://docutils.sourceforge.io/docs/ref/rst/directives.html#code
.. https://thomas-cokelaer.info/tutorials/sphinx/rest_syntax.html#restructured-text-rest-and-sphinx-cheatsheet

.. hint::
    As long as fields :code:`first` and :code:`second` are provided on XESolution concept, any class can
    be used instead of :code:`std::pair`, as demonstrated on `Explicit XESolution <./advanced.html#explicit-xesolution>`_
    section.

.. warning::
    Since OptFrame v4, any data structure can be used as a *solution representation*, as long as it implements :code:`operator<<`.
    OptFrame Classic (<= v3) required a templated class named :code:`Solution<R,ADS>`, passing both
    a *representation* type :code:`R` and *auxiliary data structure* type :code:`ADS`. This is no longer mandatory, but still widely
    used on the available examples.

Problem Context
^^^^^^^^^^^^^^^

Users will need to store general problem information (such as profits and weights of items),
so a *ProblemContext* can be introduced.
For easy interoperability with file and string inputs (on Linux/Windows), we use *Scanner* class
to process problem data (some details of 'load' function will only be discussed in a later moment):

.. 
    // COMMENTS
    // example.cpp
    // ...
    #include <OptFrame/Scanner++/Scanner.hpp>
    using namespace scannerpp;

    class ProblemContext 
    {
    public:
        int n, Q;   // item count 'n' and knapsack capacity 'Q'
        std::vector<int> p, w; // profit 'p' and weight 'w' for each item

        // some 'load' function to read data from a Scanner
        void load(Scanner& scanner) {
            // reads data from scanner and store it on 'n', 'Q', 'p' and 'w' variables
            // details presented later...
        }
    };
    // global instance for problem data
    ProblemContext pKP;

.. literalinclude:: ../../demo/02_QuickstartKP_SA/KP-fcore-ex-part2.hpp
    :linenos:
    :language: c++

.. hint::
    ProblemContext is a user-defined class that can have any desired format. A 'load' function
    is just a suggestion, but not at all necessary. The object :code:`pKP` is declared in global scope
    just to simplify the example, but it could be embedded in any other component if user desires.

Random Constructive
^^^^^^^^^^^^^^^^^^^

We need to have some initial solution for the search process, so we just proceed in a random manner.
For simplicity, we allow infeasible solutions to be generated (as if capacity was infinite).

..
    // COMMENTS
    // declaring a 'frandom' function that generates initial solution
    std::vector<bool>
    frandom()
    {
        std::vector<bool> v(pKP.n, false); // no item is chosen at first
        for (unsigned i = 0; i < v.size(); ++i)
            v[i] = rand() % 2; // 50% chance to select an item (or not)
        // returns generated solution (feasible or infeasible)
        return v;
    }

    // Instantiates a FConstructive object for method 'frandom'
    FConstructive<std::vector<bool>> randomConstructive{ frandom };

.. literalinclude:: ../../demo/02_QuickstartKP_SA/KP-fcore-ex-part3.hpp
    :linenos:
    :language: c++

.. hint::
    User can also define many advanced constructive techniques in a similar manner, such as greedy 
    and greedy randomized approaches.


Evaluator
^^^^^^^^^

Now it's time to define an evaluation (or objective) function. According to the goal of 
maximizing the profits, we iterate over selected items to accumulate profit and weights.
As discussed in constructive section, we allow accumulated weight to surpass knapsack capacity,
for infeasible configurations. 
To discourage that, we introduce negative penalization whenever capacity is exceeded (assuming weight -1000000):

..
    // COMMENTS
    // function 'fevaluate' receives a const solution and returns an evaluation
    Evaluation<int>
    fevaluate(const std::vector<bool>& s)
    {
        int sum_p = 0; // sum profits    
        int sum_w = 0; // sum weights
        for (int i = 0; i < pKP.n; ++i)
            if (s[i]) // if element is selected in knapsack
            {
                sum_p += pKP.p[i]; // adds profit of selected item
                sum_w += pKP.w[i]; // adds weight of selected item
            }
        // check if knapsack capacity is exceeded (penalization strategy)
        if (sum_w >= pKP.Q)
            sum_p -= 1000000 * (sum_w - pKP.Q); // penalization proportional to exceeded weight
        
        return Evaluation<int>{ sum_p }; // returns Evaluation object
    }

.. literalinclude:: ../../demo/02_QuickstartKP_SA/KP-fcore-ex-part4.hpp
    :linenos:
    :language: c++

We have defined :code:`fevaluate` function to create an :code:`Evaluator` object named :code:`evalKP`,
in a :code:`MAXIMIZE` direction.

.. 
    // COMMENTS (SEE ABOVE FILE!)
    FEvaluator<ESolutionKP, MAXIMIZE> evalKP{ fevaluate };

.. hint::
    User can choose :code:`MINIMIZE` if dealing with a minimization problem. For multi-objective
    problems and Pareto optimization, user should visit `Multi-Objective <./advanced.html#multi-objective>`_ section.


Neighborhood Structure
^^^^^^^^^^^^^^^^^^^^^^

In order to improve a given solution, several metaheuristics employ `Local Search Optimization <https://en.wikipedia.org/wiki/Local_search_(optimization)>`_
techniques based on the concept of Neighborhood Structure. 
Every neighborhood is related to a move operator, which is required (on FCore) to have an undo operation (capable of reverting the effects of the move).


We create a :code:`BitFlip` move, that changes the :code:`true/false` selection of a given item :math:`k`.
In this case, the `move structure` (representation of the move) is just an :code:`int`, that represents the flipped item.

.. 
    // COMMENTS
    // MoveBitFlip (move structure is an 'int')
    using MoveBitFlip = FMove<int, ESolutionKP>;

    // function 'fApplyFlip' takes a move structure 'k' and flips the item 'k' of solution pair 'se'
    // example: if item k has value 'true' (selected) it becomes 'false' (not selected), otherwise it becomes 'true' (selected)
    int
    fApplyFlip(const int& k, ESolutionKP& se)
    {
        se.first[k] = !se.first[k]; // reverts the selection of item k in solution (se.first)
        return k;                   // returns the "undo move", which is in this case the same move 'k' (symmetric move)
    }

Note the :code:`makeMoveBitFlip` move generator based on FCore library.

.. literalinclude:: ../../demo/02_QuickstartKP_SA/KP-fcore-ex-part5-fcore.hpp
    :linenos:
    :language: c++

The definition of the move can be done using inheritance, if one finds easier (which is what we will adopt).

.. literalinclude:: ../../demo/02_QuickstartKP_SA/KP-fcore-ex-part5.hpp
    :linenos:
    :language: c++

Now, it's time to define a neighborhood generator for the move.
OptFrame has three main types of neighborhoods: :code:`NS`, :code:`NSSeq` and :code:`NSEnum`.

In this example, we will use :code:`NS`, since it only requires the generation of random moves:

..
    // COMMENTS
    // random generator for BitFlip moves (returned as C++11 unique pointers)
    std::unique_ptr<Move<ESolutionKP>>
    fRandomFlip(const ESolutionKP& se)
    {
        int k = rand() % pKP.n; // selects a random item in [0..n-1]
        // generates a MoveBitFlip object with function 'fApplyFlip' and move structure 'k'
        return std::unique_ptr<Move<ESolutionKP>>(new MoveBitFlip{ k, fApplyFlip });
    }

    // Defines a NS object for BitFlip with function 'fRandomFlip'
    FNS<ESolutionKP> nsFlip{ fRandomFlip };

.. literalinclude:: ../../demo/02_QuickstartKP_SA/KP-fcore-ex-part6.hpp
    :linenos:
    :language: c++

.. hint::
    It is usually a good idea to start developing over the simplest neighborhood, which is :code:`NS`.
    Most (non-deterministic) metaheuristics only requires a :code:`NS`, as it only requires the generation of random moves.
    More advanced neighborhoods based on iterators, such as :code:`NSSeq` and :code:`NSEnum` are only required for advanced `Local Search <./advanced.html#local-search>`_ methods.


Time to Test!
^^^^^^^^^^^^^

At this point, you can already test many nice metaheuristics and solve your knapsack problem!
We use the following code to load a problem instance (see `Complete Example`_ after):

.. 
    // COMMENTS!!
    // loading data into problem context 'pTSP'
    Scanner scanner{ File{ "knapsack-example.txt" } };
    pKP.load(scanner);
    std::cout << "number of items in problem:" << pKP.n << std::endl;

.. literalinclude:: ../../demo/02_QuickstartKP_SA/mainKP-fcore-ex-part2.cpp
    :linenos:
    :language: c++

.. hint::
    It is useful to test every FCore structure independently, so as to develop unit testing for them.

To test the constructive and evaluator:

..
    // COMMENTS
    // invokes method 'generateSolution' from 'FConstructive' to get random solution (no timelimit is passed)
    std::vector<bool> sol = *randomConstructive.generateSolution(0.0);
    // prints solution using OptFrame default operator<<
    std::cout << sol << std::endl;
    
    // evaluates the initial solution with 'evalKP' and generates XESolution pair
    ESolutionKP esol(sol, evalKP.evaluate(sol));
    // prints initial evaluation value (corresponding to random solution in pair)
    esol.second.print();

.. literalinclude:: ../../demo/02_QuickstartKP_SA/mainKP-fcore-ex-part3.cpp
    :linenos:
    :language: c++

Now we give an example with of the most well-known metaheuristics: the `Simulated Annealing <https://en.wikipedia.org/wiki/Simulated_annealing>`_.
It has few parameters, including: initial temperature :code:`T0`, cooling factor :code:`alpha`, and iterations per temperature :code:`iterT`.

.. 
    // COMMENTS
    // Good quality number generator for Simulated Annealing (by default Mersenne Twister)
    RandGen rg;
    
    // Creates 'InitialSearch' object called 'initRand' (similar to 'FConstructive' object 'randomConstructive')
    BasicInitialSearch<ESolutionKP> initRand(randomConstructive, evalKP);

    // Simulated Annealing with: alpha=98%, iterT=100, initial temperature T0 = 99999
    BasicSimulatedAnnealing<ESolutionKP> sa{
        evalKP, initRand, nsFlip, 0.98, 100, 99999, rg
    };

    // Simulated Annealing with stop criteria by time (= 10 seconds)
    SearchStatus status = sa.search(StopCriteria<ESolutionKP::second_type>{ 10.0 });
    
    // Extracts best solution so far and prints its evaluation value
    ESolutionKP bestSolution = *sa.getBestSolution();

    std::cout << bestSolution.first << std::endl;   // prints solution
    bestSolution.second.print();                    // prints evaluation
    
.. literalinclude:: ../../demo/02_QuickstartKP_SA/mainKP-fcore-ex-part4.cpp
    :linenos:
    :language: c++

Complete Example
----------------

.. warning::
    We present a complete example below. Note that some small differences may exist due to updates in tutorial, including language details.
    Feel free to check folder :code:`OptFrame/Examples` for other examples on FCore and OptFrame Classic.

Example is divided in two files: :code:`KP-fcore-ex.hpp` and :code:`mainKP-fcore-ex.cpp`.

.. hint::
    This example could be made in a single file, to be even simpler. However, we recommend users to have  
    a clear separation for the header declaration of *FCore components* (on :code:`KP-fcore-ex.hpp`) 
    from the :code:`main()` entrypoint (on :code:`mainKP-fcore-ex.cpp`), since unit testing is much simpler when these are decoupled.

*KP-fcore-ex.hpp*

:code:`File 'KP-fcore-ex.hpp' located in 'demo/02_QuickstartKP_SA/'`

.. literalinclude:: ../../demo/02_QuickstartKP_SA/KP-fcore-ex.hpp
    :linenos:
    :language: c++

*mainKP-fcore-ex.cpp*

:code:`File 'mainKP-fcore-ex.cpp' located in 'demo/02_QuickstartKP_SA/'`

.. literalinclude:: ../../demo/02_QuickstartKP_SA/mainKP-fcore-ex.cpp
    :linenos:
    :language: c++

*knapsack-example.txt*

:code:`File 'knapsack-example.txt' located in 'demo/02_QuickstartKP_SA/'`

.. literalinclude:: ../../demo/02_QuickstartKP_SA/knapsack-example.txt
    :linenos:
    :language: c++

To compile it (generates binary `app_KP`)::

    g++ -g -O3 --std=c++20 -I../../include  mainKP-fcore-ex.cpp -o app_KP


