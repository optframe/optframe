Advanced
========

Some advanced tricks.

Explicit XESolution pair declaration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Instead of using standard :code:`std::pair` to glue solution and evaluation types,
one can explictly declare a class. This is useful to embed user methods and other
problem-specific operations.


.. code-block:: c++

    // explicit declaration of class to represent XESolution pair
    class MyESolution1
    {
        using first_type = std::vector<bool>;
        using second_type = Evaluation<double>;
        first_type first;   // some XSolution type
        second_type second; // some XEvaluation type
        // other specific methods
    }

    // declaration of XESolution pair via inheritance over IESolution
    class MyESolution2 : public IESolution<std::vector<bool>, Evaluation<double>>
    {
        // other specific methods
    }
