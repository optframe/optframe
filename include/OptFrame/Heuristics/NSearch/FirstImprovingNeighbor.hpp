// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HEURISTICS_NSEARCH_FIRSTIMPROVINGNEIGHBOR_HPP_
#define OPTFRAME_HEURISTICS_NSEARCH_FIRSTIMPROVINGNEIGHBOR_HPP_

// C++
#include <string>
#include <utility>
#include <vector>
//
#include "../../Evaluator.hpp"
#include "../../Experimental/NeighborhoodExploration.hpp"
#include "../../NSSeq.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = typename XES::second_type,
          XESolution XSH = XES>
class FirstImprovingNeighbor : public NeighborhoodExploration<XES, XEv> {
 private:
  GeneralEvaluator<XES, XEv, XSH>& eval;
  NSSeq<XES, XEv, XSH>& nsSeq;

 public:
  FirstImprovingNeighbor(GeneralEvaluator<XES, XEv>& _eval,
                         NSSeq<XES, XEv, XSH>& _nsSeq)
      : eval(_eval), nsSeq(_nsSeq) {}

  virtual ~FirstImprovingNeighbor() = default;

  op<RichMove<XES, XEv>> searchMove(
      const XES& se, const StopCriteria<XEv>& stopCriteria) override {
    // gets valid iterator
    uptr<NSIterator<XES, XEv>> it = nsSeq.getIterator(se);
    // TODO: may throw? just break now...
    assert(it);
    // first possible move
    it->first();
    // check if any move exists
    if (it->isDone())
      return nullopt;  // no neighbor
                       //
    do {
      // gets possible move
      uptr<Move<XES, XEv, XSH>> move = it->current();
      // check if move is valid
      if (move->canBeApplied(se)) {
        // get cost only if it's improving... otherwise, just nullopt
        op<XEv> mcost = this->isImprovingCost(*move, se);
        if (mcost) {
          // should not apply to solution directly! must return...
          RichMove<XES, XEv> rmove;
          rmove.move = std::move(move);
          rmove.cost = std::move(*mcost);
          rmove.status = SearchStatus::IMPROVEMENT;
          // return make_optional(rmove);
          return std::optional<RichMove<XES, XEv>>(std::move(rmove));
        }
      }
      // get next possible move
      it->next();
    } while (!it->isDone());
    //
    // finished search
    return std::nullopt;
  }

  // returns 'cost' if it's improving, otherwise std::nullopt
  op<XEv> isImprovingCost(Move<XES, XEv>& m, const XSH& cse,
                          bool allowEstimated = false) {
    // try to get a cost
    op<XEv> p = m.cost(cse, allowEstimated);
    // if p not null => much faster (using cost)
    if (p) {
      // verify if m is an improving move
      // if (p->isStrictImprovement()) {
      if (eval.isStrictImprovement(*p))
        return p;
      else
        return nullopt;
    } else {
      // need to update 's' together with reevaluation of 'e' => slower (may
      // perform reevaluation)

      // TODO: in the future, consider moves with nullptr reverse (must save
      // original solution/evaluation)
      assert(m.hasReverse());

      // remove constness! must make sure 'cse' is never changed from its
      // original state
      XSH& se = const_cast<XSH&>(cse);
      XEv& e = se.second;

      // saving previous evaluation
      XEv ev_begin(e);

      // apply move to both XEv and Solution
      uptr<Move<XES, XEv>> rev = eval.applyMoveReevaluate(m, se);

      // compute cost directly on Evaluation
      XEv mcost = ev_begin.diff(se.second);

      // return to original solution
      rev->apply(se);
      e = std::move(ev_begin);

      // check if it is improvement
      if (eval.isStrictImprovement(mcost)) {
        return make_optional(mcost);
      }

      return nullopt;
    }
    return nullopt;
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) ||
           (NeighborhoodExploration<XES, XEv>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << NeighborhoodExploration<XES, XEv>::idComponent()
       << ":FirstImprovingNeighbor";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }

  std::string toString() const override {
    stringstream ss;
    ss << "FirstImprovingNeighbor: " << nsSeq.toString();
    return ss.str();
  }
};

template <XSolution S, XEvaluation XEv = Evaluation<>,
          XESolution XES = pair<S, XEv>,
          X2ESolution<XES> X2ES = MultiESolution<XES>,
          XSearch<XES> XSH = std::pair<S, XEv>>
class FirstImprovingNeighborBuilder
    : public NeighborhoodExplorationBuilder<S, XEv, XES, X2ES> {
 public:
  virtual ~FirstImprovingNeighborBuilder() = default;

  // NOLINTNEXTLINE
  NeighborhoodExploration<XES, XEv>* build(
      Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf,
      string family = "") override {
    //
    GeneralEvaluator<XES, XEv>* eval;
    std::string comp_id1 = scanner.next();
    int id1 = *scanner.nextInt();
    hf.assign(eval, id1, comp_id1);

    NSSeq<XES, XEv, XSH>* nsseq;
    std::string comp_id2 = scanner.next();
    int id2 = *scanner.nextInt();
    hf.assign(nsseq, id2, comp_id2);

    // NOLINTNEXTLINE
    return new FirstImprovingNeighbor<XES, XEv, XSH>(*eval, *nsseq);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(make_pair(GeneralEvaluator<XES, XEv>::idComponent(),
                               "evaluation function"));
    params.push_back(make_pair(NSSeq<XES, XEv, XSH>::idComponent(),
                               "neighborhood structure"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == FirstImprovingNeighbor<XES, XEv>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<S, XEv>::idComponent()
       << ":FirstImprovingNeighbor";
    return ss.str();
  }

  std::string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif  // OPTFRAME_HEURISTICS_NSEARCH_FIRSTIMPROVINGNEIGHBOR_HPP_
