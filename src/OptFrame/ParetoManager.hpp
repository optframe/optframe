// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_PARETO_MANAGER_HPP
#define OPTFRAME_PARETO_MANAGER_HPP

#include <iostream>
#include <vector>
#include <cstring>

#include "Pareto.hpp"

namespace optframe
{


template<XSolution S, XEvaluation XMEv = MultiEvaluation<>, XESolution XMES = pair<S, XMEv>>
class paretoManager
{
   using XEv = Evaluation<>; // hardcoding this... TODO: solve by having a GeneralEvaluator down here!
public:
	MultiEvaluator<S, XEv, XMEv, XMES>& multiEval;
   //GeneralEvaluator<XMES, XMEv>& multiEval;
   //MultiEvaluator<S, XEv>& mev; // cannot be this, for now!
	ParetoDominance<S, XEv, XMEv> dom;
	ParetoDominanceWeak<S, XEv, XMEv> domWeak;
//	Pareto<S, XMEv> x_e;

public:

	paretoManager(MultiEvaluator<S, XEv, XMEv, XMES>& _multiEval) :
   //paretoManager(GeneralEvaluator<XMES, XMEv>& _multiEval) : // cannot be this, for now!
			multiEval(_multiEval), dom(ParetoDominance<S, XEv, XMEv>(_multiEval)), domWeak(ParetoDominanceWeak<S, XEv, XMEv>(_multiEval))
	{
	}

	virtual ~paretoManager()
	{

	}

//	virtual Pareto<S, XMEv>& getParetoInsideManager()
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

	bool addSolution(Pareto<S, XMEv>& p, const S& candidate)
	{
		MultiEvaluation<> mev = multiEval.evaluate(candidate);
      XMES cand_smev = make_pair(candidate, mev);
		//bool added = addSolutionWithMEV(p, candidate, mev);
      bool added = addSolutionWithMEV(p, cand_smev);

		return added;
	}

	//virtual bool addSolutionWithMEV(Pareto<S, XMEv>& p, const S& candidate, const MultiEvaluation<>& candidateMev)
   virtual bool addSolutionWithMEV(Pareto<S, XMEv>& p, const XMES& cand_smev)
	{
		bool added = true;
		for (int ind = 0; ind < (int) p.size(); ind++)
		{
			const MultiEvaluation<>& popIndFitness = p.getIndMultiEvaluation(ind);

			if (domWeak.dominates(popIndFitness, cand_smev.second))
				return false;

			if (dom.dominates(cand_smev.second, popIndFitness))
			{
				p.erase(ind);
				ind--;
			}

		}
		if (added)
			//p.add_indWithMev(candidate, candidateMev);
         p.add_indWithMev(cand_smev);

		return added;
	}

	bool addSolutionWithMEVReevaluation(Pareto<S, XMEv>& p, S& candidate, MultiEvaluation<>& candidateMev)
	{
		multiEval.reevaluateMEV(candidateMev, candidate);
		bool added = addSolutionWithMEV(p, candidate, candidateMev);

		return added;
	}

	bool checkDominance(const Pareto<S, XMEv>& p)
	{
		Pareto<S, XMEv> pFiltered;
		int nInd = p.size();
		for (int ind = 0; ind < nInd; ind++)
			//addSolutionWithMEV(pFiltered, p.getNonDominatedSol(ind), p.getIndMultiEvaluation(ind));
         addSolutionWithMEV(pFiltered, p.getP(ind));

		if ((int) pFiltered.size() == nInd)
			return true;

		cout << "CheckDominance, inside MOSearch, found dominated solution inside the Pareto!" << endl;
		exit(1);
		return false;
	}

//	virtual bool checkDominance(Pareto<S, XMEv>& p, MultiEvaluation<>* candidateMev, vector<MoveCost<>*>& candidateMovCost)
//	{
//		MultiEvaluation<>* tempMev = new MultiEvaluation(*candidateMev);
//
//		bool checkedValue = checkDominance(p, tempMev);
//
//		return checkedValue;
//	}
//
//	virtual bool checkDominance(Pareto<S, XMEv>& p, MultiEvaluation<>* candidateMev)
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

}; // class paretoManager


} // namespace optframe

#endif // OPTFRAME_PARETO_MANAGER_HPP
