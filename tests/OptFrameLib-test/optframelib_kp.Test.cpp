
#include <boost/ut.hpp>

// OptFrame C library headers
#include <OptFrameLib/OptFrameLib.h>

#include <OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp>
#include <OptFrame/modlog/modlog.hpp>
#include <OptFrameLib/FCoreApi1Engine.hpp>

#include "OptFrameLib/LibCTypes.h"

//
#include <OptFrame-Core-Examples/FCore-KP/KP-fcore-c-lib.hpp>
#include <OptFrame/Hyper/RunExperimentsCommand.hpp>

int main() {
  using namespace boost::ut;

  FakeEnginePtr engine =
      optframe_api1d_create_engine((int)modlog::LogLevel::Info);

  auto* eng = (FCoreApi1Engine*)engine;

  // manipulating seed from rand in engine
  eng->loader.factory.getRandGen()->setSeed(0);

  sref<KP_fcore::ProblemContext> problem{new KP_fcore::ProblemContext{}};
  Scanner scanner{"5\n260\n60 35 55 40 45\n120 70 110 80 90\n1000"};
  problem->load(scanner);
  problem->rg = eng->loader.factory.getRandGen();
  // get problem pointer as void*
  FakeProblemPtr p_ptr = (void*)(&problem.get());

  // testing initial solutions

  auto sol = KP_fcore::frandom(problem);
  // sol = vector(5) [1 , 0 , 1 , 0 , 0]

  "sol"_test = [sol] {
    expect(sol[0] == 1_i);
    expect(sol[1] == 0_i);
    expect(sol[2] == 1_i);
    expect(sol[3] == 0_i);
    expect(sol[4] == 0_i);
  };

  auto e = KP_fcore::fevaluate(problem, sol);

  "e"_test = [e] { expect(e.evaluation() == 115_i); };

  auto esol = ESolutionKP{sol, e};

  int idx_ev = optframe_api1d_add_evaluator(engine, fevaluate_c, false, p_ptr);

  "optframe_api1d_add_evaluator"_test = [idx_ev] { expect(idx_ev == 0_i); };

  int idx_c = optframe_api1d_add_constructive(engine, fconstructive_c, p_ptr,
                                              f_sol_deepcopy, f_sol_tostring,
                                              f_decref_solution);

  "optframe_api1d_add_constructive"_test = [idx_c] { expect(idx_c == 0_i); };

  int idx_is = optframe_api1d_create_initial_search(engine, idx_ev, idx_c);

  "optframe_api1d_create_initial_search"_test = [idx_is] {
    expect(idx_is == 0_i);
  };

  "random_move"_test = [problem, esol] {
    auto m = KP_fcore::fRandomFlip(problem, esol);
    expect(m->toString() == std::string("MoveBitFlip(k=1)"));
    expect(m->canBeApplied(esol));
  };

  int idx_ns =
      optframe_api1d_add_ns(engine, fnsrand_c, fmove_apply_c, fmove_eq_c,
                            fmove_cba_c, p_ptr, f_decref_move);

  "optframe_api1d_add_ns"_test = [idx_ns] { expect(idx_ns == 0_i); };

  int idx_nsseq = optframe_api1d_add_nsseq(
      engine, fnsrand_c, fIterator_c, fFirst_c, fNext_c, fIsDone_c, fCurrent_c,
      fmove_apply_c, fmove_eq_c, fmove_cba_c, p_ptr, f_decref_move);

  "optframe_api1d_add_nsseq"_test = [idx_nsseq] { expect(idx_nsseq == 0_i); };

  // =====================

  int idx_listns = optframe_api1d_create_component_list(
      engine, "[ OptFrame:NS 0 ]", "OptFrame:NS[]");

  "optframe_api1d_create_component_list"_test = [idx_listns] {
    expect(idx_listns == 0_i);
  };

  std::string prefix = "";  // default "OptFrame:" with warning
  int list_sz = optframe_api1d_engine_list_components(engine, prefix.c_str());

  "optframe_api1d_engine_list_components"_test = [list_sz] {
    expect(list_sz == 6_i);
  };

  int builders_sz = optframe_api1d_engine_list_builders(engine, prefix.c_str());

  "optframe_api1d_engine_list_builders"_test = [builders_sz] {
    expect(builders_sz == 34_i);
  };

  // =====================

  // test check module
  std::vector<double> vtest{1, 2, 3, 4, 5, 100.0};
  auto vtest2 =
      optframe::CheckCommand<FCoreLibESolution>::removeOutliersIQR(vtest);
  // vtest2: vector(5) [1 , 2 , 3 , 4 , 5]
  "test_outliers_removal_iqr"_test = [vtest2] { expect(vtest2.size() == 5_i); };

  bool expr = optframe_api1d_engine_check(
      engine, 10, 5, false, [](int, FakeEnginePtr eng) -> bool {
        // This enables debugging mode, if component supports it...
        // TODO: create an example where check fails and we verify the logs
        auto* engine = (FCoreApi1Engine*)eng;
        engine->experimentalParams["COMPONENT_LOG_LEVEL"] = "-1";
        engine->updateParameters();
        return true;
      });
  "optframe_api1d_engine_check"_test = [expr] { expect(expr); };

  return 1;
  // ==================================
  // Run some demonstration experiments

  /*
  optframe::RunExperimentsCommand<FCoreLibESolution> exp;
  std::vector<std::string> builders{
      "OptFrame:ComponentBuilder:GlobalSearch:SA:BasicSA "
      "OptFrame:GeneralEvaluator:Evaluator 0  OptFrame:InitialSearch 0 "
      "OptFrame:NS[] 0 0.99 100 99999"};
  int numRuns = 1;
  auto data =
      exp.run(numRuns, builders, eng->loader.factory, 5.0, std::nullopt);
*/

  int out_run = optframe_api1d_run_experiments(
      engine, 3,
      "OptFrame:ComponentBuilder:GlobalSearch:SA:BasicSA "
      "OptFrame:GeneralEvaluator:Evaluator 0  OptFrame:InitialSearch 0 "
      "OptFrame:NS[] 0 0.99 100 99999",
      0, "", 5.0, 4);

  // =====================

  bool b = optframe_api1d_destroy_engine(engine);

  "optframe_api1d_destroy_engine"_test = [b] { expect(b == 1_i); };

  return 0;
}