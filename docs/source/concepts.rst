Concepts
=========

OptFrame is a framework for optimization, specially focused on heuristics and metaheuristics.
Some works were published in literature with theoretical background, please visit OptFrame GitHub repository.

OptFrame supports many types of optimization strategies, such as mono and multi objective,
and several different metaheuristic classes, such as trajectory and population based.
Each of these techniques have different theories behind it, 
but OptFrame manages to put all of them together in a concise framework.

In this sense, we define a Search (or Exploration) Space in OptFrame as a pair composed by a
Solution (or Representation) Space (called **XSolution**) and 
an Evaluation (or Objective) Space (called **XEvaluation**).
Together, they form a **XESolution** space, also denoted by the pair **(XSolution,XEvaluation)**.

There are several types of search spaces, depending on the solution strategy employed.
The **XESSolution** enforces that the Evaluation Space is of single-objective kind **XSEvaluation**,
thus enforcing a *total order* over the *objType* behind the evaluation type.

.. hint::
    By default, :code:`XSEvaluation::objType` defaults to 64-bit floating point, known as *double* type.
    This, and other things can be changed (for example, to *int* or *long long*) if necessary!

So, **XESSolution** denotes the pair **(XSolution, XSEvaluation)**.

.. hint::
    Most CXX Concepts in OptFrame begin with letter **X**, to not confuse with standard classes.

On the other hand, the **XEMSolution** space denotes the pair **(XSolution, XMEvaluation)**,
where **XMEvaluation** requires multiple elements of *total order* to exist, like :math:`2^{XEvaluation}`.
This is very useful in multi-objective problems and it does not constrain the type of every
individual **XEvaluation** element, as long as they individually form a *total order* (for
example, one may be an *int*, while the other is a *double*).

.. hint::
    On practice, basic implementations such as the *MultiEvaluation* assume a single type
    for :code:`XSEvaluation::objType`, defaulting to 64-bit floating point, known as *double* type.
    This makes it simpler to code multi-objective metaheuristics, but does not exclude the
    possibility of adopting different types as a tuple, if necessary.

Some spaces contain a `Power Set`_ of
other spaces, such as the **X2Solution**, which is equivalent to a set of solutions of type :math:`2^{XSolution}`,
and the **X2ESolution**, which is a set of **XESolution** pairs, like :math:`2^{XESolution}`.

.. _Power Set: https://en.wikipedia.org/wiki/Power_set

.. hint::
    One can regard a **X2Solution** as a *population of individuals*, in Genetic Algorithm notation,
    and a **X2ESolution** as a *population of individuals together with their fitness values*.

Finally, a **X2EMSolution** denotes a set of **EMSolution** spaces, like :math:`2^{XEMSolution}`.

.. hint::
    One can regard a **X2EMSolution** as a *Pareto Set together with a Pareto Front*,
    in multi objective literature notation.

Using this process, one may define even more complex spaces, if necessary.
For more details, check C++ file *BaseConcepts.hpp* (this requires C++20 CXX Concepts).

Domains
-------

Besides the concepts presented before, OptFrame also supports Default Domains,
that are practical implementations of the spaces described before.
An interesting example is for BRKGA metaheuristic, that only allows individuals/solutions
to be represented as a vector of 01-real numbers, called *random-keys*.
So, in order to detect if a component is really compatible with other, it is necessary to
check if those are operating under the same domain.
By default, the Default Domain of a component does not appear in its name, only appearing
(with angle backet notation) if the component is operating outside its default domain. 

Function *getNamedDomain* on file *Domain.hpp* shows all available default domains in OptFrame::

  template <typename X>
  constexpr static std::string_view getNamedDomain() {
    if constexpr (is_rkf64<X>::value) return "<XRKf64>";
    if constexpr (is_rkf32<X>::value)
      return "<XRKf32>";
    else if constexpr (is_XRKf64Ef64<X>::value)
      return "<XRKf64Ef64>";
    else if constexpr (is_X2RKf64Ef64<X>::value)
      return "<X2RKf64Ef64>";
    else if constexpr (is_XESf64<X>::value)
      return "<XESf64>";
    else if constexpr (is_XESf32<X>::value)
      return "<XESf32>";
    else if constexpr (is_XESi64<X>::value)
      return "<XESi64>";
    else if constexpr (is_X2ESf64<X>::value)
      return "<X2ESf64>";
    else if constexpr (is_XMESf64<X>::value)
      return "<XMESf64>";
    else if constexpr (is_X2MESf64<X>::value)
      return "<X2MESf64>";
    else if constexpr (XESolution<X>)
      return "<XES>";
    else if constexpr (is_X2S<X>::value)
      return "<X2S>";
    else if constexpr (XSolution<X>)
      return "<XS>";
    return "";
  }

Briefly, these are the supported default domains:

- :code:`"<XS>"`: only solution is supported (no evaluation)
- :code:`"<X2S>"`: only multi solutions (or population) is supported (no evaluation)
- :code:`"<XES>"`: both solution and evaluation is supported
- :code:`"<XESf64>"`: solution and evaluation with 64-bit floating-point is supported
- :code:`"<XESf32>"`: solution and evaluation with 32-bit floating-point is supported
- :code:`"<XESi64>"`: solution and evaluation with 64-bit integer is supported
- :code:`"<X2ESf64>"`: population of :code:`XESf64` elements
- :code:`"<XMESf64>"`: both solution and multi evaluation with 64-bit floating-point is supported
- :code:`"<X2MESf64>"`: population of :code:`XMESf64` elements
- :code:`"<XRKf32>"`: only solution of random-keys with 32-bit floating-point (no evaluation)
- :code:`"<XRKf64Ef64>"`: only solution of random-keys with 64-bit floating-point and also 64-bit evaluation
- :code:`"<X2RKf64Ef64>"`: only population of :code:`XRKf64Ef64` elements

This can (CERTAINLY) look confusing, so don't worry too much about it!
It is not common to have these, specially for common mono objective optimization problems.

In the rare occasions where such domain appears in the name of a component, 
it will certainly help you observing the coherent interactions of different spaces/domain in
a single unified framework. This is the greatest power of OptFrame!
