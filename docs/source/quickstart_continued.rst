Quick start (Continued)
=======================

We expand the knowledge from `Quick Start <../quickstart>`_ where the user
has learned how to `Install <../install>`_ OptFrame, and how to compile and
test a `Simulated Annealing <https://en.wikipedia.org/wiki/Simulated_annealing>`_ metaheuristic
for the classic 0-1 Knapsack Problem (01KP).

We demonstrate how to update this code for other metaheuristics, for the classic Traveling Salesman Problem (TSP).

Problem definition and exploration methods
------------------------------------------

At this point, we assume the reader is familiarized with the Traveling Salesman Problem...
we intend to expand this section in the future with figures and more motivation (for now, sorry, and let's move on).

TSP Solution definition
^^^^^^^^^^^^^^^^^^^^^^^

We define a TSP solution as a permutations of $N$ cities being visited by a Traveling Salesman.
In this representation, each city is represented as a number $0..N-1$, being a solution a
vector of N integers (example: [0,2,3,1] means that solution starts from city 0, then follows to city 2,
then city 3, then city 1, and finally comes back to city 0). 
Objective is to find a route that minimizes distance between the $N$ visited cities.

We may define ESolutionTSP as a pair, containing a solution and its objective value (double).

..
    // COMMENTS 
    using ESolutionTSP = std::pair<
        std::vector<int>,  // first part of search space element: solution (representation)
        Evaluation<double> // second part of search space element: evaluation (objective value)
    >;

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/TSP-fcore-part1.hpp
    :linenos:
    :language: c++


Problem Data
^^^^^^^^^^^^

We read a matrix of distances between pairs of cities (considering Euclidean distance), and
store in a structure named ProblemContext. Do not forget to #include helpers from optframe namespace, 
such as Matrix and Scanner.

..
    // COMMENTS
    // TSP problem context and data reads
    class ProblemContext
    {
        public:
        int n;               // number of clients
        Matrix<double> dist; // distance matrix (Euclidean)
        // load data from Scanner
        void load(Scanner& scanner)
        {
            n = *scanner.nextInt();      // reads number of clients
            dist = Matrix<double>(n, n); // initializes n x n matrix
            //
            vector<double> xvalues(n);
            vector<double> yvalues(n);
            //
            for (int i = 0; i < n; i++) {
                scanner.next();
                xvalues[i] = *scanner.nextDouble(); // reads x
                yvalues[i] = *scanner.nextDouble(); // reads y
            }
            // calculate distance values, for every client pair (i,j)
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    dist(i, j) = distance(xvalues.at(i), yvalues.at(i), xvalues.at(j), yvalues.at(j));
        }
        // euclidean distance (double as return)
        double distance(double x1, double y1, double x2, double y2)
        {
            return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
        }
    };

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/TSP-fcore-part2.hpp
    :linenos:
    :language: c++

Finally, we declare a global object :code:`ProblemContext pTSP` (for simplicity), that includes all data.


Evaluation
^^^^^^^^^^^

Objective calculation can be done by using Functional Core class FEvaluator, which is a huge
simplification from OptFrame Core components Evaluator (for single objectives) and 
GeneralEvaluator (for single and multiple objectives).

..
    // COMMENTS
    Evaluation<double>
    fevaluate(const std::vector<int>& s)
    {
        // calculates distance from city to city in solution 's',
        // according to matrix 'dist'
        double f = 0;
        for (int i = 0; i < int(pTSP.n) - 1; i++)
            f += pTSP.dist(s[i], s[i + 1]);
        f += pTSP.dist(s[int(pTSP.n) - 1], s[0]);
        return Evaluation<double>{ f };
    }

    // Evaluate (also a global object... for simplicity!)
    FEvaluator<ESolutionTSP, MinOrMax::MINIMIZE>
    ev{
        fevaluate
    };

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/TSP-fcore-part3.hpp
    :linenos:
    :language: c++


Search methods based on neighborhoods
-------------------------------------

The next components will depend on the type of search method used, we start with 
neighborhood-based search techniques.

Random constructive
^^^^^^^^^^^^^^^^^^^

In a similar manner with Knapsack example (on Quickstart part 1), we define
a random solution generator.

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/TSP-fcore-part4.hpp
    :linenos:
    :language: c++

First move operator: Swap
^^^^^^^^^^^^^^^^^^^^^^^^^

We start with a Move operator capable of exchanging two elements from a given TSP solution.
We have four types of neighborhood definitions in OptFrame (NS, NSFind, NSSeq and NSEnum), but
two major are NS and NSSeq.

NS works well for random neighborhoods, with no intent of explicitly
visiting all possible neighbors (also useful for continuous or infinite neighborhoods).
Swap move and NS definition can be seen below.


.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/TSP-fcore-part5.hpp
    :linenos:
    :language: c++

We now define the NSSeq neighborhood with a explicit iterator definition, that requires four
operations: first (initializes first valid move), next (skips to next valid move), 
current (returns current move) and isDone (indicates if no move exists).

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/TSP-fcore-part6.hpp
    :linenos:
    :language: c++

At this point, we quickly demonstrate how novel C++20 features, such as Coroutines,
have improved OptFrame in latest versions (named FxCore library). 
We note that all four iterator operations (first, next, current and isDone) are made
available quite naturally with a single coroutine generator that executes co_yield for
each available move.

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/TSP-fxcore-nsseq.hpp
    :linenos:
    :language: c++

We will explore more of these bleeding-edge tools in an advanced topic.

Complete Example for TSP Components
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For simplicity, we separate the main TSP components in a file named :code:`TSP-fcore.hpp`.

.. hint::
    This example could be made in a single file, to be even simpler. However, we recommend users to have  
    a clear separation for the header declaration of *FCore components* (on :code:`TSP-fcore.hpp`) 
    from the :code:`main()` entrypoint depending on method used, e.g., :code:`mainTSP-fcore-ils.cpp` or :code:`mainTSP-fcore-brkga.cpp`.

*TSP-fcore.hpp*

:code:`File 'TSP-fcore.hpp' located in 'demo/03_QuickstartTSP_VNS_BRKGA/'`

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/TSP-fcore.hpp
    :linenos:
    :language: c++


Exploring with neighborhood exploration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

OptFrame supports several strategies for neighborhood exploration, such as:
First Improvement, Best Improvement, Random Selection and Multi Improvement.
We can also combine several local search strategies in a multiple strategy
called Variable Neighborhood Descent (VND).

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/mainTSP-fcore-ils-part2.cpp
    :linenos:
    :language: c++

If one wants to build a complete metaheuristic, the Iterated Local Search (ILS) or a Variable
Neighborhood Search (VNS).
The ILS is based on general perturbation concept, so we will use the concept of Levels of 
Perturbation, that are increased when stuck in a poor quality local optimum. We adopt a perturbation 
strategy that tries to escape at level p by applying p+2 random moves, e.g., at level 0,
2 random moves are applied, and so on.

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/mainTSP-fcore-ils-part3.cpp
    :linenos:
    :language: c++




Complete Example for ILS
^^^^^^^^^^^^^^^^^^^^^^^^

We provide the main file for TSP ILS :code:`mainTSP-fcore-ils.cpp`.

*mainTSP-fcore-ils.cpp*

:code:`File 'mainTSP-fcore-ils.cpp' located in 'demo/03_QuickstartTSP_VNS_BRKGA/'`

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/mainTSP-fcore-ils.cpp
    :linenos:
    :language: c++

Search methods based on random keys
-----------------------------------

We finish with the Biased Random Key Genetic Algorithm (BRKGA), a simple metaheuristic
inspired by classic Genetic Algorithm, using the solution representation of $n$ Random Keys, 
which are $[0,1]^n$ float values.

Random key generation
^^^^^^^^^^^^^^^^^^^^^

The BRKGA requires an initial solution generator, which is in this case, $n$ random [0,1] floats.
This can be done automatically by the method (since its trivial do generate $n$ [0,1] random numbers),
but we choose to demonstrate manually (by inheriting from OptFrame Core class Initial Population).

This is good to tune the degree of randomness (number of random digits) and also the random function used.

..
    // COMMENTS
    class MyRandomKeysInitPop : public InitialPopulation<std::pair<std::vector<double>, Evaluation<double>>>
    {
        using RSK = std::vector<double>;

        private:
        int sz;
        sref<RandGen> rg;

        public:
        MyRandomKeysInitPop(int size, sref<RandGen> _rg = new RandGen)
            : sz{ size }
            , rg{ _rg }
        {
        }

        // copy constructor
        MyRandomKeysInitPop(const MyRandomKeysInitPop& self)
            : sz{ self.sz }
            , rg{ self.rg }
        {
        }

        Population<std::pair<RSK, Evaluation<double>>> generatePopulation(unsigned populationSize, double timelimit) override
        {
            Population<std::pair<RSK, Evaluation<double>>> pop;

            for (unsigned i = 0; i < populationSize; i++) {
                vector<double>* d = new vector<double>(sz);
                for (int j = 0; j < sz; j++)
                    d->at(j) = (rg->rand() % 100000) / 100000.0; // 6 digit precision on random keys
                pop.push_back(d);
            }

            return pop;
        }
    };

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/mainTSP-fcore-brkga-part2.cpp
    :linenos:
    :language: c++

BRKGA decoding
^^^^^^^^^^^^^^^

BRKGA also requires a decoder function, that maps this array of random keys into a permutation.

This can be easily done with Functional Core class FDecodeRK, and an interesting approach based
on sorting the keys, related to a predefined indexing of each key.

..
    // COMMENTS
    pair<Evaluation<double>, vector<int>>
    fDecode(const vector<double>& rk)
    {
        vector<pair<double, int>> v(rk.size());
        int k = 0;
        for (unsigned i = 0; i < v.size(); i++)
            v[k] = pair<double, int>(rk[i], i);

        // sort the pairs according to the random key value 
        sort(v.begin(), v.end(), [](const pair<double, int>& i, const pair<double, int>& j) -> bool {
            return i.first < j.first;
        });

        // TSP representation is vector<int>
        vector<int> p(v.size());
        for (unsigned i = 0; i < v.size(); i++)
            p[i] = v[i].second;

        Evaluation<double> e = ev.evaluate(p);
        return make_pair(e, p);
    }

    // evaluator random keys (for TSP)
    FDecoderRK<std::vector<int>, Evaluation<>, double, MinOrMax::MINIMIZE> decoder{
        fDecode 
    };


.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/mainTSP-fcore-brkga-part3.cpp
    :linenos:
    :language: c++

BRKGA with TSP
^^^^^^^^^^^^^^^

We are ready to build a TSP instance with 3 cities with coordinates (10,10), (20,20) and (30,30),
and invoke a BRKGA to solve it.

The parameters of BRKGA are: decoding function, initial solution generator, population size, number of iterations,
also rates for mutation (randomness), elite (best solutions), preference for elite solutions, and finally, a random generation method.

..
   // COMMENTS
   sref<RandGen> rg = new RandGen;

   // load data into problem context 'pTSP'
   Scanner scanner{ "3\n1 10 10\n2 20 20\n3 30 30\n" };
   pTSP.load(scanner);
   std::cout << pTSP.dist << std::endl;

   sref<DecoderRandomKeys<ESolutionTSP::first_type, ESolutionTSP::second_type, double>> _decoder = decoder;
   sref<InitialPopulation<std::pair<vector<double>, ESolutionTSP::second_type>>> _initPop = new MyRandomKeysInitPop(pTSP.n); // passing key_size

   //eprk, pTSP.n, 1000, 30, 0.4, 0.3, 0.6
   BRKGA<ESolutionTSP, double> brkga(
     _decoder,
     MyRandomKeysInitPop(pTSP.n, rg), // key_size = pTSP.n
     30,
     1000,
     0.4,
     0.3,
     0.6,
     rg);

   auto searchOut = brkga.search(10.0); // 10.0 seconds max

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/mainTSP-fcore-brkga-part4.cpp
    :linenos:
    :language: c++

The result from searchOut can be split in two parts, an error code and the returned solution 
(the same as in Simulated Annealing or any other OptFrame search method).


Complete Example for BRKGA
^^^^^^^^^^^^^^^^^^^^^^^^^^

We provide the main file for TSP BRKGA :code:`mainTSP-fcore-brkga.cpp`.

*mainTSP-fcore-brkga.cpp*

:code:`File 'mainTSP-fcore-brkga.cpp' located in 'demo/03_QuickstartTSP_VNS_BRKGA/'`

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/mainTSP-fcore-brkga.cpp
    :linenos:
    :language: c++

More Examples 
^^^^^^^^^^^^^

For a other examples, see folder Examples/FCore-BRKGA and execute :code:`bazel build ...`

.. warning::
    Feel free to check folder :code:`OptFrame/Examples` for other examples on FCore and OptFrame Classic.

