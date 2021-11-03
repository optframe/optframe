Advanced - CheckModule
=======================

We expand the knowledge from `Quick Start <../quickstart>`_ where the user
has learned how to `Install <../install>`_ OptFrame, and how to compile and
test metaheuristics for classic 0-1 Knapsack Problem (01KP) and Traveling Salesman Problem (TSP).

We now demonstrate how to quickly test the code and provide better move evaluations.


Multiple ways to evaluate a move 
--------------------------------

.. warning::
    At this point, we assume the reader is familiarized with the Traveling Salesman Problem,
    and is also familiarized with basic workings of OptFrame and OptFrame Functional Core.

We recall the example for the TSP, where a move operation called MoveSwap is proposed.

.. literalinclude:: ../../demo/03_QuickstartTSP_VNS_BRKGA/TSP-fcore-part5.hpp
    :linenos:
    :language: c++

Basic move cost calculation (revCost)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Typically, any basic move implementation will require at least one extra evaluation (on objective space) 
and two move applications (called revCost):

1. First apply move it to current solution, becoming a neighbor solution (prevent solution copy)
2. generating an Undo Move 
3. evaluating the cost at the neighbor solution
4. applying the Undo Move and come back to original solution
5. update objective value (by numeric copy)

.. literalinclude:: ../../demo/05_Advanced_TSP_checkmodule/TSP-fcore-part5-moveclass.hpp
    :linenos:
    :language: c++

.. important::
    We have included the filter :code:`canBeApplied` to limit the scope of valid moves,
    so as an :code:`operator==` to check for equality between moves.


Faster move cost calculation (fasterCost)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Step 3 from revCost is typically costly, what may be prevented by directly updating objective value at the same time the move is applied to solution.
This can be done by adopting the :code:`applyUpdate` operation on Move implementation.
Usually, this can be done by either:

1. Precomputing the cost value before move apply, and then apply it
2. Apply the move and then directly compute the updated objective value


.. literalinclude:: ../../demo/05_Advanced_TSP_checkmodule/TSP-fcore-part5-applyupdate.hpp
    :linenos:
    :language: c++


Constant cost calculation (cost)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Finally, an even faster approach is not directly compute cost of move, without even applying it to the solution.
The method :code:`cost()` may optionally return a calculated cost, if user wants to use that.

We observe that it is important to have some precise numerics here, such as :code:`Evaluation<int>`, 
instead of :code:`Evaluation<double>`. Cost recalculation may impose calculation errors (but if user
wants to do it anyway, OptFrame will not forbid it).

.. literalinclude:: ../../demo/05_Advanced_TSP_checkmodule/TSP-fcore-part5-cost.hpp
    :linenos:
    :language: c++



Using the CheckCommand to test structures
-----------------------------------------

One can easily check for the integrity of the structures, by creating an instance of
CheckCommand.

First, create it, then add all available components (NS, NSSeq, Evaluator, Constructive, ...).
Finally, invoke it passing two parameters: randomized pressure and iterative pressure.


.. literalinclude:: ../../demo/05_Advanced_TSP_checkmodule/main_checkmodule.cpp
    :linenos:
    :language: c++


An example of output can be seen here:

|    ---------------------------------
|    Solution	Time to clone a solution
|    ---------------------------------
|    title	#tests	avg(ms)	std(ms)
|    Solution	59502	0.0009	0.0014
|    ---------------------------------
|    all	-	0.0009	-
|
|    ---------------------------------
|    \|Constructive|=1	testing construction of initial solution
|    ---------------------------------
|    #id	title	#tests	avg(ms)	std(ms)
|    #0	OptFrame:InitialSearch	100	0.0181	0.0334
|    ---------------------------------
|    all	*	-	0.0181	-
|
|    ---------------------------------
|    \|Evaluators|=1	testing full evaluate(s) of a solution
|    ---------------------------------
|    #id	title	#tests	avg(ms)	std(ms)
|    #0	Direction:MIN	59602	0.0013	0.0008
|    ---------------------------------
|    all	*	-	0.0013	-
|
|    ---------------------------------
|    \|NS|=2	testing time of move apply(s) [apply no evaluation]
|    ---------------------------------
|    #id	title	#tests	avg(ms)	std(ms)
|    #0	OptFrame:FNS	41048	0.0008	0.0015
|    #1	OptFrame:FNSSeq	18454	0.0007	0.0008
|    ---------------------------------
|    all	*	-	0.0008	-
|
|    ---------------------------------
|    \|NS|=2	testing time of cost based on move apply(s) [revCost]
|    ---------------------------------
|    #id	title	#tests	avg(ms)	std(ms)
|    #0	OptFrame:FNS	20524	0.0054	0.0033
|    #1	OptFrame:FNSSeq	9227	0.0051	0.0018
|    ---------------------------------
|    all	*	-	0.0053	-
|
|    ---------------------------------
|    \|NS|=2	testing time of cost based on move apply(e, s) [fasterCost]
|    ---------------------------------
|    #id	title	#tests	avg(ms)	std(ms)
|    #0	OptFrame:FNS	20524	0.0020	0.0019
|    #1	OptFrame:FNSSeq	9227	0.0019	0.0011
|    ---------------------------------
|    all	*	-	0.0020	-
|
|    ---------------------------------
|    \|NS|=2	testing time of real cost based on move apply(e, s) - forcing allowCosts to False [realFasterCost]
|    ---------------------------------
|    #id	title	#tests	avg(ms)	std(ms)
|    #0	OptFrame:FNS	20524	0.0015	0.0029
|    #1	OptFrame:FNSSeq	9227	0.0013	0.0006
|    ---------------------------------
|    all	*	-	0.0014	-
|
|    ---------------------------------
|    \|NS|=2	testing time of move cost()
|    ---------------------------------
|    #id	title	#tests	avg(ms)	std(ms)
|    #0	OptFrame:FNS	20524	0.0044	0.0030
|    #1	OptFrame:FNSSeq	9227	0.0042	0.0021
|    ---------------------------------
|    all	*	-	0.0043	-

Note that cost() version is faster than others, for both neighborhoods (NS and NSSeq), as expected.

Error codes on CheckCommand 
^^^^^^^^^^^^^^^^^^^^^^^^^^^

CheckCommand may launch several errors, according to the table below:


.. code-block:: c++

   static const int CMERR_EV_BETTERTHAN = 2001;
   static const int CMERR_EV_BETTEREQUALS = 2002;
   static const int CMERR_MOVE_EQUALS = 3001;
   static const int CMERR_MOVE_HASREVERSE = 3002;
   static const int CMERR_MOVE_REVREV_VALUE = 3004;
   static const int CMERR_MOVE_REVSIMPLE = 3005;
   static const int CMERR_MOVE_REVFASTER = 3006;
   static const int CMERR_MOVE_REALREVFASTER = 3008;
   static const int CMERR_MOVE_COST = 3007;

Feel free to file an Issue on OptFrame Project in order to discuss how to handle them.

In the future, we intend to expand this section.


More Tricks on CheckCommand 
^^^^^^^^^^^^^^^^^^^^^^^^^^^

For a other examples, see folder Examples/FCore-BRKGA and execute :code:`bazel build ...`

.. warning::
    Feel free to check folder :code:`OptFrame/Examples` for other examples on FCore and OptFrame Classic.

