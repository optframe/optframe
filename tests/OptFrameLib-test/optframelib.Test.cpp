
#include <boost/ut.hpp>

// OptFrame C library headers
#include <OptFrameLib/OptFrameLib.h>

#include <OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp>
#include <OptFrame/modlog/modlog.hpp>

#include "OptFrameLib/LibCTypes.h"

// directly testing optframe C library (static)

double fevaluate_c(FakeProblemPtr p_ptr, FakeSolutionPtr s_ptr) {
  using PKP = KP_fcore::ProblemContext;
  auto* v = (std::vector<bool>*)(s_ptr);
  std::shared_ptr<PKP> problem_view{(PKP*)p_ptr, [](PKP*) {}};
  sref<PKP> problem{problem_view};
  auto e = KP_fcore::fevaluate(problem, *v);
  return e.evaluation();
}

int main() {
  using namespace boost::ut;

  FakeEnginePtr engine =
      optframe_api1d_create_engine((int)modlog::LogLevel::Info);

  optframe_api0d_engine_welcome(engine);

  sref<KP_fcore::ProblemContext> problem{new KP_fcore::ProblemContext{}};
  Scanner scanner{"5\n260\n60 35 55 40 45\n120 70 110 80 90\n1000"};
  problem->load(scanner);
  problem->rg->setSeed(0);

  auto sol = KP_fcore::frandom(problem);

  "sol"_test = [sol] {
    expect(sol[0] == 1_i);
    expect(sol[1] == 0_i);
    expect(sol[2] == 1_i);
    expect(sol[3] == 0_i);
    expect(sol[4] == 0_i);
  };

  auto e = KP_fcore::fevaluate(problem, sol);

  "e"_test = [e] { expect(e.evaluation() == 115_i); };

  FakeProblemPtr p_ptr = (void*)(&problem.get());

  int idx_ev = optframe_api1d_add_evaluator(engine, fevaluate_c, false, p_ptr);

  "optframe_api1d_add_evaluator"_test = [idx_ev] { expect(idx_ev == 0_i); };

  optframe_api1d_destroy_engine(engine);

  return 0;
}