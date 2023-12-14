Simulated Annealing metaheuristic
=================================

General Concepts and Theory
---------------------------

Simulated Annealing is a popular metaheuristic, proposed by Kirkpatrick et al. in 1983::

    S. Kirkpatrick, D.C. Gellat, and M.P. Vecchi. Optimization by Simulated
    Annealing. Science, 220:671-680, 1983.

To learn more, we recommend the following classic books:

- Handbook of Metaheuristics
- Metaheuristics: from Design to Implementation

In Portuguese, we recommend the textbook by prof. Marcone Jamilson Freitas Souza.

Simulated Annealing pseudocode
------------------------------

1. Classic Definition in Literature
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

- :math:`f(.)`: objective function (minimization)
- :math:`\mathcal{N}(.)`: neighborhood structure
- :math:`\alpha`: cooling factor
- :math:`SAmax`: max number of iterations per temperature
- :math:`Ti`: initial temperature
- :math:`\xi(.)`: random number generator
- :math:`stop(.)`: stop criteria
- :math:`s`: initial solution


Pseudocode for Classic Version in Literature
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: html

    .. pcode::
        :linenos:

        \begin{algorithm}
        \caption{SA}
        \begin{algorithmic}
        \Procedure{SA}{$f(.)$, $\mathcal{N}(.)$, $\alpha$, $SAmax$, $Ti$, $s$, $\xi(.)$}
            \State $s^* \gets s$
            \State $IterT \gets 0$
            \State $T \gets Ti$
            \While{$T > 0$ \textbf{and} \textbf{not} $stop(s)$}
            \While{$IterT < SAmax$ \textbf{and} \textbf{not} $stop(s)$}
                \State $IterT \gets IterT+1$
                \State $s' \gets \mathcal{N}^{ANY}_j(s)$
                \State $\Delta \gets f(s') - f(s)$
                \If{$\Delta < 0$}
                    \State s  \gets s'$
                    \If{$f(s) < f(s^*)$}
                        \State $s^* \gets s'$
                    \EndIf
                \Else
                    \State $x \gets \xi^{\mathbb{R}}(0, 1)$
                    \If{$x < e^{\frac{-\Delta}{T}}$}
                        \State $s \gets s'$
                    \EndIf
                \EndIf
            \EndWhile
                \State $T \gets \alpha \times T$
                \State $IterT \gets 0$
            \EndWhile
            \State \textbf{return} $NO\_REPORT, s^*$
        \EndProcedure
        \end{algorithmic}
        \end{algorithm}



BasicSimulatedAnnealing
-----------------------

1. Main Definition
~~~~~~~~~~~~~~~~~~

Parameters
^^^^^^^^^^

- :math:`stop(.)`: reference to StopCriteria component
- :math:`g(.)`: shared reference to GeneralEvaluator component
- :math:`initsol()`: shared reference to InitialSearch component
- :math:`\mathcal{N}_k(.)`: list of shared references to NS component
- :math:`\alpha`: cooling factor (double)
- :math:`SAmax`: max number of iterations per temperature (int)
- :math:`Ti`: initial temperature (double)
- :math:`\xi(.)`: shared reference to RandGen component

Pseudocode for Main Version
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. only:: html

    .. pcode::
        :linenos:

        \begin{algorithm}
        \caption{BasicSimulatedAnnealing}
        \begin{algorithmic}
        \Procedure{BasicSimulatedAnnealing}{$stop(.)$, $g(.)$, $initsol()$, $\mathcal{N}_k(.)$, $\alpha$, $SAmax$, $Ti$, $\xi(.)$}
            \State $\langle s,e \rangle  \gets initsol(stop)$
            \If{$\not\exists \langle s,e \rangle $}
                \State \textbf{return} $NO\_SOLUTION$, $\langle \rangle$
            \EndIf
            \State $\langle s^*,e^*\rangle \gets \langle s,e \rangle$
            \State $T \gets Ti$
            \State $iterT \gets 0$
            \While{$T \geq 0.0001$ \textbf{and} \textbf{not} $stop(e^*)$}
                \State $j \gets \xi^{\mathbb{Z}}(0, k-1)$
                \State $m \gets \mathcal{N}^{ANY}_j( \langle s,e\rangle  )$
                \If{$\not\exists m$}
                    \State \textbf{return} $EARLY\_STOP, \langle s^*, e^*\rangle$
                \EndIf
                \State $\langle s_1, e_1\rangle  \gets \langle s,e\rangle $
                \State $\langle s_1', e_1^\circ\rangle, \bar m  \gets m \oplus \langle s_1,e_1\rangle $
                \State $\langle s_1', e_1'\rangle  \gets g( \langle s_1', e_1^\circ \rangle )$
                \If{$g_<(e_1', e_1)$}
                    \State $\langle s,e\rangle  \gets \langle s_1', e_1'\rangle $
                    \If{$g_<(e, e^*)$}
                        \State $\langle s^*,e^*\rangle  \gets \langle s, e\rangle $
                    \EndIf
                \Else
                    \State $x \gets \xi^{\mathbb{R}}(0, 1)$
                    \State $\Delta \gets |e_1' - e|$
                    \If{$x < e^{\frac{-\Delta}{T}}$}
                        \State $\langle s,e\rangle  \gets \langle s_1', e_1'\rangle $
                    \EndIf
                \EndIf
                \If{$iterT < SAmax$}
                \State $iterT \gets iterT + 1$
                \Else
                \State $iterT \gets 0$
                \State $T \gets \alpha \cdot T$
                \EndIf
            \EndWhile
            \State \textbf{return} $NO\_REPORT, \langle s^*, e^*\rangle $
        \EndProcedure
        \end{algorithmic}
        \end{algorithm}

.. only:: latex

   .. raw:: latex

        \begin{algorithm}
        \caption{BasicSimulatedAnnealing}
        \begin{algorithmic}
        \Procedure{BasicSimulatedAnnealing}{$stop(.)$, $g(.)$, $initsol()$, $\mathcal{N}_k(.)$, $\alpha$, $SAmax$, $Ti$, $\xi(.)$}
            \State $\langle s,e \rangle  \gets initsol(stop)$
            \If{$\not\exists \langle s,e \rangle $}
                \State \textbf{return} $NO\_SOLUTION$, $\langle \rangle$
            \EndIf
            \State $\langle s^*,e^*\rangle \gets \langle s,e \rangle$
            \State $T \gets Ti$
            \State $iterT \gets 0$
            \While{$T \geq 0.0001$ \textbf{and} \textbf{not} $stop(e^*)$}
                \State $j \gets \xi^{\mathbb{Z}}(0, k-1)$
                \State $m \gets \mathcal{N}^{ANY}_j( \langle s,e\rangle  )$
                \If{$\not\exists m$}
                    \State \textbf{return} $EARLY\_STOP, \langle s^*, e^*\rangle$
                \EndIf
                \State $\langle s_1, e_1\rangle  \gets \langle s,e\rangle $
                \State $\langle s_1', e_1^\circ\rangle, \bar m  \gets m \oplus \langle s_1,e_1\rangle $
                \State $\langle s_1', e_1'\rangle  \gets g( \langle s_1', e_1^\circ \rangle )$
                \If{$g_<(e_1', e_1)$}
                    \State $\langle s,e\rangle  \gets \langle s_1', e_1'\rangle $
                    \If{$g_<(e, e^*)$}
                        \State $\langle s^*,e^*\rangle  \gets \langle s, e\rangle $
                    \EndIf
                \Else
                    \State $x \gets \xi^{\mathbb{R}}(0, 1)$
                    \State $\Delta \gets |e_1' - e|$
                    \If{$x < e^{\frac{-\Delta}{T}}$}
                        \State $\langle s,e\rangle  \gets \langle s_1', e_1'\rangle $
                    \EndIf
                \EndIf
                \If{$iterT < SAmax$}
                \State $iterT \gets iterT + 1$
                \Else
                \State $iterT \gets 0$
                \State $T \gets \alpha \cdot T$
                \EndIf
            \EndWhile
            \State \textbf{return} $NO\_REPORT, \langle s^*, e^*\rangle $
        \EndProcedure
        \end{algorithmic}
        \end{algorithm}

SearchStatus return codes
^^^^^^^^^^^^^^^^^^^^^^^^^

There are return codes being currently used: :math:`NO\_SOLUTION`, :math:`EARLY\_STOP` and :math:`NO\_REPORT`.
The return :math:`EARLY\_STOP` will trigger warnings.

Primary and Secondary search spaces
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

BasicSimulatedAnnealing is a *trajectory-based single objective global search* method:

- The primary search space (best type) XSH is *XESSolution*, where its base type XES is also *XESSolution*.
- The secondary search space (incumbent type) XSH2 is *XESSolution*, where its base type XES2 is also *XESSolution*.

This occurs since BasicSimulatedAnnealing inherits from SingleObjSearch, 
that constraints its *XESolution* space for single objective *XESSolution*,
and also ITrajectory, that requires XSH=XSH2.

To better understand these notations, see `Concepts <./concepts.html>`_



Primary ComponentBuilder string syntax
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

One may build BasicSimulatedAnnealing on C++ by using its constructors from `BasicSimulatedAnnealing.hpp` header file.

It belongs to **SA** family and its Component Builder inherits from `GlobalSearchBuilder`, 
so a common way to find it (e.g. in OptFrame Python), is to use::

    your_problem.engine.list_builders(":BasicSA")

The component builder string identifier for BasicSimulatedAnnealing is::

    "OptFrame:ComponentBuilder:GlobalSearch:SA:BasicSA"

Expected arguments are::

    OptFrame:ComponentBuilder:GlobalSearch:SA:BasicSA |params|=6
        param 0 => OptFrame:GeneralEvaluator:Evaluator : evaluation function
        param 1 => OptFrame:InitialSearch : constructive heuristic
        param 2 => OptFrame:NS[] : list of NS
        param 3 => OptFrame:double : cooling factor
        param 4 => OptFrame:int : number of iterations for each temperature
        param 5 => OptFrame:double : initial temperature

The **Default Domain** for BasicSimulatedAnnealing component is :code:`"<XESf64>"` 
(single solutions on search space with 64 bits floating-point on objective space), 
as inherited from *GlobalSearch* and *SingleObjSearch*.

Example of string syntax
^^^^^^^^^^^^^^^^^^^^^^^^

A simple example could be::

    "OptFrame:GeneralEvaluator:Evaluator 0 OptFrame:InitialSearch 0 OptFrame:NS[] 0 0.98 1000 999999"

See Examples folder for real examples on C++ and OptFrame Python examples for using component builder string syntax.

1. Helpers
~~~~~~~~~~

Simulated Annealing family includes a special method to estimate the 
initial temperature :code:`estimateInitialTemperature`.

This method is found in textbook by prof. Marcone Jamilson Freitas Souza (In Portuguese).


3. Extended Versions and Callbacks
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

One may build extended versions of BasicSimulatedAnnealing, 
by configuring its callbacks and using alternative component builders.

SearchContext
^^^^^^^^^^^^^

BasicSimulatedAnnealing defines a SearchContext called SearchContextSA, 
with the following data:

- `BasicSimulatedAnnealing<XES>& self``: reference to self (to get parameters)
- `double T`: current temperature
- `int iterT`: current iteration (per temperature)

Must double check these in the future (unstable to use):

- `std::optional<XES>& best`: reference to best solution, if exists
- `std::optional<XES>& incumbent`: reference to incumbent solution, if exists

BasicSimulatedAnnealing allows manipulation of its SearchContextSA in callbacks,
in order to change/personalize its search behavior.

Pseudocode for Extended Version
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The pseudocode below details the extension possibilities on BasicSimulatedAnnealing.

.. only:: html
        
    .. pcode::
        :linenos:

        \begin{algorithm}
        \caption{BasicSimulatedAnnealingCallbacks}
        \begin{algorithmic}
        \Procedure{BasicSimulatedAnnealingCallbacks}{$stop(.)$, $g(.)$, $initsol()$, $\mathcal{N}_k(.)$, $Ti$, $\xi(.)$, $onBest(.)$, $onIncumbent(.)$, $onLoop(.)$, $onBeforeLoop(.)$}
            \State $\langle s,e \rangle  \gets initsol(stop)$
            \If{$\not\exists \langle s,e \rangle $}
                \State \textbf{return} $NO\_SOLUTION$, $\langle \rangle$
            \EndIf
            \State $onIncumbent(\langle s,e\rangle)$
            \State $\langle s^*,e^*\rangle \gets \langle s,e \rangle$
            \State $onBest(\langle s^*,e^*\rangle)$
            \State $context.T \gets Ti$
            \State $context.iterT \gets 0$
            \While{$onLoop(context, stop)$}
                \State $j \gets \xi^{\mathbb{Z}}(0, k-1)$
                \State $m \gets \mathcal{N}^{ANY}_j( \langle s,e\rangle  )$
                \If{$\not\exists m$}
                    \State \textbf{return} $EARLY\_STOP, \langle s^*, e^*\rangle$
                \EndIf
                \State $\langle s_1, e_1\rangle  \gets \langle s,e\rangle $
                \State $\langle s_1', e_1^\circ\rangle, \bar m  \gets m \oplus \langle s_1,e_1\rangle $
                \State $\langle s_1', e_1'\rangle  \gets g( \langle s_1', e_1^\circ \rangle )$
                \If{$g_<(e_1', e_1)$}
                    \State $\langle s,e\rangle  \gets \langle s_1', e_1'\rangle $
                    \State $onIncumbent(\langle s,e\rangle)$
                    \If{$g_<(e, e^*)$}
                        \State $\langle s^*,e^*\rangle  \gets \langle s, e\rangle $
                        \State $onBest(\langle s^*,e^*\rangle)$
                    \EndIf
                \Else
                    \State $x \gets \xi^{\mathbb{R}}(0, 1)$
                    \State $\Delta \gets |e_1' - e|$
                    \If{$x < e^{\frac{-\Delta}{T}}$}
                        \State $\langle s,e\rangle  \gets \langle s_1', e_1'\rangle $
                        \State $onIncumbent(\langle s,e\rangle)$
                    \EndIf
                \EndIf
                \State $context \gets onBeforeLoop(context)$
            \EndWhile
            \State \textbf{return} $NO\_REPORT, \langle s^*, e^*\rangle $
        \EndProcedure
        \end{algorithmic}
        \end{algorithm}

.. only:: latex

   .. raw:: latex

        \begin{algorithm}
        \caption{BasicSimulatedAnnealingCallbacks}
        \begin{algorithmic}
        \Procedure{BasicSimulatedAnnealingCallbacks}{$stop(.)$, $g(.)$, $initsol()$, $\mathcal{N}_k(.)$, $Ti$, $\xi(.)$, $onBest(.)$, $onIncumbent(.)$, $onLoop(.)$, $onBeforeLoop(.)$}
            \State $\langle s,e \rangle  \gets initsol(stop)$
            \If{$\not\exists \langle s,e \rangle $}
                \State \textbf{return} $NO\_SOLUTION$, $\langle \rangle$
            \EndIf
            \State $onIncumbent(\langle s,e\rangle)$
            \State $\langle s^*,e^*\rangle \gets \langle s,e \rangle$
            \State $onBest(\langle s^*,e^*\rangle)$
            \State $context.T \gets Ti$
            \State $context.iterT \gets 0$
            \While{$onLoop(context, stop)$}
                \State $j \gets \xi^{\mathbb{Z}}(0, k-1)$
                \State $m \gets \mathcal{N}^{ANY}_j( \langle s,e\rangle  )$
                \If{$\not\exists m$}
                    \State \textbf{return} $EARLY\_STOP, \langle s^*, e^*\rangle$
                \EndIf
                \State $\langle s_1, e_1\rangle  \gets \langle s,e\rangle $
                \State $\langle s_1', e_1^\circ\rangle, \bar m  \gets m \oplus \langle s_1,e_1\rangle $
                \State $\langle s_1', e_1'\rangle  \gets g( \langle s_1', e_1^\circ \rangle )$
                \If{$g_<(e_1', e_1)$}
                    \State $\langle s,e\rangle  \gets \langle s_1', e_1'\rangle $
                    \State $onIncumbent(\langle s,e\rangle)$
                    \If{$g_<(e, e^*)$}
                        \State $\langle s^*,e^*\rangle  \gets \langle s, e\rangle $
                        \State $onBest(\langle s^*,e^*\rangle)$
                    \EndIf
                \Else
                    \State $x \gets \xi^{\mathbb{R}}(0, 1)$
                    \State $\Delta \gets |e_1' - e|$
                    \If{$x < e^{\frac{-\Delta}{T}}$}
                        \State $\langle s,e\rangle  \gets \langle s_1', e_1'\rangle $
                        \State $onIncumbent(\langle s,e\rangle)$
                    \EndIf
                \EndIf
                \State $context \gets onBeforeLoop(context)$
            \EndWhile
            \State \textbf{return} $NO\_REPORT, \langle s^*, e^*\rangle $
        \EndProcedure
        \end{algorithmic}
        \end{algorithm}

Callbacks
^^^^^^^^^

There are four **generic callbacks** available on extended versions of simulated annealing:

- *onBest*: from GlobalSearch
- *onIncumbent*: from ITrajectory
- *onLoop*: from ILoop
- *onBeforeLoop*: from ILoop

The *onBest* and *onIncumbent* are generic callbacks that work on current solution.
The *onLoop* and *onBeforeLoop* from ILoop can be better explored as *specific callbacks*.

The are four **specific callbacks** implemented: *onBestCtx*, *onIncumbentCtx*, *onLoopCtx* 
and *onBeforeLoopCtx*.

By overriding *onLoopCtx* and *onBeforeLoopCtx* one may manipulate SearchContextSA,
for example, to implement alternative cooling schemes for Simulated Annealing.


Alternative Parameters
^^^^^^^^^^^^^^^^^^^^^^

Some possibilities may appear only in C++ constructors,
such as passing a single neighborhood instead of a list.

.. important::
    The **searchBy** method inherited from *GlobalSearch* allows directly passing a primary XESolution element,
    thus *overriding the initsol() component*.


.. warning::
    This section is still incomplete!
