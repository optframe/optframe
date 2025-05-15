
#include <boost/ut.hpp>

// OptFrame C library headers
#include <OptFrameLib/OptFrameLib.h>

#include <OptFrame-Core-Examples/FCore-TSP/TSP-fcore.hpp>
#include <OptFrame/modlog/modlog.hpp>
#include <OptFrameLib/FCoreApi1Engine.hpp>

#include "OptFrameLib/LibCTypes.h"

using TSP_fcore::ESolutionTSP;
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

bool fmove_eq_c(FakeProblemPtr p_ptr, FakeMovePtr m1_ptr, FakeMovePtr m2_ptr) {
  auto* m1 = (MoveSwap*)m1_ptr;
  auto* m2 = (MoveSwap*)m2_ptr;
  return (m1->i == m2->i) && (m1->j == m2->j);
}

bool fmove_cba_c(FakeProblemPtr p_ptr, FakeMovePtr m_ptr,
                 FakeSolutionPtr s_ptr) {
  auto* m = (MoveSwap*)m_ptr;
  return (::abs(m->i - m->j) >= 2) && (m->i >= 1) && (m->j >= 1);
}

FakePythonObjPtr fIterator_c(FakeProblemPtr, FakeSolutionPtr) {
  return new MoveSwap{0, 0};
}
void fFirst_c(FakeProblemPtr, FakePythonObjPtr it_ptr) {
  auto* m = (MoveSwap*)it_ptr;
  m->i = 0;
  m->j = 1;
}
void fNext_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* problem = (TSP_fcore::ProblemContext*)p_ptr;
  auto* m = (MoveSwap*)it_ptr;
  if (m->j < (problem->n - 1)) {
    m->j++;
  } else {
    m->i++;
    m->j = m->i + 1;
  }
}
bool fIsDone_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* problem = (TSP_fcore::ProblemContext*)p_ptr;
  auto* m = (MoveSwap*)it_ptr;
  return m->i >= problem->n - 1;
}
FakeMovePtr fCurrent_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* m = (MoveSwap*)it_ptr;
  return new MoveSwap{m->i, m->j};
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

  std::stringstream ss;
  auto* eng = (FCoreApi1Engine*)engine;
  eng->logstream = &ss;
  eng->engineLogLevel = modlog::LogLevel::Debug;

  optframe_api0d_engine_welcome(engine);

  std::string welcome_message = ss.str();
  ss.clear();

  "welcome"_test = [welcome_message] {
    expect((welcome_message.find(
                "Welcome to OptFrame Functional Core (FCore) - version") !=
            std::string::npos));
  };

  // testing welcome again on std::cout
  eng->logstream = &std::cout;
  optframe_api0d_engine_welcome(engine);

  "first_rand"_test = [eng] {
    eng->loader.factory.getRandGen()->setSeed(9999);
    expect(eng->loader.factory.getRandGen()->rand() % 100 == 5_i);
  };

  "optframe_api1d_engine_rand"_test = [engine] {
    optframe_api1d_engine_rand_set_seed(engine, 9999);
    expect(optframe_api1d_engine_rand(engine) % 100 == 5_i);
  };

  // manipulating seed from rand in engine
  eng->loader.factory.getRandGen()->setSeed(0);

  sref<TSP_fcore::ProblemContext> problem{new TSP_fcore::ProblemContext{}};
  Scanner scanner{"5\n1 1.0 1.0\n2 2.0 5.0\n3 3.0 4.0\n4 4.0 3.0\n5 5.0 2.0"};
  problem->load(scanner);
  problem->rg = eng->loader.factory.getRandGen();
  // get problem pointer as void*
  FakeProblemPtr p_ptr = (void*)(&problem.get());

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

  // problem->rg->setSeed(1);

  "random_move"_test = [problem, esol] {
    auto m = TSP_fcore::fRandomSwap(problem, esol);
    expect(m->toString() == std::string("MoveSwap(i=2; j=4)"));
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

  // =====================

  // test check module

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

  // =====================

  bool b = optframe_api1d_destroy_engine(engine);

  "optframe_api1d_destroy_engine"_test = [b] { expect(b == 1_i); };

  return 0;
}