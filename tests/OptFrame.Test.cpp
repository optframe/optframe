
// collection of 'OptFrame' core tests

//#define CATCH_CONFIG_MAIN // This tells Catch to provide a main()
#ifdef MAKE
#include <catch2/catch_amalgamated.hpp>
#else
#include <catch2/catch_test_macros.hpp>
#endif

#include "./OptFrame-test/Helper-test/MultiESolution.Test.hpp"
#include "./OptFrame-test/Heuristics-test/HeuristicsAll.Test.hpp"
#include "./OptFrame-test/Hyper-test/HeuristicFactory.Test.hpp"
#include "./OptFrame-test/Matrix.Test.hpp"
#include "./OptFrame-test/ParetoDominance.Test.hpp"
#include "./OptFrame-test/RandGen.Test.hpp"
#include "./OptFrame-test/Util-test/Util.Test.hpp"

// good
