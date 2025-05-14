
#include <boost/ut.hpp>

// OptFrame C library headers
#include <OptFrameLib/OptFrameLib.h>

#include <OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp>
#include <OptFrame/modlog/modlog.hpp>
#include <OptFrameLib/FCoreApi1Engine.hpp>

#include "OptFrameLib/LibCTypes.h"

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

  // manipulating seed from rand in engine
  eng->loader.factory.getRandGen()->setSeed(0);

  sref<KP_fcore::ProblemContext> problem{new KP_fcore::ProblemContext{}};
  Scanner scanner{"5\n260\n60 35 55 40 45\n120 70 110 80 90\n1000"};
  problem->load(scanner);
  problem->rg = eng->loader.factory.getRandGen();

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

  FakeProblemPtr p_ptr = (void*)(&problem.get());

  int idx_ev = optframe_api1d_add_evaluator(engine, fevaluate_c, false, p_ptr);

  "optframe_api1d_add_evaluator"_test = [idx_ev] { expect(idx_ev == 0_i); };

  int idx_c = optframe_api1d_add_constructive(engine, fconstructive_c, p_ptr,
                                              f_sol_deepcopy, f_sol_tostring,
                                              f_decref_solution);

  "optframe_api1d_add_constructive"_test = [idx_c] { expect(idx_c == 0_i); };

  bool b = optframe_api1d_destroy_engine(engine);

  "optframe_api1d_destroy_engine"_test = [b] { expect(b == 1_i); };

  return 0;
}