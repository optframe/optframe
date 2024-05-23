// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_PARETOMANAGER_HPP_
#define OPTFRAME_PARETOMANAGER_HPP_

#include <cstring>
#include <iostream>
#include <utility>
#include <vector>
//
#include "./Pareto.hpp"

namespace optframe {

template <XESolution XES, XEMSolution XMES>
class ParetoManager {
  using S = typename XMES::first_type;
  using XMEv = typename XMES::second_type;
  using XEv = typename XMEv::XEv;

 public:
  sref<MultiEvaluator<XES, XMES>> multiEval;
  // GeneralEvaluator<XMES>& multiEval;
  // MultiEvaluator<S, XEv>& mev; // cannot be this, for now!
  ParetoDominance<XES, XMES> dom;
  ParetoDominanceWeak<XES, XMES> domWeak;
  // Pareto<XMES> x_e;

 public:
  explicit ParetoManager(sref<MultiEvaluator<XES, XMES>> _multiEval)
      :  // paretoManager(GeneralEvaluator<XMES>& _multiEval) : // cannot
         // be this, for now!
        multiEval{_multiEval},
        dom{ParetoDominance<XES, XMES>(_multiEval)},
        domWeak{ParetoDominanceWeak<XES, XMES>(_multiEval)} {}

  virtual ~ParetoManager() {}

  //	virtual Pareto<XMES>& getParetoInsideManager()
  //	{
  //		return x_e;
  //	}

  //	MultiEvaluator<S, XEv>& getMultiEvaluator()
  //	{
  //		return multiEval;
  //	}

  //	//Special addSolution used in the 2PPLS speedUp
  //	bool addSolution(S* candidate)
  //	{
  //		MultiEvaluation<>& mev = multiEval.evaluate(*candidate);
  //		bool added = addSolution(candidate, &mev);
  //		delete &mev;
  //		return added;
  //	}
  //
  //	virtual bool addSolution(const S* candidate, const MultiEvaluation<>*
  // mev)
  //	{
  //		cout << "Something wrong has happen! \n It is inside addSolution
  // candidate,mev! \n This should be reimplemented" << endl; getchar();
  // exit(1);
  ////		return false;
  //	}

  bool addSolution(Pareto<XMES>& p, const S& candidate) {
    MultiEvaluation<> mev = multiEval->evaluate(candidate);
    XMES cand_smev = make_pair(candidate, mev);
    // bool added = addSolutionWithMEV(p, candidate, mev);
    bool added = addSolutionWithMEV(p, cand_smev);

    return added;
  }

  // virtual bool addSolutionWithMEV(Pareto<XMES>& p, const S& candidate,
  // const MultiEvaluation<>& candidateMev)
  virtual bool addSolutionWithMEV(Pareto<XMES>& p, const XMES& cand_smev) {
    bool added = true;
    for (int ind = 0; ind < (int)p.size(); ind++) {
      const MultiEvaluation<>& popIndFitness = p.getIndMultiEvaluation(ind);

      if (domWeak.dominates(popIndFitness, cand_smev.second)) return false;

      if (dom.dominates(cand_smev.second, popIndFitness)) {
        p.erase(ind);
        ind--;
      }
    }
    if (added)
      // p.add_indWithMev(candidate, candidateMev);
      p.add_indWithMev(cand_smev);

    return added;
  }

  bool addSolutionWithMEVReevaluation(Pareto<XMES>& p, S& candidate,
                                      MultiEvaluation<>& candidateMev) {
    multiEval.reevaluateMEV(candidateMev, candidate);
    bool added = addSolutionWithMEV(p, candidate, candidateMev);

    return added;
  }

  bool checkDominance(const Pareto<XMES>& p) {
    Pareto<XMES> pFiltered;
    int nInd = p.size();
    for (int ind = 0; ind < nInd; ind++)
      // addSolutionWithMEV(pFiltered, p.getNonDominatedSol(ind),
      // p.getIndMultiEvaluation(ind));
      addSolutionWithMEV(pFiltered, p.getP(ind));

    if ((int)pFiltered.size() == nInd) return true;

    cout << "CheckDominance, inside MOSearch, found dominated solution inside "
            "the Pareto!"
         << endl;
    exit(1);
    return false;
  }

  //	virtual bool checkDominance(Pareto<XMES>& p, MultiEvaluation<>*
  // candidateMev, vector<MoveCost<>*>& candidateMovCost)
  //	{
  //		MultiEvaluation<>* tempMev = new MultiEvaluation(*candidateMev);
  //
  //		bool checkedValue = checkDominance(p, tempMev);
  //
  //		return checkedValue;
  //	}
  //
  //	virtual bool checkDominance(Pareto<XMES>& p, MultiEvaluation<>*
  // candidateMev)
  //	{
  //		for (int ind = 0; ind < x_e.size(); ind++)
  //		{
  //			MultiEvaluation<> popIndFitness =
  // x_e.getIndMultiEvaluation(ind); 			if
  // (domWeak.dominates(popIndFitness,
  //*candidateMev)) 				return false;
  //		}
  //		return true;
  //	}

  //	template<class T>
  //	bool addSolution(vector<T>& nonDom, T candidate)
  //	{
  //		for (int ind = 0; ind < nonDom.size(); ind++)
  //		{
  //			if (domWeak.dominates(nonDom[ind], candidate))
  //				return false;
  //
  //			if (dom.dominates(candidate, nonDom[ind]))
  //			{
  //				nonDom.erase(nonDom.begin() + ind);
  //				ind--;
  //			}
  //		}
  //
  //		nonDom.push_back(candidate);
  //		return true;
  //	}

  //	template<class T>
  //	bool addSolution(vector<T*>& nonDom, T* candidate)
  //	{
  //		for (int ind = 0; ind < nonDom.size(); ind++)
  //		{
  //			if (domWeak.dominates(*nonDom.at(ind), *candidate))
  //				return false;
  //
  //			if (dom.dominates(*candidate, *nonDom.at(ind)))
  //			{
  //				nonDom.erase(nonDom.begin() + ind);
  //				ind--;
  //			}
  //		}
  //
  //		nonDom.push_back(candidate);
  //		return true;
  //	}

};  // class paretoManager

}  // namespace optframe

#endif  // OPTFRAME_PARETOMANAGER_HPP_
