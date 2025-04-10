// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_EA_RK_DECODERRANDOMKEYS_HPP_  // NOLINT
#define OPTFRAME_HEURISTICS_EA_RK_DECODERRANDOMKEYS_HPP_  // NOLINT

#if (__cplusplus < 202302L) || defined(NO_CXX_MODULES)

#include <algorithm>
#include <string>
// #include "../../../Evaluation.hpp"
// #include "../../../Evaluator.hpp"
// #include "../../../Solution.hpp"
// #include "../../../Solutions/CopySolution.hpp"
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/VEPopulation.hpp>
#include <OptFrame/Heuristics/EA/EA.hpp>
#include <OptFrame/Heuristics/EA/RK/RK.hpp>

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

namespace optframe {

MOD_EXPORT using random_keys = std::vector<double>;

MOD_EXPORT template <XESolution XES, ConceptsComparability KeyType>
class DecoderRandomKeys : public Component {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  using RSK = std::vector<KeyType>;
  using XES2 = std::pair<RSK, XEv>;
  using X2ES = VEPopulation<XES2>;

 public:
  virtual ~DecoderRandomKeys() {}

  // returns evaluation and, optionally the constructed solution.
  // Flag 'needsSolution' indicates that solution is ABSOLUTELY necessary...
  virtual std::pair<XEv, op<S>> decode(const RSK& rk, bool needsSolution) = 0;

  virtual bool isMinimization() const = 0;

  virtual void vsort(X2ES& pop) const { psort(isMinimization(), pop); }

 private:
  void psort(bool isMin, X2ES& p) const {
    // basic selection sort
    for (int i = 0; i < int(p.size()) - 1; i++) {
      int best = i;
      for (int j = i + 1; j < int(p.size()); j++)
        if ((isMin &&
             p.at(j).second.evaluation() < p.at(best).second.evaluation()) ||
            (!isMin &&
             p.at(j).second.evaluation() > p.at(best).second.evaluation()))
          best = j;
      // swap best
      if (best != i) {
        XES2 si = p.at(i);
        p.at(i) = p.at(best);
        p.at(best) = si;

        // double di = fitness[i];
        // fitness[i] = fitness[best];
        // fitness[best] = di;
      }
    }
  }

 public:
  static std::string idComponent() {
    std::stringstream ss;
    ss << Component::idComponent() << ":" << EA::family() << ":" << RK::family()
       << "DecoderRandomKeys";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override { return id(); }
};

// ========================================================
// Basic implementation to use class Evaluator<random_keys>
// ========================================================

// transforms random_keys into R and use given evaluator<R> to generate output
// XEv XRS is not good to be default, as it must come from outside, and be
// compatible
//
//
// template<XRepresentation R, XRSolution<R> XRS, XEvaluation XEv,
// XRepresentation RKeys = optframe::random_keys>
//
// template<XRepresentation R, XRSolution<R> XRS, XEvaluation XEv,
// XRepresentation RKeys = optframe::random_keys>
//
template <XSolution S, XEvaluation XEv, ConceptsComparability KeyType,
          XESolution XES = std::pair<S, XEv>>
class DecoderRandomKeysEvaluator : public DecoderRandomKeys<XES, KeyType> {
  using RSK = std::vector<KeyType>;

 public:
  // using RKEvaluator = Evaluator<random_keys, OPTFRAME_DEFAULT_ADS,
  // CopySolution<random_keys,OPTFRAME_DEFAULT_ADS>;

  Evaluator<RSK, XEv, XES>& evaluator;

  explicit DecoderRandomKeysEvaluator(Evaluator<RSK, XEv, XES>& _evaluator)
      : evaluator{_evaluator} {}

  virtual ~DecoderRandomKeysEvaluator() {}

  std::pair<XEv, op<S>> decode(const RSK& rk, bool needsSolution) override {
    return std::pair<XEv, op<S>>(evaluator.evaluate(rk, nullptr), nullptr);
  }

  bool isMinimization() const override { return evaluator.isMinimization(); }
};

// transforms 'random_keys' into a XRS solution (with R=permutation), then use
// Evaluator<XRS, XEv> to generate output XEv
// template<XRSolution<vector<int>> XRS, XEvaluation XEv>
//
template <XEvaluation XEv, ConceptsComparability KeyType = double,
          XESolution XES = std::pair<std::vector<int>, XEv>>
class EvaluatorPermutationRandomKeys : public DecoderRandomKeys<XES, KeyType> {
  using RSK = std::vector<KeyType>;

 public:
  Evaluator<std::vector<int>, XEv, XES>& ev;  // evaluator for permutation
  int a, b;                                   // decode in interval [a,b]

  EvaluatorPermutationRandomKeys(Evaluator<std::vector<int>, XEv, XES>& _ev,
                                 int _a, int _b)
      : ev{_ev}, a(_a), b(_b) {
    assert(a <= b);
  }

  std::pair<XEv, op<std::vector<int>>> decode(const RSK& rk,
                                              bool needsSolution) override {
    int sz = b - a + 1;
    std::vector<std::pair<double, int>> v(sz);
    int k = 0;
    for (int i = a; i <= b; i++, k++) v[k] = pair<double, int>(rk[i], k);

    sort(v.begin(), v.end(),
         [](const std::pair<double, int>& i, const std::pair<double, int>& j)
             -> bool { return i.first < j.first; });

    // R = vector<int>
    std::vector<int> p(v.size());
    for (unsigned i = 0; i < v.size(); i++) p[i] = v[i].second;

    // XRS is user solution, based on 'vector<int>'
    // XRS sevp(p);
    // XEv e = ev.evaluate(sevp);
    //
    XEv e = ev.evaluate(p);

    // you have the option to actually return a Solution<vector<int>> for
    // post-decoding purposes
    // return pair<XEv, op<S>>(e, make_optional(XRS(p)));
    return pair<XEv, op<std::vector<int>>>(e, make_optional(p));
  }

  bool isMinimization() const override { return ev.isMinimization(); }
};

// implementation of decoder for subset function (std::vector<bool>)
// template<XRSolution<vector<bool>> XRS, XEvaluation XEv = Evaluation<>>
//
template <XEvaluation XEv, ConceptsComparability KeyType,
          XESolution XES = std::pair<std::vector<bool>, XEv>>
class EvaluatorSubsetRandomKeys : public DecoderRandomKeys<XES, KeyType> {
  using RSK = std::vector<KeyType>;

 public:
  Evaluator<std::vector<bool>, XEv, XES>& ev;  // evaluator for permutation
  int a, b;                                    // decode in interval [a,b]
  double limit;  // limit to decide membership (default=0.5)

  EvaluatorSubsetRandomKeys(Evaluator<std::vector<bool>, XEv, XES>& _ev, int _a,
                            int _b, double _limit = 0.5)
      : ev(_ev), a(_a), b(_b), limit(_limit) {
    assert(a <= b);
  }

  virtual std::pair<XEv, op<std::vector<bool>>> decode(
      const RSK& rk, bool needsSolution) override {
    std::vector<bool> v(b - a + 1);
    for (unsigned i = 0; i < v.size(); i++) v[i] = rk[i] >= limit;

    // ----
    // XRS sev(v);
    // Evaluation<> e = ev.evaluate(sev);
    //
    XEv e = ev.evaluate(v);

    // you have the option to actually return a Solution<vector<bool>> for
    // post-decoding purposes
    return pair<XEv, op<std::vector<bool>>>(e, make_optional(v));
  }

  bool isMinimization() const override { return ev.isMinimization(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_EA_RK_DECODERRANDOMKEYS_HPP_ // NOLINT
