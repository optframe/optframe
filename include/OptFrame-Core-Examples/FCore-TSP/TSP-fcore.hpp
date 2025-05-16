// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2025 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef OPTFRAME_CORE_EXAMPLES_FCORE_TSP_TSP_FCORE_HPP_  // NOLINT
#define OPTFRAME_CORE_EXAMPLES_FCORE_TSP_TSP_FCORE_HPP_  // NOLINT

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <algorithm>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>
//
#include <OptFCore/FCoreAll.hpp>
#include <OptFrame/Concepts/Domain.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Heuristics/EA/RK/InitialEPopulationRK.hpp>
#include <OptFrame/Pareto/MultiEvaluator.hpp>
#include <OptFrame/Pareto/Pareto.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Search/InitialPopulation.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

using namespace std;
using namespace optframe;
using namespace scannerpp;

// next definitions come here within namespace
// this also works when defining in global scope (same as 'class')
MOD_EXPORT namespace TSP_fcore {
  using ESolutionTSP = std::pair<std::vector<int>, Evaluation<int>>;
  // using MINIMIZE = MinOrMax::MINIMIZE;

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
  static_assert(XESolution<ESolutionTSP>);
#endif

  // TSP problem context and data reads
  class ProblemContext {
   public:
    sref<RandGen> rg{new RandGen{}};
    int n;             // number of clients
    Matrix<int> dist;  // distance matrix (Euclidean)
    // load data from Scanner
    void load(Scanner& scanner) {
      n = *scanner.nextInt();    // reads number of clients
      dist = Matrix<int>(n, n);  // initializes n x n matrix
      //
      vector<double> xvalues(n);
      vector<double> yvalues(n);
      //
      for (int i = 0; i < n; i++) {
        scanner.next();
        xvalues[i] = *scanner.nextDouble();  // reads x
        yvalues[i] = *scanner.nextDouble();  // reads y
      }
      // calculate distance values, for every client pair (i,j)
      for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
          dist(i, j) = ::round(distance(xvalues.at(i), yvalues.at(i),
                                        xvalues.at(j), yvalues.at(j)));
    }
    // euclidean distance (double as return)
    double distance(double x1, double y1, double x2, double y2) {
      return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
    }
  };
  // Create TSP Problem Context
  // ProblemContextTSP pTSP;

  Evaluation<int> fevaluate(sref<ProblemContext> pTSP,
                            const std::vector<int>& s) {
    int f1 = 0;
    for (int i = 0; i < int(pTSP->n) - 1; i++) f1 += pTSP->dist(s[i], s[i + 1]);
    f1 += pTSP->dist(s[int(pTSP->n) - 1], s[0]);
    return Evaluation<int>{f1};
  }

  std::vector<int> frandom(sref<ProblemContext> pTSP) {
    vector<int> v(pTSP->n, -1);  // get information from context
    for (auto i = 0; i < (int)v.size(); i++) v[i] = i;
    // leave 0 on first position and shuffle the rest
    for (auto i = 1; i < (int)v.size() - 1; i++) {
      int j = pTSP->rg->rand(i, v.size() - 1);
      // swap i and j
      int aux = v[i];
      v[i] = v[j];
      v[j] = aux;
    }
    return v;
  }

  class MoveSwap : public Move<ESolutionTSP> {
   public:
    int i, j;

    MoveSwap(int _i, int _j) : i{_i}, j{_j} {}

    bool canBeApplied(const ESolutionTSP& se) override {
      return (::abs(i - j) >= 2) && (i >= 1) && (j >= 1);
    }

    uptr<Move<ESolutionTSP>> apply(ESolutionTSP& se) override {
      // perform swap of clients i and j
      int aux = se.first[j];
      se.first[j] = se.first[i];
      se.first[i] = aux;
      return uptr<Move<ESolutionTSP>>(new MoveSwap{j, i});
    }

    bool operator==(const Move<ESolutionTSP>& other) const override {
      auto& fmove = (MoveSwap&)other;
      return (i == fmove.i) && (j == fmove.j);
    }

    std::string toString() const override {
      std::stringstream ss;
      ss << "MoveSwap(i=" << i << "; j=" << j << ")";
      return ss.str();
    }
  };

  class Move2Opt : public Move<ESolutionTSP> {
   public:
    int i, j;

    Move2Opt(int _i, int _j) : i{_i}, j{_j} {}

    bool canBeApplied(const ESolutionTSP& se) override {
      return (::abs(i - j) >= 2) && (i >= 1) && (j >= 1);
    }

    uptr<Move<ESolutionTSP>> apply(ESolutionTSP& se) override {
      // perform swap of clients i and j
      int aux = se.first[j];
      se.first[j] = se.first[i];
      se.first[i] = aux;
      return uptr<Move<ESolutionTSP>>(new Move2Opt{j, i});
    }

    bool operator==(const Move<ESolutionTSP>& other) const override {
      auto& fmove = (Move2Opt&)other;
      return (i == fmove.i) && (j == fmove.j);
    }

    std::string toString() const override {
      std::stringstream ss;
      ss << "Move2Opt(i=" << i << "; j=" << j << ")";
      return ss.str();
    }
  };

  template <typename T>
  uptr<Move<ESolutionTSP>> makeMove(int i, int j) {
    return uptr<Move<ESolutionTSP>>(new T{i, j});
  }

  template <typename T>
  uptr<Move<ESolutionTSP>> fRandomMove(sref<ProblemContext> pTSP,
                                       const ESolutionTSP& se) {
    int i = pTSP->rg->rand(pTSP->n - 3);
    int j = pTSP->rg->rand(i + 2, pTSP->n - 1);
    return makeMove<T>(i, j);
  }

  uptr<Move<ESolutionTSP>> makeMoveSwap(sref<ProblemContext> pTSP, int i,
                                        int j) {
    return makeMove<MoveSwap>(i, j);
  }

  uptr<Move<ESolutionTSP>> fRandomSwap(sref<ProblemContext> pTSP,
                                       const ESolutionTSP& se) {
    return fRandomMove<MoveSwap>(pTSP, se);
  }

  uptr<Move<ESolutionTSP>> makeMove2Opt(int i, int j) {
    return makeMove<Move2Opt>(i, j);
  }

  uptr<Move<ESolutionTSP>> fRandom2Opt(sref<ProblemContext> pTSP,
                                       const ESolutionTSP& se) {
    return fRandomMove<Move2Opt>(pTSP, se);
  }

  // ============= MoveSwapDelta ==========

  class MoveSwapDelta : public Move<ESolutionTSP> {
   public:
    ProblemContext* pTSP;
    int i, j;

    MoveSwapDelta(ProblemContext* _pTSP, int _i, int _j)
        : pTSP{_pTSP}, i{_i}, j{_j} {}

    bool canBeApplied(const ESolutionTSP& se) override {
      return (::abs(i - j) >= 2) && (i >= 1) && (j >= 1);
    }

    uptr<Move<ESolutionTSP>> apply(ESolutionTSP& se) override {
      // perform swap of clients i and j
      int aux = se.first[j];
      se.first[j] = se.first[i];
      se.first[i] = aux;
      return uptr<Move<ESolutionTSP>>(new MoveSwapDelta{pTSP, j, i});
    }

    uptr<Move<ESolutionTSP>> applyUpdate(ESolutionTSP& se) override {
      auto& v = se.first;
      int diff = 0;
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
      int aux = se.first[j];
      se.first[j] = se.first[i];
      se.first[i] = aux;

      se.second.setObjFunction(se.second.getObjFunction() + diff);
      return uptr<Move<ESolutionTSP>>(new MoveSwapDelta{pTSP, j, i});
    }

    bool operator==(const Move<ESolutionTSP>& other) const override {
      auto& fmove = (MoveSwapDelta&)other;
      return (i == fmove.i) && (j == fmove.j);
    }

    std::string toString() const override {
      std::stringstream ss;
      ss << "MoveSwapDelta(i=" << i << "; j=" << j << ")";
      return ss.str();
    }
  };

  uptr<Move<ESolutionTSP>> makeMoveSwapDelta(sref<ProblemContext> pTSP, int i,
                                             int j) {
    return uptr<Move<ESolutionTSP>>(new MoveSwapDelta{&pTSP.get(), i, j});
  }

  uptr<Move<ESolutionTSP>> fRandomSwapDelta(sref<ProblemContext> pTSP,
                                            const ESolutionTSP& se) {
    int i = pTSP->rg->rand() % pTSP->n;
    int j = i;
    while ((j <= i) || (i == 0)) {
      i = pTSP->rg->rand() % pTSP->n;
      j = pTSP->rg->rand() % pTSP->n;
    }

    return makeMoveSwapDelta(pTSP, i, j);
  }

  class OptFrameCoreTSP {
   public:
    using S = typename ESolutionTSP::first_type;
    using XEv = typename ESolutionTSP::second_type;
    using XES = ESolutionTSP;
    FConstructive<std::vector<int>, ProblemContext> crand;
    FEvaluator<XES, MINIMIZE, ProblemContext> ev;
    FNS<ESolutionTSP, ProblemContext> nsswap;
    FNSSeq<std::pair<int, int>, ESolutionTSP, ProblemContext> nsseq;

    explicit OptFrameCoreTSP(sref<ProblemContext> p)
        : crand{p, frandom},
          ev{p, fevaluate},
          nsswap{p, fRandomSwap},
          nsseq{p,
                [](sref<ProblemContext> pTSP,
                   const ESolutionTSP& se) -> uptr<Move<ESolutionTSP>> {
                  int i = rand() % pTSP->n;
                  int j = i;
                  while (j <= i) {
                    i = rand() % pTSP->n;
                    j = rand() % pTSP->n;
                  }
                  return makeMoveSwap(pTSP, i, j);
                },

                [](sref<ProblemContext> pTSP, const ESolutionTSP& se)
                    -> std::pair<int, int> { return make_pair(-1, -1); },
                [](sref<ProblemContext> pTSP, std::pair<int, int>& p) -> void {
                  p.first = 0;
                  p.second = 1;
                },
                [](sref<ProblemContext> pTSP, std::pair<int, int>& p) -> void {
                  if (p.second < (pTSP->n - 1)) {
                    p.second++;
                  } else {
                    p.first++;
                    p.second = p.first + 1;
                  }
                },
                [](sref<ProblemContext> pTSP, std::pair<int, int>& p) -> bool {
                  return p.first >= pTSP->n - 1;
                },
                [](sref<ProblemContext> pTSP,
                   std::pair<int, int>& p) -> uptr<Move<ESolutionTSP>> {
                  return makeMoveSwap(pTSP, p.first, p.second);
                }} {
      //
    }
  };

}  // namespace BTSP_fcore

#endif  // OPTFRAME_CORE_EXAMPLES_FCORE_TSP_TSP_FCORE_HPP_ // NOLINT
