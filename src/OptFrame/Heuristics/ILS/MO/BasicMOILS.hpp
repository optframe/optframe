// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_BasicMOILS_HPP_
#define OPTFRAME_BasicMOILS_HPP_

#include <algorithm>

#include "MultiObjILS.hpp"
#include "BasicMOILSPerturbation.hpp"


namespace optframe
{

typedef int BasicHistory;

template<XSolution S, XEvaluation XMEv=MultiEvaluation<>, XESolution XMES = pair<S, XMEv>>
//template<XESolution XMES, XEvaluation XMEv=MultiEvaluation<>>
class BasicMOILS: public MultiObjILS<BasicHistory, S, XMEv, XMES>
{
   using XEv = Evaluation<>; // hardcoded.. TODO: fix
private:
	BasicMOILSPerturbation<XMES, XMEv>& p;
	int iterMax;

public:

   BasicMOILS(MultiEvaluator<S, XEv, XMEv, XMES>& _mev, InitialPareto<S, XMEv, XMES>& _init_pareto, int _init_pop_size, MOLocalSearch<S, XMEv>* _ls, RandGen& _rg, BasicMOILSPerturbation<XMES, XMEv>& _p, int _iterMax) :
	//BasicMOILS(GeneralEvaluator<XMES, XMEv>& _mev, InitialPareto<S, XMEv, XMES>& _init_pareto, int _init_pop_size, MOLocalSearch<S, XMEv>* _ls, RandGen& _rg, BasicMOILSPerturbation<XMES, XMEv>& _p, int _iterMax) :
		MultiObjILS<BasicHistory, S, XMEv, XMES>(_mev,_init_pareto,_init_pop_size,_ls,_rg), p(_p), iterMax(_iterMax)
	{
	}

	virtual ~BasicMOILS()
	{
	}

	virtual BasicHistory& initializeHistory() override
	{
		int& iter = *new int;
		iter = 0;

		return iter;
	}

	virtual void perturbation(XMES& smev, const StopCriteria<XMEv>& stopCriteria, BasicHistory& history) override
	{
		int iter = history;

		p.perturb(smev, stopCriteria);

		// Incrementa a iteracao
		iter++;

		// Atualiza o historico
		history = iter;
	}

	virtual void acceptanceCriterion(const Pareto<S, XMEv>& pf, BasicHistory& history) override
	{

		if (pf.getNewNonDominatedSolutionsStatus())
		{
			cout << "New Pareto size: is "<<pf.size();
			cout << " on [iter without improvement " << history << "]" << endl;

			// =======================
			//  Atualiza o historico
			// =======================
			history = 0;
		}
	}

	virtual bool terminationCondition(BasicHistory& history) override
	{
		int iter = history;

		return (iter >= iterMax);
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (MultiObjSearch<S, XMEv>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << MultiObjILS<BasicHistory, S, XMEv>::idComponent() << "BasicMOILS";
		return ss.str();
	}

};

}

#endif /*OPTFRAME_BasicMOILS_HPP_*/
