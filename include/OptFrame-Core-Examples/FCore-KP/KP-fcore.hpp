// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrame developers
// https://github.com/optframe/optframe

#ifndef OPTFRAME_CORE_EXAMPLES_FCORE_KP_KP_FCORE_HPP_  // NOLINT
#define OPTFRAME_CORE_EXAMPLES_FCORE_KP_KP_FCORE_HPP_  // NOLINT

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

// C++
#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
// priority loading
#include <OptFrame/printable/printable.hpp>
//
#include <OptFCore/FCore.hpp>
#include <OptFCore/FCoreAll.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>

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

using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT

MOD_EXPORT namespace KP_fcore {
  using ESolutionKP = std::pair<std::vector<bool>,  // (representation)
                                Evaluation<int>     // (objective value)
                                >;
  class ProblemContext {
   public:
    sref<RandGen> rg;
    //
    int n{-1};
    int Q{-1};
    vector<int> p;
    vector<int> w;
    int INF_W{100000};
    ProblemContext() : rg{new RandGen{}} {}
    //
    void load(Scanner& scanner) {
      n = *scanner.nextInt();
      Q = *scanner.nextInt();
      p = vector<int>(n);
      w = vector<int>(n);
      //
      for (int i = 0; i < n; i++) {
        p[i] = *scanner.nextInt();
      }
      //
      for (int i = 0; i < n; i++) {
        w[i] = *scanner.nextInt();
      }
      //
      INF_W = *scanner.nextInt();
    }
  };

  //
  // ProblemContext pKP;  // NOLINT

  inline std::vector<bool> frandom(sref<ProblemContext> pKP) {
    vector<bool> v(pKP->n, false);
    for (unsigned i = 0; i < v.size(); i++) v[i] = pKP->rg->rand() % 2;
    //
    return v;
  }

  //
  inline Evaluation<int> fevaluate(sref<ProblemContext> pKP,
                                   const std::vector<bool>& s) {
    int f = 0;
    int sum_w = 0;
    for (int i = 0; i < pKP->n; i++)
      if (s[i]) {
        f += pKP->p[i];
        sum_w += pKP->w[i];
      }
    //
    if (sum_w >= pKP->Q) f -= pKP->INF_W * (sum_w - pKP->Q);
    // std::cout << "\tevaluated with f = " << f << std::endl;
    //
    return Evaluation<int>{f};
  }

  //
  class MoveBitFlip : public Move<ESolutionKP> {
   public:
    int k;  // MoveBitFlip (moveData = 'int' (k))

    explicit MoveBitFlip(int _k) : k{_k} {}

    uptr<Move<ESolutionKP>> apply(ESolutionKP& se) override {
      // std::cout << "apply k=" << k << " to " << se.first << std::endl;
      se.first[k] = !se.first[k];  // reverts element 'k'
      return uptr<Move<ESolutionKP>>(
          new MoveBitFlip{k});  // returns reverse move
    }

    bool operator==(const Move<ESolutionKP>& _m) const override {
      MoveBitFlip& m = (MoveBitFlip&)_m;
      return this->k == m.k;
    }

    std::string toString() const override {
      std::stringstream ss;
      ss << "MoveBitFlip(k=" << k << ")";
      return ss.str();
    }
  };

  uptr<Move<ESolutionKP>> makeMoveBitFlip(sref<ProblemContext> pKP, int k) {
    return uptr<Move<ESolutionKP>>(new MoveBitFlip{k});
  }
  //
  uptr<Move<ESolutionKP>> fRandomFlip(sref<ProblemContext> pKP,
                                      const ESolutionKP& se) {
    int k = pKP->rg->rand() % pKP->n;  // [0..n-1]
    //
    return uptr<Move<ESolutionKP>>(makeMoveBitFlip(pKP, k));
  }

  class OptFrameCoreKP {
   public:
    FConstructive<std::vector<bool>, ProblemContext> randomConstructive;
    FEvaluator<ESolutionKP, MAXIMIZE, ProblemContext> evalKP;
    FNS<ESolutionKP, ProblemContext> nsFlip;
    FNSSeq<int, ESolutionKP, ProblemContext> nsseqFlip;

    explicit OptFrameCoreKP(sref<ProblemContext> p)
        : randomConstructive{p, frandom},
          evalKP{
              FEvaluator<ESolutionKP, MAXIMIZE, ProblemContext>{p, fevaluate}},
          nsFlip{FNS<ESolutionKP, ProblemContext>{p, fRandomFlip}},
          nsseqFlip{FNSSeq<int, ESolutionKP, ProblemContext>{
              p, fRandomFlip,
              [](sref<ProblemContext> pKP, const ESolutionKP& se) -> int {
                return -1;
              },
              [](sref<ProblemContext> pKP, int& k) -> void { k = 0; },
              [](sref<ProblemContext> pKP, int& k) -> void { k++; },
              [](sref<ProblemContext> pKP, int& k) -> bool {
                return k >= pKP->n;
              },
              [](sref<ProblemContext> pKP, int& k) -> uptr<Move<ESolutionKP>> {
                return uptr<Move<ESolutionKP>>(makeMoveBitFlip(pKP, k));
              }}}

    {}
  };

}  // namespace KP_fcore

#endif  // OPTFRAME_CORE_EXAMPLES_FCORE_KP_KP_FCORE_HPP_ // NOLINT
