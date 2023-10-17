// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_NSENUM_HPP_
#define OPTFRAME_NSENUM_HPP_

#include "NSEnumIterator.hpp"
#include "NSSeq.hpp"
#include "RandGen.hpp"

// using namespace std;

namespace optframe {

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSearch<XES> XSH>
#else
template <typename XES, typename XSH>
#endif
class NSEnum : public NSSeq<XES, XSH> {
  using XEv = typename XES::second_type;

 protected:
  sref<RandGen> rg;

 public:
  explicit NSEnum(sref<RandGen> _rg) : rg(_rg) {}

  virtual ~NSEnum() = default;

  // every move from NSEnum must be solution-independent (only
  // problem-dependent)
  bool isSolutionIndependent() const final { return true; }

  uptr<Move<XES, XSH>> randomMove(const XES&) override {
    unsigned int x = rg->rand(size());
    return indexMove(x);
  }

  uptr<NSIterator<XES>> getIterator(const XES&) override {
    return uptr<NSIterator<XES>>(new NSEnumIterator<XES>(*this));
  }

  virtual uptr<Move<XES, XSH>> indexMove(unsigned int index) = 0;

  virtual unsigned int size() const = 0;

 public:
  // findFrom: returns the *next* move starting from index k (inclusive), that
  // strictly improves current solution 'se', according 'gev' RETURNS: pair<
  // uptr<Move<XES, XSH>>, op<XEv> >
  virtual pair<Move<XES, XSH>*, op<XEv>> findFrom(unsigned int& k,
                                                  GeneralEvaluator<XES>& gev,
                                                  XES& se) {
    // iterates from k
    for (unsigned xk = k; xk < this->size(); xk++) {
      // gets variable index 'xk'
      uptr<Move<XES, XSH>> pm = this->indexMove(xk);
      if (!pm->canBeApplied(se)) continue;
      Move<XES, XSH>* m = pm.get();
      op<XEv> mvcost = gev.moveCost(*m, se);
      if (gev.isStrictImprovement(*mvcost)) {
        k = xk;  // updates k
        return std::make_pair(m, mvcost);
      }
    }
    k = this->size();  // updates k
    // no good move
    return std::make_pair(nullptr, std::nullopt);
  }

  // findBest: returns move that greatly improves current solution 'se',
  // according 'gev' iterates using findFrom: 0..size-1
  pair<Move<XES, XSH>*, op<XEv>> findBest(GeneralEvaluator<XES>& gev,
                                          XES& se) override {
    // starts count iterator
    unsigned int k = 0;
    // best move
    op<XEv> bestCost = std::nullopt;
    Move<XES, XSH>* bestMove = nullptr;
    // stops at maximum (or return)
    while (k < this->size()) {
      // gets index 'k'
      pair<Move<XES, XSH>*, op<XEv>> mve = findFrom(k, gev, se);
      // if not good, process is finished: returns existing best
      if (!mve.second) return std::make_pair(bestMove, bestCost);
      // if improvement exists, try to improve best
      if ((!bestCost) || gev.betterStrict(*mve.second, *bestCost)) {
        bestMove = mve.first;
        bestCost = std::move(mve.second);
      }
    }
    // finished: returns best available
    return std::make_pair(bestMove, bestCost);
  }

 public:
  static string idComponent() {
    stringstream ss;
    ss << NSSeq<XES, XSH>::idComponent() << ":NSEnum";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (NSSeq<XES, XSH>::compatible(s));
  }
};

}  // namespace optframe

#endif /*OPTFRAME_NSENUM_HPP_*/
