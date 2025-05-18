
#include <boost/ut.hpp>

// OptFrame C library headers
#include <OptFrameLib/OptFrameLib.h>

#include <OptFrame-Core-Examples/FCore-TSP/TSP-fcore.hpp>
#include <OptFrame/modlog/modlog.hpp>
#include <OptFrameLib/FCoreApi1Engine.hpp>

#include "OptFrameLib/LibCTypes.h"

using TSP_fcore::ESolutionTSP;
using TSP_fcore::Move2Opt;
using TSP_fcore::MoveSwap;
using TSP_fcore::MoveSwapDelta;

// directly testing optframe C library (static)

double fevaluate_c(FakeProblemPtr p_ptr, FakeSolutionPtr s_ptr) {
  using PTSP = TSP_fcore::ProblemContext;
  std::shared_ptr<PTSP> problem_view{(PTSP*)p_ptr, [](PTSP*) {}};
  sref<PTSP> problem{problem_view};
  auto* v = (std::vector<int>*)(s_ptr);
  auto e = TSP_fcore::fevaluate(problem, *v);
  return e.evaluation();
}

FakeSolutionPtr fconstructive_c(FakeProblemPtr p_ptr) {
  using PTSP = TSP_fcore::ProblemContext;
  std::shared_ptr<PTSP> problem_view{(PTSP*)p_ptr, [](PTSP*) {}};
  sref<PTSP> problem{problem_view};
  auto v = TSP_fcore::frandom(problem);
  return new std::vector<int>(std::move(v));
}

// ==================

FakeMovePtr fnsrand_c(FakeProblemPtr p_ptr, FakeSolutionPtr s_ptr) {
  using PTSP = TSP_fcore::ProblemContext;
  std::shared_ptr<PTSP> problem_view{(PTSP*)p_ptr, [](PTSP*) {}};
  sref<PTSP> problem{problem_view};
  auto* v = (std::vector<int>*)s_ptr;
  auto m =
      TSP_fcore::fRandomSwap(problem, ESolutionTSP{*v, Evaluation<int>{0}});
  auto* m_ptr = m.release();
  return m_ptr;
}

FakeMovePtr fnsrand_c_2opt(FakeProblemPtr p_ptr, FakeSolutionPtr s_ptr) {
  using PTSP = TSP_fcore::ProblemContext;
  std::shared_ptr<PTSP> problem_view{(PTSP*)p_ptr, [](PTSP*) {}};
  sref<PTSP> problem{problem_view};
  auto* v = (std::vector<int>*)s_ptr;
  auto m =
      TSP_fcore::fRandom2Opt(problem, ESolutionTSP{*v, Evaluation<int>{0}});
  auto* m_ptr = m.release();
  return m_ptr;
}

FakeMovePtr fmove_apply_c(FakeProblemPtr p_ptr, FakeMovePtr m_ptr,
                          FakeSolutionPtr s_ptr) {
  auto* m = (MoveSwap*)m_ptr;
  int i = m->i;
  int j = m->j;
  auto* v = (std::vector<int>*)s_ptr;
  int aux = (*v)[j];
  (*v)[j] = (*v)[i];
  (*v)[i] = aux;
  return new MoveSwap{j, i};
}

FakeMovePtr fmove_apply_c_2opt(FakeProblemPtr p_ptr, FakeMovePtr m_ptr,
                               FakeSolutionPtr s_ptr) {
  auto* m = (Move2Opt*)m_ptr;
  auto* v = (std::vector<int>*)s_ptr;
  reverse(v->begin() + m->i, v->begin() + m->j + 1);
  return new Move2Opt{m->i, m->j};
}

template <typename T>
bool fmove_eq_c_base(FakeProblemPtr p_ptr, FakeMovePtr m1_ptr,
                     FakeMovePtr m2_ptr) {
  auto* m1 = (T*)m1_ptr;
  auto* m2 = (T*)m2_ptr;
  return (m1->i == m2->i) && (m1->j == m2->j);
}

bool fmove_eq_c(FakeProblemPtr p_ptr, FakeMovePtr m1_ptr, FakeMovePtr m2_ptr) {
  return fmove_eq_c_base<MoveSwap>(p_ptr, m1_ptr, m2_ptr);
}

bool fmove_eq_c_2opt(FakeProblemPtr p_ptr, FakeMovePtr m1_ptr,
                     FakeMovePtr m2_ptr) {
  return fmove_eq_c_base<TSP_fcore::Move2Opt>(p_ptr, m1_ptr, m2_ptr);
}

template <typename T>
bool fmove_cba_c_base(FakeProblemPtr p_ptr, FakeMovePtr m_ptr,
                      FakeSolutionPtr s_ptr) {
  auto* m = (T*)m_ptr;
  return (::abs(m->i - m->j) >= 2) && (m->i >= 1) && (m->j >= 1);
}

bool fmove_cba_c(FakeProblemPtr p_ptr, FakeMovePtr m_ptr,
                 FakeSolutionPtr s_ptr) {
  return fmove_cba_c_base<MoveSwap>(p_ptr, m_ptr, s_ptr);
}

