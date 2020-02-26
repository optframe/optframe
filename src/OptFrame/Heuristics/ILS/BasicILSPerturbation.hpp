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

template<XSolution S, XEvaluation XEv = Evaluation<>>
class BasicILSPerturbation: public ILS, public Component
{
private:
	Evaluator<S, XEv>& evaluator;
	int pMin;
	int pMax;
	vector<NS<S, XEv>*> ns;
	RandGen& rg;

public:
	BasicILSPerturbation(Evaluator<S, XEv>& e, int _pMin, int _pMax, vector<NS<S, XEv>*>& _ns, RandGen& _rg) :
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

	BasicILSPerturbation(Evaluator<S, XEv>& e, int _pMin, int _pMax, NS<S, XEv>& _ns, RandGen& _rg) :
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

	void add_ns(NS<S, XEv>& _ns)
	{
		ns.push_back(&_ns);
	}

	void perturb(pair<S, XEv>& se, SOSC<XEv>& stopCriteria) // TODO: override?? what?
	{
      S& s = se.first;
      XEv& e = se.second;
      //
		for (int i = pMin; i < pMax; i++)
		{
			int nk = rand() % ns.size();

			Move<S, XEv>* mp = ns[nk]->validRandomMove(s);

			if (!mp)
			{
				cout << "BasicILSPerturbation warning: perturbation found no valid move for neighborhood: ";
				ns[nk]->print();
			}
			else
			{
				Move<S, XEv>& m = *mp;
            // TODO: fix with unique_ptr
				Component::safe_delete(m.applyUpdate(e, s));
				delete &m;
			}
		}

		evaluator.reevaluate(e, s); // updates 'e'
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

template<XSolution S, XEvaluation XEv = Evaluation<>, X2ESolution<S, XEv> X2ES = MultiESolution<S, XEv>>
class BasicILSPerturbationBuilder : public ComponentBuilder<S, XEv, X2ES>
{
public:
	virtual ~BasicILSPerturbationBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<S, XEv, X2ES>& hf, string family = "")
	{
		Evaluator<S, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		int pMin = scanner.nextInt();
		int pMax = scanner.nextInt();

		vector<NS<S, XEv>*> ns_list;
		hf.assignList(ns_list, scanner.nextInt(), scanner.next()); // reads backwards!

		return new BasicILSPerturbation<S, XEv>(*eval, pMin, pMax, ns_list, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<S, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair("OptFrame:int", "pMin: min number of moves"));
		params.push_back(make_pair("OptFrame:int", "pMax: max number of moves"));
		stringstream ss;
		ss << NS<S, XEv>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of neighborhood structures"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicILSPerturbation<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<S, XEv, X2ES>::idComponent() << ILS::family() << "basic_pert";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BASICILSPerturbation_HPP_*/
