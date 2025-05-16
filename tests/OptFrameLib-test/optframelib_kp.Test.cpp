
#include <boost/ut.hpp>

// OptFrame C library headers
#include <OptFrameLib/OptFrameLib.h>

#include <OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp>
#include <OptFrame/modlog/modlog.hpp>
#include <OptFrameLib/FCoreApi1Engine.hpp>

#include "OptFrameLib/LibCTypes.h"

using KP_fcore::ESolutionKP;
using KP_fcore::MoveBitFlip;

// directly testing optframe C library (static)

double fevaluate_c(FakeProblemPtr p_ptr, FakeSolutionPtr s_ptr) {
  using PKP = KP_fcore::ProblemContext;
  std::shared_ptr<PKP> problem_view{(PKP*)p_ptr, [](PKP*) {}};
  sref<PKP> problem{problem_view};
  auto* v = (std::vector<bool>*)(s_ptr);
  auto e = KP_fcore::fevaluate(problem, *v);
  return e.evaluation();
}

FakeSolutionPtr fconstructive_c(FakeProblemPtr p_ptr) {
  using PKP = KP_fcore::ProblemContext;
  std::shared_ptr<PKP> problem_view{(PKP*)p_ptr, [](PKP*) {}};
  sref<PKP> problem{problem_view};
  auto v = KP_fcore::frandom(problem);
  return new std::vector<bool>(std::move(v));
}

FakeMovePtr fnsrand_c(FakeProblemPtr p_ptr, FakeSolutionPtr s_ptr) {
  using PKP = KP_fcore::ProblemContext;
  std::shared_ptr<PKP> problem_view{(PKP*)p_ptr, [](PKP*) {}};
  sref<PKP> problem{problem_view};
  auto* v = (std::vector<bool>*)s_ptr;
  auto m = KP_fcore::fRandomFlip(problem, ESolutionKP{*v, Evaluation<int>{0}});
  auto* m_ptr = m.release();
  return m_ptr;
}

FakeMovePtr fmove_apply_c(FakeProblemPtr p_ptr, FakeMovePtr m_ptr,
                          FakeSolutionPtr s_ptr) {
  auto* m = (MoveBitFlip*)m_ptr;
  int k = m->k;
  auto* v = (std::vector<bool>*)s_ptr;
  (*v)[k] = !(*v)[k];  // reverts element 'k'
  return new MoveBitFlip{k};
}

bool fmove_eq_c(FakeProblemPtr p_ptr, FakeMovePtr m1_ptr, FakeMovePtr m2_ptr) {
  auto* m1 = (MoveBitFlip*)m1_ptr;
  auto* m2 = (MoveBitFlip*)m2_ptr;
  return m1->k == m2->k;
}

bool fmove_cba_c(FakeProblemPtr p_ptr, FakeMovePtr m_ptr,
                 FakeSolutionPtr s_ptr) {
  return true;
}

FakePythonObjPtr fIterator_c(FakeProblemPtr, FakeSolutionPtr) {
  return new MoveBitFlip{0};
}
void fFirst_c(FakeProblemPtr, FakePythonObjPtr it_ptr) {
  auto* m = (MoveBitFlip*)it_ptr;
  m->k = 0;
}
void fNext_c(FakeProblemPtr, FakePythonObjPtr it_ptr) {
  auto* m = (MoveBitFlip*)it_ptr;
  m->k++;
}
bool fIsDone_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* problem = (KP_fcore::ProblemContext*)p_ptr;
  auto* m = (MoveBitFlip*)it_ptr;
  return m->k >= problem->n;
}
FakeMovePtr fCurrent_c(FakeProblemPtr p_ptr, FakePythonObjPtr it_ptr) {
  auto* m = (MoveBitFlip*)it_ptr;
  return new MoveBitFlip{m->k};
}

FakeSolutionPtr f_sol_deepcopy(FakeSolutionPtr s_ptr) {
  auto* v = (std::vector<bool>*)(s_ptr);
  return new std::vector<bool>(*v);
}

size_t f_sol_tostring(FakeSolutionPtr s_ptr, char* str, size_t str_size) {
  auto* v = (std::vector<bool>*)(s_ptr);
  std::stringstream ss;
  ss << *v;
  std::string s = ss.str();
  strcpy(str, s.c_str());
  return s.length();
}

int f_decref_solution(FakeSolutionPtr s_ptr) {
  auto* v = (std::vector<bool>*)(s_ptr);
  delete v;
  return true;
}

int f_decref_move(FakeMovePtr m_ptr) {
  auto* m = (Move<ESolutionKP>*)(m_ptr);
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

  std::string prefix = "";  // default "OptFrame:"
  int list_sz = optframe_api1d_engine_list_components(engine, prefix.c_str());

  "optframe_api1d_engine_list_components"_test = [list_sz] {
    expect(list_sz == 6_i);
  };

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