bool fmove_cba_c_2opt(FakeProblemPtr p_ptr, FakeMovePtr m_ptr,
                      FakeSolutionPtr s_ptr) {
  return fmove_cba_c_base<TSP_fcore::Move2Opt>(p_ptr, m_ptr, s_ptr);
}

FakePythonObjPtr fIterator_c(FakeProblemPtr, FakePythonObjPtr it_ptr) {
  return new MoveSwap{0, 0};
}

FakePythonObjPtr fIterator_c_2opt(FakeProblemPtr, FakePythonObjPtr it_ptr) {
  return new TSP_fcore::Move2Opt{0, 0};
}

template <typename T>
FakePythonObjPtr fIterator_c_base(FakeProblemPtr, FakeSolutionPtr) {
  return new T{0, 0};
}

template <typename T>
void fFirst_c_base(FakeProblemPtr, FakePythonObjPtr it_ptr) {
  auto* m = (T*)it_ptr;
  m->i = 0;
  m->j = 1;
}

void fFirst_c(FakeProblemPtr, FakePythonObjPtr it_ptr) {
  return fFirst_c_base<MoveSwap>(nullptr, it_ptr);
}

void fFirst_c_2opt(FakeProblemPtr, FakePythonObjPtr it_ptr) {
  return fFirst_c_base<TSP_fcore::Move2Opt>(nullptr, it_ptr);
}

template <typename T>
void fNext_c_base(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* problem = (TSP_fcore::ProblemContext*)p_ptr;
  auto* m = (T*)it_ptr;
  if (m->j < (problem->n - 1)) {
    m->j++;
  } else {
    m->i++;
    m->j = m->i + 1;
  }
}

void fNext_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  return fNext_c_base<MoveSwap>(p_ptr, it_ptr);
}

void fNext_c_2opt(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  return fNext_c_base<MoveSwap>(p_ptr, it_ptr);
}

template <typename T>
bool fIsDone_c_base(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* problem = (TSP_fcore::ProblemContext*)p_ptr;
  auto* m = (T*)it_ptr;
  return m->i >= problem->n - 1;
}

bool fIsDone_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  return fIsDone_c_base<MoveSwap>(p_ptr, it_ptr);
}

bool fIsDone_c_2opt(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  return fIsDone_c_base<MoveSwap>(p_ptr, it_ptr);
}

template <typename T>
FakeMovePtr fCurrent_c_base(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* m = (T*)it_ptr;
  return new MoveSwap{m->i, m->j};
}

FakeMovePtr fCurrent_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  return fCurrent_c_base<MoveSwap>(p_ptr, it_ptr);
}

FakeMovePtr fCurrent_c_2opt(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  return fCurrent_c_base<MoveSwap>(p_ptr, it_ptr);
}

// ==================

FakeMovePtr fnsrand_delta_c(FakeProblemPtr p_ptr, FakeSolutionPtr s_ptr) {
  using PTSP = TSP_fcore::ProblemContext;
  std::shared_ptr<PTSP> problem_view{(PTSP*)p_ptr, [](PTSP*) {}};
  sref<PTSP> problem{problem_view};
  auto* v = (std::vector<int>*)s_ptr;
  auto m = TSP_fcore::fRandomSwapDelta(problem,
                                       ESolutionTSP{*v, Evaluation<int>{0}});
  auto* m_ptr = m.release();
  return m_ptr;
}

FakeMovePtr fmove_apply_delta_c(FakeProblemPtr p_ptr, FakeMovePtr m_ptr,
                                FakeSolutionPtr s_ptr) {
  auto* problem = (TSP_fcore::ProblemContext*)p_ptr;
  auto* m = (MoveSwapDelta*)m_ptr;
  int i = m->i;
  int j = m->j;
  auto* v = (std::vector<int>*)s_ptr;
  int aux = (*v)[j];
  (*v)[j] = (*v)[i];
  (*v)[i] = aux;
  return new MoveSwapDelta{problem, j, i};
}

PairMoveDoubleLib fmove_applyupdate_delta_c(FakeProblemPtr p_ptr,
                                            FakeMovePtr m_ptr,
                                            FakeSolutionPtr s_ptr, double e) {
  auto* pTSP = (TSP_fcore::ProblemContext*)p_ptr;
  auto* m = (MoveSwapDelta*)m_ptr;
  double diff = 0;
  int i = m->i;
  int j = m->j;
  auto* pv = (std::vector<int>*)s_ptr;
  auto& v = *pv;

  int before_i = (pTSP->n + i - 1) % pTSP->n;
  int before_j = (pTSP->n + j - 1) % pTSP->n;
  int after_i = (pTSP->n + i + 1) % pTSP->n;
  int after_j = (pTSP->n + j + 1) % pTSP->n;

  diff -= pTSP->dist(v[before_i], v[i]);
  diff -= pTSP->dist(v[i], v[after_i]);
  diff -= pTSP->dist(v[before_j], v[j]);
  diff -= pTSP->dist(v[j], v[after_j]);
  diff += pTSP->dist(v[before_i], v[j]);
  diff += pTSP->dist(v[j], v[after_i]);
  diff += pTSP->dist(v[before_j], v[i]);
  diff += pTSP->dist(v[i], v[after_j]);

  // perform swap of clients i and j
  int aux = v[j];
  v[j] = v[i];
  v[i] = aux;

  return PairMoveDoubleLib{.first = new MoveSwapDelta{pTSP, j, i},
                           .second = diff};
}

