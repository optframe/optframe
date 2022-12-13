// SPDX-License-Identifier:  MIT OR LGPL-3.0-or-later
// Copyright (C) 2007-2022 - OptFrme developers
// https://github.com/optframe/optframe

#ifndef OPTFRAME_CORE_EXAMPLES_FCORE_KP_KP_FCORE_HPP_  // NOLINT
#define OPTFRAME_CORE_EXAMPLES_FCORE_KP_KP_FCORE_HPP_  // NOLINT

// C++
#include <algorithm>
#include <functional>
#include <iostream>
#include <utility>
#include <vector>
//
#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp>  // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;        // NOLINT
using namespace optframe;   // NOLINT
using namespace scannerpp;  // NOLINT

namespace KP_fcore {

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
    cout << endl;
    //
    for (int i = 0; i < n; i++) {
      w[i] = *scanner.nextInt();
    }
    cout << endl;
  }
};

//
// ProblemContext pKP;  // NOLINT

std::vector<bool> frandom(sref<ProblemContext> pKP) {
  vector<bool> v(pKP->n, false);
  for (unsigned i = 0; i < v.size(); i++) v[i] = pKP->rg->rand() % 2;
  //
  return v;
}

//
Evaluation<int> fevaluate(sref<ProblemContext> pKP,
                          const std::vector<bool>& s) {
  int f = 0;
  int sum_w = 0;
  for (int i = 0; i < pKP->n; i++)
    if (s[i]) {
      f += pKP->p[i];
      sum_w += pKP->w[i];
    }
  //
  if (sum_w >= pKP->Q) f -= 1000000 * (sum_w - pKP->Q);
  //
  return Evaluation<int>{f};
}

//
class MoveBitFlip : public Move<ESolutionKP> {
 public:
  int k;  // MoveBitFlip (moveData = 'int' (k))

  explicit MoveBitFlip(int _k) : k{_k} {}

  uptr<Move<ESolutionKP>> apply(ESolutionKP& se) override {
    se.first[k] = !se.first[k];                          // reverts element 'k'
    return uptr<Move<ESolutionKP>>(new MoveBitFlip{k});  // returns reverse move
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
        evalKP{FEvaluator<ESolutionKP, MAXIMIZE, ProblemContext>{p, fevaluate}},
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
