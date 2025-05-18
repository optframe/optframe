
#include <boost/ut.hpp>

// OptFrame C library headers
#include <OptFrameLib/OptFrameLib.h>

#include <OptFrame-Core-Examples/FCore-TSP/TSP-fcore.hpp>
#include <OptFrame/modlog/modlog.hpp>
#include <OptFrameLib/FCoreApi1Engine.hpp>

#include "OptFrameLib/LibCTypes.h"

//
#include <OptFrame-Core-Examples/FCore-TSP/TSP-fcore-c-lib.hpp>

int main() {
  using namespace boost::ut;

  FakeEnginePtr engine =
      optframe_api1d_create_engine((int)modlog::LogLevel::Disabled);

  auto* eng = (FCoreApi1Engine*)engine;

  // manipulating seed from rand in engine
  eng->loader.factory.getRandGen()->setSeed(0);

  sref<TSP_fcore::ProblemContext> problem{new TSP_fcore::ProblemContext{}};
  Scanner scanner{"5\n1 1.0 1.0\n2 2.0 5.0\n3 3.0 4.0\n4 4.0 3.0\n5 5.0 2.0"};
  problem->load(scanner);
  problem->rg = eng->loader.factory.getRandGen();
  // get problem pointer as void*
  FakeProblemPtr p_ptr = (void*)(&problem.get());
  eng->experimentalParams["NS_VALID_RANDOM_MOVE_MAX_TRIES"] = "50";
  eng->updateParameters();

  // testing initial solutions

  auto sol = TSP_fcore::frandom(problem);
  // sol = vector(5) [0 , 4 , 2 , 3 , 1]

  "sol"_test = [sol] {
    expect(sol[0] == 0_i);
    expect(sol[1] == 4_i);
    expect(sol[2] == 2_i);
    expect(sol[3] == 3_i);
    expect(sol[4] == 1_i);
  };

  auto e = TSP_fcore::fevaluate(problem, sol);

  "e"_test = [e] { expect(e.evaluation() == 15_i); };

  auto esol = ESolutionTSP{sol, e};

  int idx_ev = optframe_api1d_add_evaluator(engine, fevaluate_c, true, p_ptr);

  "optframe_api1d_add_evaluator"_test = [idx_ev] { expect(idx_ev == 0_i); };

  int idx_c = optframe_api1d_add_constructive(engine, fconstructive_c, p_ptr,
                                              f_sol_deepcopy, f_sol_tostring,
                                              f_decref_solution);

  "optframe_api1d_add_constructive"_test = [idx_c] { expect(idx_c == 0_i); };

  int idx_is = optframe_api1d_create_initial_search(engine, idx_ev, idx_c);

  "optframe_api1d_create_initial_search"_test = [idx_is] {
    expect(idx_is == 0_i);
  };

  // problem->rg->setSeed(1);

  "random_move"_test = [problem, esol] {
    auto m = TSP_fcore::fRandomSwap(problem, esol);
    expect(m->toString() == std::string("MoveSwap(i=1; j=4)"));
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

  int idx_ns_v2 = optframe_api2d_add_ns(
      engine, fnsrand_delta_c, fmove_apply_delta_c, fmove_eq_delta_c,
      fmove_cba_delta_c, fmove_applyupdate_delta_c, p_ptr, f_decref_move);

  "optframe_api2d_add_ns"_test = [idx_ns_v2] { expect(idx_ns_v2 == 1_i); };

  int idx_nsseq2 = optframe_api1d_add_nsseq(
      engine, fnsrand_c_2opt, fIterator_c_2opt, fFirst_c_2opt, fNext_c_2opt,
      fIsDone_c_2opt, fCurrent_c_2opt, fmove_apply_c_2opt, fmove_eq_c_2opt,
      fmove_cba_c_2opt, p_ptr, f_decref_move);

  int new_list = optframe_api1d_create_component_list(
      engine, "[ OptFrame:NS 0 OptFrame:NS 1 ]", "OptFrame:NS[]");

  // =====================

  // test check module

  eng->check.setMessageLevel(modlog::LogLevel::Info);

  /*
  bool expr = optframe_api1d_engine_check(
      engine, 100, 5, false, [](int, FakeEnginePtr eng) -> bool {
        // This enables debugging mode, if component supports it...
        // TODO: create an example where check fails and we verify the logs
        auto* engine = (FCoreApi1Engine*)eng;
        engine->experimentalParams["COMPONENT_LOG_LEVEL"] = "-1";
        engine->updateParameters();
        return true;
      });
  "optframe_api1d_engine_check"_test = [expr] { expect(expr); };
  */

  // =====================

  // begin serious experiments!

  sref<TSP_fcore::ProblemContext> problem2{new TSP_fcore::ProblemContext{}};
  std::string filename = "tsp_ignore_berlin52.txt";
  std::fstream fs;
  fs.open(filename.c_str());
  if (fs.is_open())
    fs.close();
  else {
    std::cout << "failed to read '" << filename << "'" << std::endl;
    optframe_api1d_destroy_engine(engine);
    return 1;
  }
  Scanner scanner2{File{filename}};
  problem2->load(scanner2);
  problem2->rg = eng->loader.factory.getRandGen();

  std::cout << problem->n << std::endl;
  std::cout << problem->dist << std::endl;
  // copy data to other problem
  (*problem).n = problem2->n;
  (*problem).dist = problem2->dist;
  std::cout << problem->n << std::endl;
  std::cout << problem->dist << std::endl;

  // ============================

  int ls_0 = optframe_api1d_build_local_search(
      engine, "OptFrame:ComponentBuilder:LocalSearch:BI",
      "OptFrame:GeneralEvaluator 0 OptFrame:NS:NSFind:NSSeq 0");

  int ls_1 = optframe_api1d_build_local_search(
      engine, "OptFrame:ComponentBuilder:LocalSearch:BI",
      "OptFrame:GeneralEvaluator 0 OptFrame:NS:NSFind:NSSeq 1");

  int list_vnd_idx = optframe_api1d_create_component_list(
      engine, "[ OptFrame:LocalSearch 0 OptFrame:LocalSearch 1 ]",
      "OptFrame:LocalSearch[]");

  int id_vnd = optframe_api1d_build_local_search(
      engine, "OptFrame:ComponentBuilder:LocalSearch:VND",
      "OptFrame:GeneralEvaluator 0 OptFrame:LocalSearch[] 0");

  int id_pert = optframe_api1d_build_component(
      engine, "OptFrame:ComponentBuilder:ILS:LevelPert:LPlus2",
      "OptFrame:GeneralEvaluator 0 OptFrame:NS 0", "OptFrame:ILS:LevelPert");

  int iterMax = problem->n * 10;  // 520 in berlin52
  int levelMax = problem->n / 5;  // 10 in berlin52

  std::stringstream ss_ILS_params;
  ss_ILS_params << "OptFrame:GeneralEvaluator:Evaluator 0 "
                << "OptFrame:InitialSearch 0  OptFrame:LocalSearch " << id_vnd
                << " OptFrame:ILS:LevelPert 0 " << iterMax << " " << levelMax;

  // eng->experimentalParams["COMPONENT_LOG_LEVEL"] = "4";  // disabled
  // eng->experimentalParams["COMPONENT_LOG_LEVEL"] = "0";  // info
  // eng->updateParameters();

  int build_ils = optframe_api1d_build_single(
      engine, "OptFrame:ComponentBuilder:SingleObjSearch:ILS:ILSLevels",
      ss_ILS_params.str().c_str());

  std::cout << "build_ils:" << build_ils << std::endl;

  std::stringstream ssRunAll;
  ssRunAll << "OptFrame:ComponentBuilder:GlobalSearch:SA:BasicSA ";
  ssRunAll << "OptFrame:GeneralEvaluator:Evaluator 0  OptFrame:InitialSearch 0 "
              "OptFrame:NS[] 0 0.995 1000 10000";
  ssRunAll << "\n";
  ssRunAll << "OptFrame:ComponentBuilder:SingleObjSearch:ILS:ILSLevels ";
  ssRunAll << ss_ILS_params.str();

  // ============================

  eng->experimentalParams["COMPONENT_LOG_LEVEL"] = "4";  // disabled
  eng->updateParameters();

  int out_run = optframe_api1d_run_experiments(
      engine, 3, ssRunAll.str().c_str(), 0, "", problem2->n / 5.0);

  // =====================

  bool b = optframe_api1d_destroy_engine(engine);

  "optframe_api1d_destroy_engine"_test = [b] { expect(b == 1_i); };

  return 0;
}