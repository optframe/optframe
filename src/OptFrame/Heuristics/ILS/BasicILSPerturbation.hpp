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

#ifndef OPTFRAME_BASICILSPerturbation_HPP_
#define OPTFRAME_BASICILSPerturbation_HPP_

#include <math.h>
#include <vector>

#include "../../NS.hpp"
#include "../../RandGen.hpp"
#include "../../ComponentBuilder.h"

#include "ILS.h"

namespace optframe
{

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class BasicILSPerturbation: public ILS, public Component
{
private:
	Evaluator<R, ADS, S>& evaluator;
	int pMin;
	int pMax;
	vector<NS<R, ADS, S>*> ns;
	RandGen& rg;

public:
	BasicILSPerturbation(Evaluator<R, ADS, S>& e, int _pMin, int _pMax, vector<NS<R, ADS, S>*>& _ns, RandGen& _rg) :
		evaluator(e), pMin(_pMin), pMax(_pMax), ns(_ns), rg(_rg)
	{
		if(pMax < pMin)
		{
			cout << "BasicILSPerturbation warning: pMax > pMin! Swapping both." << endl;
			int aux = pMax;
			pMax = pMin;
			pMin = aux;
		}

		if(ns.size()==0)
			cout << "BasicILSPerturbation warning: empty neighborhood list." << endl;
	}

	BasicILSPerturbation(Evaluator<R, ADS, S>& e, int _pMin, int _pMax, NS<R, ADS, S>& _ns, RandGen& _rg) :
		evaluator(e), pMin(_pMin), pMax(_pMax), rg(_rg)
	{
		ns.push_back(&_ns);
		if(pMax < pMin)
		{
			cout << "BasicILSPerturbation warning: pMax > pMin! Swapping both." << endl;
			int aux = pMax;
			pMax = pMin;
			pMin = aux;
		}

		if(ns.size()==0)
			cout << "BasicILSPerturbation warning: empty neighborhood list." << endl;
	}


	virtual ~BasicILSPerturbation()
	{
	}

	void add_ns(NS<R, ADS, S>& _ns)
	{
		ns.push_back(&_ns);
	}

	void perturb(S& s, Evaluation<>& e, SOSC& stopCriteria)
	{
		for (int i = pMin; i < pMax; i++)
		{
			int nk = rand() % ns.size();

			Move<R, ADS, S>* mp = ns[nk]->validRandomMoveSolution(s);

			if (!mp)
			{
				cout << "BasicILSPerturbation warning: perturbation found no valid move for neighborhood: ";
				ns[nk]->print();
			}
			else
			{
				Move<R, ADS, S>& m = *mp;
				Component::safe_delete(m.applyUpdateSolution(e, s));
				delete &m;
			}
		}

		evaluator.reevaluateSolution(e, s); // updates 'e'
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << ILS::family() << "basic_pert";
		return ss.str();
	}
};

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class BasicILSPerturbationBuilder : public ComponentBuilder<R, ADS, S>
{
public:
	virtual ~BasicILSPerturbationBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
	{
		Evaluator<R, ADS, S>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		int pMin = scanner.nextInt();
		int pMax = scanner.nextInt();

		vector<NS<R, ADS, S>*> ns_list;
		hf.assignList(ns_list, scanner.nextInt(), scanner.next()); // reads backwards!

		return new BasicILSPerturbation<R, ADS, S>(*eval, pMin, pMax, ns_list, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, S>::idComponent(), "evaluation function"));
		params.push_back(make_pair("OptFrame:int", "pMin: min number of moves"));
		params.push_back(make_pair("OptFrame:int", "pMax: max number of moves"));
		stringstream ss;
		ss << NS<R, ADS, S>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of neighborhood structures"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicILSPerturbation<R, ADS, S>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS, S>::idComponent() << ILS::family() << "basic_pert";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BASICILSPerturbation_HPP_*/
