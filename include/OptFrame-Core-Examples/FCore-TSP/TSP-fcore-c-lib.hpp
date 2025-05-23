#pragma once

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