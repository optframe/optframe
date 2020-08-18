Introduction
=============


OptFrame is a framework for modeling and solving challenging optimization 
problems via (meta)-heuristic techniques.
It is developed in modern C++, aiming to provide both high computational 
efficiency and easy of use.
The project has started in 2008 at `Universidade Federal de Ouro Preto 
(UFOP) <https://www.ufop.br>`_ and considerably improved since then. Several master 
and PhD thesis have been developed on it, mostly at the Computing Institute 
of `Universidade Federal Fluminense (UFF) <http://www.ic.uff.br>`_. Its latest version 
v4 has been released in 2020, with the introduction of several functional 
programming features and C++11/14/17/20 capabilities.

OptFrame supports several state-of-the-art `metaheuristics 
<https://en.wikipedia.org/wiki/Metaheuristic>`_, such as:

- `Genetic Algorithm <https://en.wikipedia.org/wiki/Genetic_algorithm>`_ and Evolution Strategies
- `Simulated Annealing <https://en.wikipedia.org/wiki/Simulated_annealing>`_
- `Variable Neighborhood Search <https://en.wikipedia.org/wiki/Variable_neighborhood_search>`_ (VNS)
- `Iterated Local Search <https://en.wikipedia.org/wiki/Iterated_local_search>`_ (ILS)
- `Tabu Search <https://en.wikipedia.org/wiki/Tabu_search>`_
- `Greedy Randomized Adaptive Search Procedures <https://en.wikipedia.org/wiki/Greedy_randomized_adaptive_search_procedure>`_ (GRASP)

And also `multi-objective optimization <https://en.wikipedia.org/wiki/Multi-objective_optimization>`_ 
metaheuristics, such as NSGA-II.

The theoretical background behind OptFrame modeling will be described in details
in :doc:`concepts <../concepts>` section.
Every OptFrame component can be specialized for the problem at hand, to achieve maximum
performance during search.
This caused OptFrame users (up to v3) to have several files in a single project, that
required some extra setup to start new projects.
Since v4, we provide :doc:`OptFrame Functional Core (FCore) <../fcore>` abstractions, so that the intended
metaheuristic can be implemented in a few lines of C++ code.

If you have already :doc:`installed <../install>` OptFrame, you can jump to
:doc:`quick start <../quickstart>` to see how it works on practice.

Acknowledgements
-----------------

.. role::  raw-html(raw)
    :format: html

OptFrame has been developed with :raw-html:`&hearts;`, thanks to many contributors and
advices from brillant minds from academia.
Several researchers worldwide have contributed with ideas that represent the core of
OptFrame, specially: 

- Marcone Jamilson Freitas Souza *(for metaheuristic teaching and support)*
- Nenad Mladenovic *(for great works on neighborhood exploration)*
- Luiz Satoru Ochi *(for metaheuristic teaching and support)*
- El-Ghazali Talbi *(for great books on metaheuristics)*

Contributors
------------

Several contributors made this possible, please refer to the CONTRIBUTORS file on 
OptFrame project on `GitHub <https://github.com/optframe/optframe>`_.

This project is currently maintained by `Igor M. Coelho <https://github.com/igormcoelho>`_
and `Vitor N. Coelho <https://github.com/vncoelho>`_.

License
-------

Project is free, and source code is released under LGPLv3 copyleft license.

Some side projects such as Scanner++ are released under MIT License.

See complete :doc:`license <../license>`.
