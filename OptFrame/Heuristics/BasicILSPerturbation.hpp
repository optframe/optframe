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

#include "../NS.hpp"
#include "../RandGen.hpp"
#include "../ComponentBuilder.h"

#include "ILS.h"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BasicILSPerturbation: public ILS, public OptFrameComponent
{
private:
	vector<NS<R, ADS, DS>*> ns;
	Evaluator<R, ADS, DS>& evaluator;
	int pMax;
	RandGen& rg;

public:
	BasicILSPerturbation(Evaluator<R, ADS, DS>& e, int _pMax, NS<R, ADS, DS>& _ns, RandGen& _rg) :
		evaluator(e), pMax(_pMax), rg(_rg)
	{
		ns.push_back(&_ns);
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
		int f = 0; // number of failures

		while (f < pMax)
		{
			int x = rg.rand(ns.size());

			Move<R, ADS, DS>& m = ns[x]->move(s);

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

		if (f == pMax)
			cout << "ILS Warning: perturbation had no effect in " << pMax << " tries!" << endl;

		evaluator.evaluate(e, s); // updates 'e'
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << OptFrameComponent::idComponent() << ILS::family << "basic_pert";
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

	virtual OptFrameComponent* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		Evaluator<R, ADS, DS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		int limit = scanner.nextInt();

		NS<R, ADS, DS>* ns;
		hf.assign(ns, scanner.nextInt(), scanner.next()); // reads backwards!

		return new BasicILSPerturbation<R, ADS, DS>(*eval, limit, *ns, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, DS>::idComponent(), "evaluation function"));
		params.push_back(make_pair("int", "max number of not appliable moves"));
		params.push_back(make_pair(NS<R, ADS, DS>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicILSPerturbation<R, ADS, DS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS, DS>::idComponent() << ILS::family << "basic_pert";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

#endif /*OPTFRAME_BASICILSPerturbation_HPP_*/
