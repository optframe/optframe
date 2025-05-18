
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
  bool run_SA = false;
  bool run_ILS = true;

  using namespace boost::ut;

  FakeEnginePtr engine =
      optframe_api1d_create_engine((int)modlog::LogLevel::Disabled);

  auto* eng = (FCoreApi1Engine*)engine;

  // manipulating seed from rand in engine
  eng->loader.factory.getRandGen()->setSeed(0);

  sref<TSP_fcore::ProblemContext> problem{new TSP_fcore::ProblemContext{}};
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
  Scanner scanner{File{filename}};
  problem->load(scanner);
  problem->rg = eng->loader.factory.getRandGen();

  // get problem pointer as void*
  FakeProblemPtr p_ptr = (void*)(&problem.get());
  eng->experimentalParams["NS_VALID_RANDOM_MOVE_MAX_TRIES"] = "50";
  // eng->experimentalParams["COMPONENT_LOG_LEVEL"] = "4";  // disabled
  eng->updateParameters();

  int idx_ev = optframe_api1d_add_evaluator(engine, fevaluate_c, true, p_ptr);

  int idx_c = optframe_api1d_add_constructive(engine, fconstructive_c, p_ptr,
                                              f_sol_deepcopy, f_sol_tostring,
                                              f_decref_solution);

  int idx_is = optframe_api1d_create_initial_search(engine, idx_ev, idx_c);

  int idx_ns =
      optframe_api1d_add_ns(engine, fnsrand_c, fmove_apply_c, fmove_eq_c,
                            fmove_cba_c, p_ptr, f_decref_move);

  int idx_nsseq = optframe_api1d_add_nsseq(
      engine, fnsrand_c, fIterator_c, fFirst_c, fNext_c, fIsDone_c, fCurrent_c,
      fmove_apply_c, fmove_eq_c, fmove_cba_c, p_ptr, f_decref_move);

  int idx_nsseq2 = optframe_api1d_add_nsseq(
      engine, fnsrand_c_2opt, fIterator_c_2opt, fFirst_c_2opt, fNext_c_2opt,
      fIsDone_c_2opt, fCurrent_c_2opt, fmove_apply_c_2opt, fmove_eq_c_2opt,
      fmove_cba_c_2opt, p_ptr, f_decref_move);

  int new_list = optframe_api1d_create_component_list(
      engine, "[ OptFrame:NS 0 OptFrame:NS 1 ]", "OptFrame:NS[]");

  // eng->check.setMessageLevel(modlog::LogLevel::Info);

  // =====================

  std::stringstream ss_SA_params;
  ss_SA_params
      << "OptFrame:GeneralEvaluator:Evaluator 0  OptFrame:InitialSearch 0 "
         "OptFrame:NS[] 0 0.99 100 99999";

  int build_sa = optframe_api1d_build_single(
      engine, "OptFrame:ComponentBuilder:GlobalSearch:SA:BasicSA",
      ss_SA_params.str().c_str());

  std::cout << "build_sa:" << build_sa << std::endl;

  // =====================

  int ls_0 = optframe_api1d_build_local_search(
      engine, "OptFrame:ComponentBuilder:LocalSearch:BI",
      "OptFrame:GeneralEvaluator 0 OptFrame:NS:NSFind:NSSeq 0");

  int ls_1 = optframe_api1d_build_local_search(
      engine, "OptFrame:ComponentBuilder:LocalSearch:BI",
      "OptFrame:GeneralEvaluator 0 OptFrame:NS:NSFind:NSSeq 1");

  int list_vnd_idx = optframe_api1d_create_component_list(
      engine, "[ OptFrame:LocalSearch 0 OptFrame:LocalSearch 1 ]",
      "OptFrame:LocalSearch[]");

  int id_pert = optframe_api1d_build_component(
      engine, "OptFrame:ComponentBuilder:ILS:LevelPert:LPlus2",
      "OptFrame:GeneralEvaluator 0 OptFrame:NS 0", "OptFrame:ILS:LevelPert");

  int iterMax = problem->n * 10;  // 520 in berlin52
  int levelMax = problem->n / 5;  // 10 in berlin52

  std::stringstream ss_ILS_params;
  ss_ILS_params << "OptFrame:GeneralEvaluator:Evaluator 0 "
                   "OptFrame:InitialSearch 0  OptFrame:LocalSearch 0 "
                   " OptFrame:ILS:LevelPert 0 "
                << iterMax << " " << levelMax;

  // eng->experimentalParams["COMPONENT_LOG_LEVEL"] = "4";  // disabled
  // eng->experimentalParams["COMPONENT_LOG_LEVEL"] = "0";  // info
  eng->updateParameters();

  int build_ils = optframe_api1d_build_single(
      engine, "OptFrame:ComponentBuilder:SingleObjSearch:ILS:ILSLevels",
      ss_ILS_params.str().c_str());

  std::cout << "build_ils:" << build_ils << std::endl;

  // ====================

  double timelimit = problem->n / 1.0;  // 5.0;

  if (run_SA) {
    LibSearchOutput sa_out =
        optframe_api1d_run_sos_search(engine, build_sa, timelimit);
    std::cout << sa_out.best_e << std::endl;
  }

  if (run_ILS) {
    LibSearchOutput ils_out =
        optframe_api1d_run_sos_search(engine, build_ils, timelimit);
    std::cout << ils_out.best_e << std::endl;
  }

  // =====================

  bool b = optframe_api1d_destroy_engine(engine);

  return 0;
}