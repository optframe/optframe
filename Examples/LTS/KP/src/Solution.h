#ifndef KP_SOLUTION_H_
#define KP_SOLUTION_H_

#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Helper/Solutions/CopySolution.hpp>

#include "Representation.h"

using namespace optframe;

// int (?)
typedef OPTFRAME_DEFAULT_ADS MY_ADS;

//typedef CopySolution<RepKP, MY_ADS> SolutionKP;

typedef RepKP SolutionKP;  // using representation as solution

using ESolutionKP = pair<SolutionKP, EvaluationKP>;

struct test_kp_solution_copy_solution {
  void f() {
    RepKP rep;
    CopySolution<RepKP> sol(rep);

    // TODO: only if default ADS is nullptr_t on CopySolution
    nullptr_t defads;
    CopySolution<RepKP> sol2(rep, defads);
  }
};

#endif /*KP_SOLUTION_H_*/
