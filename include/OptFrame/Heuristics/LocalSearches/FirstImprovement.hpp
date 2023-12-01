// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_FI_HPP_
#define OPTFRAME_FI_HPP_

#include "../../Evaluator.hpp"
#include "../../LocalSearch.hpp"
#include "../../NSSeq.hpp"

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class FirstImprovement : public LocalSearch<XES> {
 private:
  sref<GeneralEvaluator<XES, XSH>> eval;
  sref<NSSeq<XES, XSH>> nsSeq;

 public:
  FirstImprovement(sref<GeneralEvaluator<XES>> _eval,
                   sref<NSSeq<XES, XSH>> _nsSeq)
      : eval(_eval), nsSeq(_nsSeq) {}

  virtual ~FirstImprovement() = default;

  // DEPRECATED
  // virtual void exec(S& s, const StopCriteria<XEv>& stopCriteria)
  //{
  //	Evaluation<> e = std::move(ev.evaluate(s));
  //	exec(s, e, stopCriteria);
  //}

  SearchStatus searchFrom(XES& se,
                          const StopCriteria<XEv>& stopCriteria) override {
    if (Component::verbose) {
      std::cout << "FI: searchFrom begins" << std::endl;
      std::cout << eval->id() << std::endl;
    }
    // XSolution& s = se.first;
    // XEv& e = se.second;

    if (Component::verbose) std::cout << "FI: getIterator" << std::endl;
    uptr<NSIterator<XES>> it = nsSeq->getIterator(se);
    //
    if (!it) {
      if (Component::warning)
        std::cout << "FI(WARNING): getIterator FAILED!" << std::endl;
      return SearchStatus::FAILED;  // poor implementation
    }
    //
    string bestMoveId = "";
    if (Component::verbose) std::cout << "FI: it->first()" << std::endl;
    it->first();

    if (it->isDone()) {
      if (Component::warning)
        std::cout << "FI(WARN): empty neighborhood!" << std::endl;
      return SearchStatus::NO_REPORT;
    }

    do {
      if (Component::verbose) std::cout << "FI: it->current()" << std::endl;
      uptr<Move<XES, XSH>> move = it->current();

      if (!move) {
        if (Component::warning)
          std::cout << "FI(WARNING): it->current() FAILED!" << std::endl;
        return SearchStatus::FAILED;  // poor implementation
      }

      // TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
      /*
                        if(e.getLocalOptimumStatus(move->id()))
                        {
                                delete &it;
                                delete move;
                                return;
                        }
                        */

      //			bestMoveId = move->id();

      if (Component::verbose)
        std::cout << "FI: move->canBeApplied()" << std::endl;
      if (move->canBeApplied(se)) {
        if (this->acceptsImprove(*move, se)) {
          // TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
          // e.setLocalOptimumStatus(bestMoveId, false); //set NS 'id' out of
          // Local Optimum

          return SearchStatus::IMPROVEMENT;
        }
      }

      if (Component::verbose) std::cout << "FI: it->next()" << std::endl;
      it->next();
    } while (!it->isDone());

    // TODO: deprecated! use LOS in NSSeq and NSSeqIterator instead
    // if(bestMoveId != "")
    //	e.setLocalOptimumStatus(bestMoveId, true); //set NS 'id' on Local
    // Optimum
    return SearchStatus::LOCAL_OPT;
  }

  // used on FirstImprovement
  // Accept and apply move if it improves parameter moveCost
  /// bool acceptsImprove(Move<XES, XSH>& m, XSH& se, MoveCost<>* mc = nullptr,
  /// bool allowEstimated = false)
  bool acceptsImprove(Move<XES, XSH>& m, XSH& se, bool allowEstimated = false) {
    if (Component::verbose)
      std::cout << "FI: begin acceptsImprove()" << std::endl;
    // XSolution& s = se.first;
    XEv& e = se.second;

    // try to get a cost
    op<XEv> p = m.cost(se, allowEstimated);

    // if p not null => much faster (using cost)
    if (p) {
      // verify if m is an improving move
      // if (p->isStrictImprovement()) {
      if (eval->isStrictImprovement(*p)) {
        // apply move and get reverse
        uptr<Move<XES, XSH>> rev = m.apply(se);
        // update value using calculated cost
        p->update(e);
        return true;
      } else {
        return false;
      }
    } else {
      if (Component::verbose) std::cout << "FI: no cost" << std::endl;
      // need to update 's' together with reevaluation of 'e' => slower (may
      // perform reevaluation)

      // TODO: in the future, consider moves with nullptr reverse (must save
      // original solution/evaluation)
      assert(m.hasReverse());

      // saving previous evaluation
      XEv ev_begin(e);

      // apply move to both XEv and Solution
      uptr<Move<XES, XSH>> rev = eval->applyMoveReevaluate(m, se);

      // compute cost directly on Evaluation
      XEv mcost = ev_begin.diff(se.second);

      if (Component::verbose)
        std::cout << "FI: isStrictImprovement?" << std::endl;
      // check if it is improvement
      if (eval->isStrictImprovement(mcost)) {
        return true;
      }

      // must return to original situation

      // apply reverse move in order to get the original solution back
      // TODO - Vitor, Why apply Move with e is not used???
      //			Even when reevaluate is implemented, It would be
      // hard to design a strategy that is faster than copying previous
      // evaluation
      //==================================================================
      // pair<Move<S, XEv>*, XEv> ini = applyMove(*rev, s);

      // if XEv wasn't 'outdated' before, restore its previous status
      //			if (!outdated)
      //				e.outdated = outdated;

      // go back to original evaluation
      //			e = ini.second;
      //			delete ini.first;

      if (Component::verbose)
        std::cout << "FI: No improvement. Will reverse." << std::endl;

      uptr<Move<XES, XSH>> ini = rev->apply(se);
      // for now, must be not nullptr
      assert(ini != nullptr);
      // TODO: include management for 'false' hasReverse()
      assert(rev->hasReverse() && ini);
      e = std::move(ev_begin);
      //==================================================================

      if (Component::verbose)
        std::cout << "FI: ends acceptsImprove()" << std::endl;

      return false;
    }
  }

  bool compatible(std::string s) override {
    return (s == idComponent()) || (LocalSearch<XES>::compatible(s));
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearch<XES>::idComponent() << ":FI";
    return ss.str();
  }

  virtual string id() const override { return idComponent(); }

  std::string toString() const override {
    stringstream ss;
    ss << "FI: " << nsSeq->toString();
    return ss.str();
  }
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES>
#else
template <typename XES>
#endif
class FirstImprovementBuilder : public LocalSearchBuilder<XES> {
 public:
  virtual ~FirstImprovementBuilder() {}

  LocalSearch<XES>* build(Scanner& scanner, HeuristicFactory<XES>& hf,
                          string family = "") override {
    if (this->verbose)
      std::cout << "Debug: FirstImprovementBuilder::build()" << std::endl;

    sptr<GeneralEvaluator<XES>> eval;
    std::string sid_0 = scanner.next();
    int id_0 = *scanner.nextInt();
    hf.assign(eval, id_0, sid_0);

    sptr<NSSeq<XES>> nsseq;
    std::string sid_1 = scanner.next();
    int id_1 = *scanner.nextInt();
    hf.assign(nsseq, id_1, sid_1);

    return new FirstImprovement<XES>(eval, nsseq);
  }

  vector<pair<std::string, std::string>> parameters() override {
    vector<pair<string, string>> params;
    params.push_back(
        make_pair(GeneralEvaluator<XES>::idComponent(), "evaluation function"));
    params.push_back(
        make_pair(NSSeq<XES>::idComponent(), "neighborhood structure"));

    return params;
  }

  bool canBuild(std::string component) override {
    return component == FirstImprovement<XES>::idComponent();
  }

  static string idComponent() {
    stringstream ss;
    ss << LocalSearchBuilder<XES>::idComponent() << ":FI";
    return ss.str();
  }

  std::string toString() const override { return id(); }

  virtual string id() const override { return idComponent(); }
};

}  // namespace optframe

#endif /*OPTFRAME_FI_HPP_*/
