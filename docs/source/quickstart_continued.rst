Quick start (Continued)
=======================

We expand the knowledge from `Quick Start <../quickstart>`_ where the user
has learned how to `Install <../install>`_ OptFrame, and how to compile and
test a `Simulated Annealing <https://en.wikipedia.org/wiki/Simulated_annealing>`_ metaheuristic
for the classic 0-1 Knapsack Problem (01KP).

We demonstrate how to update this code for other metaheuristics, for the classic Traveling Salesman Problem (TSP).

Solution definition and exploration method
----------------------------------------

We define a TSP solution as a permutations of $N$ cities being visited by a Traveling Salesman.
In this representation, each city is represented as a number $0..N-1$, being a solution a
vector of N integers (example: [0,2,3,1] means that solution starts from city 0, then follows to city 2,
then city 3, then city 1, and finally comes back to city 0). 
Objective is to find a route that minimizes distance between the $N$ visited cities.

We may define ESolutionTSP as a pair, containing a solution and its objective value (double).

.. code-block:: c++

    using ESolutionTSP = std::pair<
        std::vector<int>,  // first part of search space element: solution (representation)
        Evaluation<double> // second part of search space element: evaluation (objective value)
    >;


Problem Data
^^^^^^^^^^^^

We read a matrix of distances between pairs of cities (considering Euclidean distance), and
store in a structure named ProblemContext. Do not forget to #include helpers from optframe namespace, 
such as Matrix and Scanner.

.. code-block:: c++

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

Finally, we declare a global object (for simplicity), that includes all data.

.. code-block:: c++

    // Create TSP Problem Context
    ProblemContext pTSP;


Evaluation
^^^^^^^^^^^

Objective calculation can be done by using Functional Core class FEvaluator, which is a huge
simplification from OptFrame Core components Evaluator (for single objectives) and 
GeneralEvaluator (for single and multiple objectives).

.. code-block:: c++

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


Search method
^^^^^^^^^^^^^^^

We start now with the Biased Random Key Genetic Algorithm (BRKGA), a simple metaheuristic
inspired by classic Genetic Algorithm, using the solution representation of $n$ Random Keys, 
which are $[0,1]^n$ float values.

Random key generation
^^^^^^^^^^^^^^^^^^^^^

The BRKGA requires an initial solution generator, which is in this case, $n$ random [0,1] floats.
This can be done automatically by the method (since its trivial do generate $n$ [0,1] random numbers),
but we choose to demonstrate manually (by inheriting from OptFrame Core class Initial Population).

This is good to tune the degree of randomness (number of random digits) and also the random function used.

.. code-block:: c++

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

BRKGA decoding
^^^^^^^^^^^^^^^

BRKGA also requires a decoder function, that maps this array of random keys into a permutation.

This can be easily done with Functional Core class FDecodeRK, and an interesting approach based
on sorting the keys, related to a predefined indexing of each key.

.. code-block:: c++

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



BRKGA with TSP
^^^^^^^^^^^^^^^

We are ready to build a TSP instance with 3 cities with coordinates (10,10), (20,20) and (30,30),
and invoke a BRKGA to solve it.

The parameters of BRKGA are: decoding function, initial solution generator, population size, number of iterations,
also rates for mutation (randomness), elite (best solutions), preference for elite solutions, and finally, a random generation method.

.. code-block:: c++

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


The result from searchOut can be split in two parts, an error code and the returned solution 
(the same as in Simulated Annealing or any other OptFrame search method).


Complete Example
----------------

For a complete example, see folder Examples/FCore-BRKGA and execute :code:`bazel build ...`

.. warning::
    Feel free to check folder :code:`OptFrame/Examples` for other examples on FCore and OptFrame Classic.

