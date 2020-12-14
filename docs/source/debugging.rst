Debugging
=========

OptFrame provides several search methods that are ready to use, including classic metaheuristics.
Users may feel the need for debug messages during experimentation, and for this reason, we provide a complete debug/messaging system.

.. hint::
    Advanced users are recommended to write their own search methods, based on existing templates/examples. 
    This allows having more control over parameters and execution flow, so as introducing state-of-the-art high performance techniques.
    See section "Custom Methods" for more information.

Logs/Message System
--------------------

Every OptFrame component (including search methods) inherits from :code:`Component` class (see file :code:`Component.hpp`).
This allows automatic support for two types of logs: *user logs* and *machine logs*.

User Logs
^^^^^^^^^

User logs are commonly used for understanding the methods, so as tracking possible warning and errors.
There are five different levels (error, warning, information, verbose, debug): 

- error: execution errors to log stream (this is different from exception messaging on :code:`cerr`, which is typically disabled by :code:`-fno-exceptions`)
- warning: execution warnings to log stream
- information: typical informative messages (DEFAULT log level)
- verbose: excessive messaging (typically for debug purposes)
- debug: same as verbose, but completely disabled/removed when :code:`NDEBUG` flag is active (good for performance)

Typically, *information* level is enabled, presenting restricted (but useful) search-related information.
Examples are: best solution improvements (only value is presented); initial/final results of search method.

.. hint::
    When designing new methods, avoid printing complete solutions, only print its value (or values, when multi-objective).
    The reason is that solution structures tend to get bigger and complex to read, being mostly useless for debug purposes.
    For recording solutions, one may easily customize event callbacks (see section "Custom Callbacks").

To activate verbose/debug logs recursively (from a component into all its sub-components), use method :code:`setVerboseR()`.

.. code-block:: c++

    auto mySearchMethod = ...;    // maybe a Simulated Annealing?
    mySearchMethod.setVerboseR(); // sets verbose/debug level to all sub-components
    // check that debug level is activated (will print 'debug=1')
    std::cout << "debug=" << mySearchMethod.debug << std::endl;

To activate a specific log level, to some specific component, just use method :code:`setMessageLevel(...)`:

.. code-block:: c++

    auto mySearchMethod = ...;    // maybe a Simulated Annealing?
    mySearchMethod.setMessageLevel(LogLevel::Silent); // no logs
    // check that debug level is inactive (will print 'debug=0')
    std::cout << "debug=" << mySearchMethod.debug << std::endl;

On the other hand, one may want to completely disable all logs for all sub-components (maybe some script execution?):

.. code-block:: c++

    auto mySearchMethod = ...;    // maybe a Simulated Annealing?
    mySearchMethod.setSilentR();  // no logs for component and all sub-components
    // check that debug level is inactive (will print 'debug=0')
    std::cout << "debug=" << mySearchMethod.debug << std::endl;

User may also choose the stream for logs (DEFAULT is :code:`std::cout`):

.. code-block:: c++

    auto mySearchMethod = ...;          // maybe a Simulated Annealing?
    mySearchMethod.setLogR(&std::cout); // log stream for component and all sub-components

.. hint::
    Fine-grained components should not provide information logs, otherwise log system could be quickly overloaded.
    User is welcome to implement its own logs (using stream :code:`(*Component::logdata)`) during development of its components.

.. warning::
    Not all search components are currently designed to support all suitable message types and custom log stream.
    Some messages may currently leak through :code:`std::cout` or :code:`std::cerr` (feel free to open an Issue or PR in github).


Machine Logs
^^^^^^^^^^^^

An interesting feature of OptFrame components is support for structured (context-aware) machine logs.
Typical machine logs are: list of best values and iteration/time when it was found; launch configuration of search method.

User may push machine logs through standard streams (such as :code:`std::cout`) or specific files.
However, it is useful to add some *context* to the logging, typically *plain text* or *json*.

.. warning::
    Machine logs are disabled by default.

.. code-block:: c++

    auto mySearchMethod = ...;          // maybe a Simulated Annealing?
    std::ofstream fout("output.txt");   // creates 'output.txt' file
    // send 'ctxt' semantic stream outputs to designated file
    optframe::ctxt.setStream(fout);     // redirects 'ctxt' plain text stream to file
    // sets machine logs through plain text stream
    mySearchMethod.mlog = &optframe::ctxt;

.. hint::
    User can also select json context stream :code:`optframe::cjson` as output.
    There are also plans to support CSV format as :code:`optframe::ccsv`, but we need some help on that...

.. warning::
    Context log streams depend on each implementation of the search methods/components.
    Currenty, few methods support several context standards, so *plain text* should be preferred (at first!).



Custom Callbacks
----------------

There are two interesting callbacks to customize search methods:

- onBest: activated when a new *best* is found
- onIncumbent: activated when *incument* has changed

We intentionally avoid the word "solution" here, since "best" may be some "best solution" or "best Pareto Set" for multi-objective.
This is the same for "incumbent", as it may be "incumbent solution", "incumbent population" or something else.

Here's an example for *onBest*, for some single-objective trajectory-based method:

.. code-block:: c++

    auto mySearchMethod = ...;          // maybe a Simulated Annealing?
    mySearchMethod.onBest = [](auto& self) 
    {
        // logging solution to user logs 
        (*self.logdata) << "My method has improved best solution! Print solution:\t" << self.best->first << std::endl;
        // logging solution to machine logs
        (*self.mlog) << "solution\t" << self.best->first << std::endl;
        // fine-tuning stop criteria according to some solution characteristic (or objective value)
        return self.best->second.evaluation() > 9500.0; // halts if less or equals to 9500.0 (minimization problem)
    };

Callbacks allow search to be stopped (thus also acting as a custom stop criteria): returning false should halt execution.

.. danger::
    This section is incomplete.

Custom Methods 
--------------

User may implement a new search method by inheriting from :code:`GlobalSearch` class 
and implementing :code:`search` method.

.. danger::
    This section is incomplete.

