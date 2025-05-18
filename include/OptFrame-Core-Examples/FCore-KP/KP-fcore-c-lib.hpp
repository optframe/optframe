#pragma once

// OptFrame C library headers
#include <OptFrameLib/OptFrameLib.h>

#include <OptFrame-Core-Examples/FCore-KP/KP-fcore.hpp>
#include <OptFrame/modlog/modlog.hpp>
#include <OptFrameLib/FCoreApi1Engine.hpp>

#include "OptFrameLib/LibCTypes.h"

//
#include <OptFrame/Hyper/RunExperimentsCommand.hpp>

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
