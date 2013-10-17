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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BasicILSPerturbation: public ILS, public Component
{
private:
	Evaluator<R, ADS, DS>& evaluator;
	int pMin;
	int pMax;
	int limit;
	vector<NS<R, ADS, DS>*> ns;
	RandGen& rg;

public:
	BasicILSPerturbation(Evaluator<R, ADS, DS>& e, int _pMin, int _pMax, int _limit, vector<NS<R, ADS, DS>*>& _ns, RandGen& _rg) :
		evaluator(e), pMin(_pMin), pMax(_pMax), limit(_limit), ns(_ns), rg(_rg)
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

	virtual ~BasicILSPerturbation()
	{
	}

	void add_ns(NS<R, ADS, DS>& _ns)
	{
		ns.push_back(&_ns);
	}

	void perturb(Solution<R, ADS>& s, Evaluation<DS>& e, double timelimit, double target_f)
	{
		for (int i = pMin; i < pMax; i++)
		{
			int nk = rand() % ns.size();

			int f = 0; // number of failures
			while (f < limit)
			{
				Move<R, ADS, DS>& m = ns[nk]->move(s);

				if (m.canBeApplied(s))
				{
					delete &m.apply(e, s);
					delete &m;
					break;
				}
				else
					f++;

				delete &m;
			}

			if (f == limit)
			{
				cout << "BasicILSPerturbation warning: perturbation had no effect with " << limit << " failures (!canBeApplied) for neighborhood :";
				ns[nk]->print();
			}
		}

		evaluator.evaluate(e, s); // updates 'e'
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ILS::family() << "basic_pert";
		return ss.str();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BasicILSPerturbationBuilder : public ComponentBuilder<R, ADS, DS>
{
public:
	virtual ~BasicILSPerturbationBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		int pMin = scanner.nextInt();
		int pMax = scanner.nextInt();
		int limit = scanner.nextInt();

		vector<NS<R, ADS, DS>*> ns_list;
		hf.assignList(ns_list, scanner.nextInt(), scanner.next()); // reads backwards!

		return new BasicILSPerturbation<R, ADS, DS>(*eval, pMin, pMax, limit, ns_list, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		params.push_back(make_pair("int", "pMin: min number of moves"));
		params.push_back(make_pair("int", "pMax: max number of moves"));
		params.push_back(make_pair("int", "limit: max number of failures for canBeApplied"));
		//params.push_back(make_pair(NS<R, ADS, DS>::idComponent(), "neighborhood structure"));
		stringstream ss;
		ss << NS<R, ADS, DS>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of neighborhood structures"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicILSPerturbation<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS, DS>::idComponent() << ILS::family() << "basic_pert";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BASICILSPerturbation_HPP_*/
