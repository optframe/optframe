Advanced
========

Some advanced tricks and optimization techniques.

.. danger::
    This section is incomplete!


Explicit XESolution 
^^^^^^^^^^^^^^^^^^^

Instead of using standard :code:`std::pair` to glue solution and evaluation types,
one can explictly declare a class. This is useful to embed user methods and other
problem-specific operations.


.. code-block:: c++

    // explicit declaration of class to represent XESolution pair
    class MyESolution1
    {
    public:
        using first_type = std::vector<bool>;
        using second_type = Evaluation<double>;
        first_type first;   // some XSolution type
        second_type second; // some XEvaluation type
        // other specific methods ...
    }

    // declaration of XESolution pair via inheritance over IESolution
    class MyESolution2 : public IESolution<std::vector<bool>, Evaluation<double>>
    {
        // other specific methods ...
    }


Local Search
^^^^^^^^^^^^

OptFrame has three main types of neighborhoods: :code:`NS`, :code:`NSSeq` and :code:`NSEnum`.

Advanced local search, ...


Move Cost
^^^^^^^^^

.. hint::
    On OptFrame Classic (not FCore) it's possible to avoid the creation of an undo move. Currently, all 
    existing examples on OptFrame Classic use the undo feature anyway, since move undo is typically much faster than a complete copy of
    the solution structure.


Multi-Objective
^^^^^^^^^^^^^^^

For multiple objectives, ...