bool fmove_eq_delta_c(FakeProblemPtr p_ptr, FakeMovePtr m1_ptr,
                      FakeMovePtr m2_ptr) {
  auto* m1 = (TSP_fcore::MoveSwapDelta*)m1_ptr;
  auto* m2 = (MoveSwapDelta*)m2_ptr;
  return (m1->i == m2->i) && (m1->j == m2->j);
}

bool fmove_cba_delta_c(FakeProblemPtr p_ptr, FakeMovePtr m_ptr,
                       FakeSolutionPtr s_ptr) {
  auto* m = (MoveSwapDelta*)m_ptr;
  return (::abs(m->i - m->j) >= 2) && (m->i >= 1) && (m->j >= 1);
}

FakePythonObjPtr fIterator_delta_c(FakeProblemPtr, FakeSolutionPtr) {
  return new MoveSwap{0, 0};
}
void fFirst_delta_c(FakeProblemPtr, FakePythonObjPtr it_ptr) {
  auto* m = (MoveSwapDelta*)it_ptr;
  m->i = 0;
  m->j = 1;
}
void fNext_delta_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* problem = (TSP_fcore::ProblemContext*)p_ptr;
  auto* m = (MoveSwapDelta*)it_ptr;
  if (m->j < (problem->n - 1)) {
    m->j++;
  } else {
    m->i++;
    m->j = m->i + 1;
  }
}
bool fIsDone_delta_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* problem = (TSP_fcore::ProblemContext*)p_ptr;
  auto* m = (MoveSwapDelta*)it_ptr;
  return m->i >= problem->n - 1;
}
FakeMovePtr fCurrent_delta_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* problem = (TSP_fcore::ProblemContext*)p_ptr;
  auto* m = (MoveSwapDelta*)it_ptr;
  return new MoveSwapDelta{problem, m->i, m->j};
}

// ==================

FakeSolutionPtr f_sol_deepcopy(FakeSolutionPtr s_ptr) {
  auto* v = (std::vector<int>*)(s_ptr);
  return new std::vector<int>(*v);
}

size_t f_sol_tostring(FakeSolutionPtr s_ptr, char* str, size_t str_size) {
  auto* v = (std::vector<int>*)(s_ptr);
  std::stringstream ss;
  ss << *v;
  std::string s = ss.str();
  strcpy(str, s.c_str());
  return s.length();
}

int f_decref_solution(FakeSolutionPtr s_ptr) {
  auto* v = (std::vector<int>*)(s_ptr);
  delete v;
  return true;
}

int f_decref_move(FakeMovePtr m_ptr) {
  auto* m = (Move<ESolutionTSP>*)(m_ptr);
  delete m;
  return true;
}

int main() {
  using namespace boost::ut;

  FakeEnginePtr engine =
      optframe_api1d_create_engine((int)modlog::LogLevel::Disabled);

  auto* eng = (FCoreApi1Engine*)engine;

  // manipulating seed from rand in engine
  eng->loader.factory.getRandGen()->setSeed(0);

  sref<TSP_fcore::ProblemContext> problem{new TSP_fcore::ProblemContext{}};
  Scanner scanner{File{"tsp_small.txt"}};
  problem->load(scanner);
  problem->rg = eng->loader.factory.getRandGen();
  // get problem pointer as void*
  FakeProblemPtr p_ptr = (void*)(&problem.get());
  eng->experimentalParams["NS_VALID_RANDOM_MOVE_MAX_TRIES"] = "50";
  eng->updateParameters();

  int idx_ev = optframe_api1d_add_evaluator(engine, fevaluate_c, false, p_ptr);

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

  eng->check.setMessageLevel(modlog::LogLevel::Info);

  // =====================

  std::stringstream ss_SA_params;
  ss_SA_params
      << "OptFrame:GeneralEvaluator:Evaluator 0  OptFrame:InitialSearch 0 "
         "OptFrame:NS[] 0 0.99 100 99999";

  int build_sa = optframe_api1d_build_single(
      engine, "OptFrame:ComponentBuilder:GlobalSearch:SA:BasicSA",
      ss_SA_params.str().c_str());

  std::cout << "build_sa:" << build_sa << std::endl;

  double timelimit = 5.0;

  LibSearchOutput sa_out =
      optframe_api1d_run_sos_search(engine, build_sa, timelimit);

  std::cout << sa_out.best_e << std::endl;

  // =====================

  bool b = optframe_api1d_destroy_engine(engine);

  return 0;
}