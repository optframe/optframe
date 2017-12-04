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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicMOILS: public MultiObjILS<BasicHistory, R, ADS>
{

private:
	BasicMOILSPerturbation<R, ADS>& p;
	int iterMax;

public:

	BasicMOILS(MultiEvaluator<R, ADS>& _mev, InitialPareto<R, ADS>& _init_pareto, int _init_pop_size, MOLocalSearch<R, ADS>* _ls, RandGen& _rg, BasicMOILSPerturbation<R, ADS>& _p, int _iterMax) :
		MultiObjILS<BasicHistory, R, ADS >(_mev,_init_pareto,_init_pop_size,_ls,_rg), p(_p), iterMax(_iterMax)
	{

	}

	virtual ~BasicMOILS()
	{
	}

	virtual BasicHistory& initializeHistory()
	{
		int& iter = *new int;
		iter = 0;

		return iter;
	}

	virtual void perturbation(Solution<R, ADS>& s, MultiEvaluation& e, MOSC& stopCriteria, BasicHistory& history)
	{
		int iter = history;

		p.perturb(s, e, stopCriteria);

		// Incrementa a iteracao
		iter++;

		// Atualiza o historico
		history = iter;
	}

	virtual void acceptanceCriterion(const Pareto<R, ADS>& pf, BasicHistory& history)
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

	virtual bool terminationCondition(BasicHistory& history)
	{
		int iter = history;

		return (iter >= iterMax);
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (MultiObjSearch<R, ADS>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << MultiObjILS<BasicHistory, R, ADS>::idComponent() << "BasicMOILS";
		return ss.str();
	}

};

}

#endif /*OPTFRAME_BasicMOILS_HPP_*/
