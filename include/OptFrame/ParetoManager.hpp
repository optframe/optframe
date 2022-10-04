// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_PARETO_MANAGER_HPP
#define OPTFRAME_PARETO_MANAGER_HPP

#include <cstring>
#include <iostream>
#include <vector>

#include "Pareto.hpp"

namespace optframe {

template <XSolution S, XEvaluation XMEv = MultiEvaluation<>, XESolution XMES = pair<S, XMEv>>
class paretoManager {
  using XEv = Evaluation<>;  // hardcoding this... TODO: solve by having a GeneralEvaluator down here!
 public:
  MultiEvaluator<XMES>& multiEval;
  //GeneralEvaluator<XMES, XMEv>& multiEval;
  //MultiEvaluator<S, XEv>& mev; // cannot be this, for now!
  ParetoDominance<S, XEv, XMEv> dom;
  ParetoDominanceWeak<S, XEv, XMEv> domWeak;
  //	Pareto<XMES> x_e;

 public:
  paretoManager(MultiEvaluator<XMES>& _multiEval)
      :  //paretoManager(GeneralEvaluator<XMES, XMEv>& _multiEval) : // cannot be this, for now!
        multiEval(_multiEval),
        dom(ParetoDominance<S, XEv, XMEv>(_multiEval)),
        domWeak(ParetoDominanceWeak<S, XEv, XMEv>(_multiEval)) {
  }

  virtual ~paretoManager() {
  }

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
  //	virtual bool addSolution(const S* candidate, const MultiEvaluation<>* mev)
  //	{
  //		cout << "Something wrong has happen! \n It is inside addSolution candidate,mev! \n This should be reimplemented" << endl;
  //		getchar();
  //		exit(1);
  ////		return false;
  //	}

  bool addSolution(Pareto<XMES>& p, const S& candidate) {
    MultiEvaluation<> mev = multiEval.evaluate(candidate);
    XMES cand_smev = make_pair(candidate, mev);
    //bool added = addSolutionWithMEV(p, candidate, mev);
    bool added = addSolutionWithMEV(p, cand_smev);

    return added;
  }

  //virtual bool addSolutionWithMEV(Pareto<XMES>& p, const S& candidate, const MultiEvaluation<>& candidateMev)
  virtual bool addSolutionWithMEV(Pareto<XMES>& p, const XMES& cand_smev) {
    bool added = true;
    for (int ind = 0; ind < (int)p.size(); ind++) {
      const MultiEvaluation<>& popIndFitness = p.getIndMultiEvaluation(ind);

      if (domWeak.dominates(popIndFitness, cand_smev.second))
        return false;

      if (dom.dominates(cand_smev.second, popIndFitness)) {
        p.erase(ind);
        ind--;
      }
    }
    if (added)
      //p.add_indWithMev(candidate, candidateMev);
      p.add_indWithMev(cand_smev);

    return added;
  }

  bool addSolutionWithMEVReevaluation(Pareto<XMES>& p, S& candidate, MultiEvaluation<>& candidateMev) {
    multiEval.reevaluateMEV(candidateMev, candidate);
    bool added = addSolutionWithMEV(p, candidate, candidateMev);

    return added;
  }

  bool checkDominance(const Pareto<XMES>& p) {
    Pareto<XMES> pFiltered;
    int nInd = p.size();
    for (int ind = 0; ind < nInd; ind++)
      //addSolutionWithMEV(pFiltered, p.getNonDominatedSol(ind), p.getIndMultiEvaluation(ind));
      addSolutionWithMEV(pFiltered, p.getP(ind));

    if ((int)pFiltered.size() == nInd)
      return true;

    cout << "CheckDominance, inside MOSearch, found dominated solution inside the Pareto!" << endl;
    exit(1);
    return false;
  }

  //	virtual bool checkDominance(Pareto<XMES>& p, MultiEvaluation<>* candidateMev, vector<MoveCost<>*>& candidateMovCost)
  //	{
  //		MultiEvaluation<>* tempMev = new MultiEvaluation(*candidateMev);
  //
  //		bool checkedValue = checkDominance(p, tempMev);
  //
  //		return checkedValue;
  //	}
  //
  //	virtual bool checkDominance(Pareto<XMES>& p, MultiEvaluation<>* candidateMev)
  //	{
  //		for (int ind = 0; ind < x_e.size(); ind++)
  //		{
  //			MultiEvaluation<> popIndFitness = x_e.getIndMultiEvaluation(ind);
  //			if (domWeak.dominates(popIndFitness, *candidateMev))
  //				return false;
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

#endif  // OPTFRAME_PARETO_MANAGER_HPP
