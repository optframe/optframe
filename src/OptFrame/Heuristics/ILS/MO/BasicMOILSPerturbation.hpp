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

#ifndef OPTFRAME_MOBASICILSPerturbation_HPP_
#define OPTFRAME_MOBASICILSPerturbation_HPP_

#include <math.h>
#include <vector>

#include "../../../NS.hpp"
#include "../../../RandGen.hpp"
#include "../../../ComponentBuilder.h"
#include "../../../MultiEvaluator.hpp"
#include "../../../MultiObjSearch.hpp"


#include "MOILS.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicMOILSPerturbation: public MOILS, public Component
{
private:
	MultiEvaluator<R, ADS>& mEval;
	int pMin;
	int pMax;
	vector<NS<R, ADS>*> ns;
	RandGen& rg;

public:
	BasicMOILSPerturbation(MultiEvaluator<R, ADS>& _mEval, int _pMin, int _pMax, vector<NS<R, ADS>*>& _ns, RandGen& _rg) :
		mEval(_mEval), pMin(_pMin), pMax(_pMax), ns(_ns), rg(_rg)
	{
		if(pMax < pMin)
		{
			cout << "BasicMOILSPerturbation warning: pMax > pMin! Swapping both." << endl;
			int aux = pMax;
			pMax = pMin;
			pMin = aux;
		}

		if(ns.size()==0)
			cout << "BasicMOILSPerturbation warning: empty neighborhood list." << endl;
	}

	BasicMOILSPerturbation(MultiEvaluator<R, ADS>& _mEval, int _pMin, int _pMax, NS<R, ADS>& _ns, RandGen& _rg) :
		mEval(_mEval), pMin(_pMin), pMax(_pMax), rg(_rg)
	{
		ns.push_back(&_ns);
		if(pMax < pMin)
		{
			cout << "BasicMOILSPerturbation warning: pMax > pMin! Swapping both." << endl;
			int aux = pMax;
			pMax = pMin;
			pMin = aux;
		}

		if(ns.size()==0)
			cout << "BasicMOILSPerturbation warning: empty neighborhood list." << endl;
	}


	virtual ~BasicMOILSPerturbation()
	{
	}

	void add_ns(NS<R, ADS>& _ns)
	{
		ns.push_back(&_ns);
	}

	void perturb(Solution<R, ADS>& s, MultiEvaluation& mev, MOSC& stopCriteria)
	{
		for (int i = pMin; i < pMax; i++)
		{
			int nk = rand() % ns.size();

			Move<R, ADS>* mp = ns[nk]->validRandomMoveSolution(s);

			if (!mp)
			{
				cout << "BasicMOILSPerturbation warning: perturbation found no valid move for neighborhood: ";
				ns[nk]->print();
			}
			else
			{
				Move<R, ADS>& m = *mp;
				Component::safe_delete(m.applyMEVUpdateSolution(mev, s));
				delete &m;
			}
		}

		mEval.reevaluateSolutionMEV(mev, s); // updates 'e'
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << ILS::family() << "MObasic_pert";
		return ss.str();
	}
};


}

#endif /*OPTFRAME_MOBASICILSPerturbation_HPP_*/
