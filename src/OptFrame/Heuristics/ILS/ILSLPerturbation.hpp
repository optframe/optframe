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

#ifndef OPTFRAME_ILSLPerturbation_HPP_
#define OPTFRAME_ILSLPerturbation_HPP_

#include <math.h>
#include <vector>

#include "../../NS.hpp"
#include "../../RandGen.hpp"

#include "ILS.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class ILSLPerturbation: public Component, public ILS
{
public:

	virtual ~ILSLPerturbation()
	{
	}

	virtual void perturb(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria, int level) = 0;

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ILS::family() << "LevelPert";
		return ss.str();

	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class ILSLPerturbationLPlus2: public ILSLPerturbation<R, ADS>
{
private:
	vector<NS<R, ADS>*> ns;
	Evaluator<R, ADS>& evaluator;
	RandGen& rg;

public:
	ILSLPerturbationLPlus2(Evaluator<R, ADS>& e, NS<R, ADS>& _ns, RandGen& _rg) :
			evaluator(e), rg(_rg)
	{
		ns.push_back(&_ns);
	}

	virtual ~ILSLPerturbationLPlus2()
	{
	}

	void add_ns(NS<R, ADS>& _ns)
	{
		ns.push_back(&_ns);
	}

	void perturb(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria, int level)
	{
		int a = 0; // number of appliable moves

		level += 2; // level 0 applies 2 moves

		while (a < level)
		{
			int x = rg.rand(ns.size());

			Move<R, ADS>* m = ns[x]->validRandomMoveSolution(s);

			if (m)
			{
				a++;
				Component::safe_delete(m->applyUpdateSolution(e, s));
			}
			else
				if(Component::warning)
					cout << "ILS Warning: perturbation had no effect in level " << a << "!" << endl;

			delete m;
		}

		evaluator.reevaluateSolution(e, s); // updates 'e'
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (ILSLPerturbation<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ILSLPerturbation<R, ADS>::idComponent() << ":LPlus2";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class ILSLPerturbationLPlus2Prob: public ILSLPerturbation<R, ADS>
{
private:
	vector<NS<R, ADS>*> ns;
	vector<pair<int, double> > pNS;
	Evaluator<R, ADS>& evaluator;
	RandGen& rg;

public:
	ILSLPerturbationLPlus2Prob(Evaluator<R, ADS>& e, NS<R, ADS>& _ns, RandGen& _rg) :
			evaluator(e), rg(_rg)
	{
		ns.push_back(&_ns);
		pNS.push_back(make_pair(1, 1));
	}

	virtual ~ILSLPerturbationLPlus2Prob()
	{
	}

	void add_ns(NS<R, ADS>& _ns)
	{
		ns.push_back(&_ns);
		pNS.push_back(make_pair(1, 1));

		double soma = 0;
		for (int i = 0; i < ns.size(); i++)
			soma += pNS[i].first;
		for (int i = 0; i < ns.size(); i++)
			pNS[i].second = pNS[i].first / soma;
	}

	void changeProb(vector<int> pri)
	{
		int nNeighborhoods = ns.size();
		if (pri.size() != nNeighborhoods)
		{
			cout << "ERROR ON PRIORITES SIZE!" << endl;
			return;
		}

		double soma = 0;
		for (int i = 0; i < nNeighborhoods; i++)
		{
			pNS[i].first = pri[i];
			soma += pri[i];
		}

		for (int i = 0; i < ns.size(); i++)
			pNS[i].second = pNS[i].first / soma;

		cout<<"Printing probabilities ILSLPerturbationLPlus2Prob:"<<endl;
		for (int i = 0; i < ns.size(); i++)
			cout << "pNS[i].first: " << pNS[i].first << "\t pNS[i].second: " << pNS[i].second << endl;
		cout<<endl;
	}

	void perturb(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria, int level)
	{
		int a = 0; // number of appliable moves

		level += 2; // level 0 applies 2 moves

		while (a < level)
		{
			double prob = rg.rand01();
			int x = 0;
			double sum = pNS[x].second;

			while (prob > sum)
			{
				x++;
				sum += pNS[x].second;
			}

			Move<R, ADS>* m = ns[x]->validMove(s);

			if (m)
			{
				a++;
				Component::safe_delete(m->apply(e, s));
			}
			else
				if(Component::warning)
					cout << "ILS Warning: perturbation had no effect in level " << a << "!" << endl;

			delete m;
		}

		evaluator.evaluate(e, s); // updates 'e'
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ILSLPerturbation<R, ADS>::idComponent() << ":LPlus2Prob";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class ILSLPerturbationLPlus2Builder: public ComponentBuilder<R, ADS>
{
public:
	virtual ~ILSLPerturbationLPlus2Builder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NS<R, ADS>* ns;
		hf.assign(ns, scanner.nextInt(), scanner.next()); // reads backwards!

		return new ILSLPerturbationLPlus2<R, ADS>(*eval, *ns, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NS<R, ADS>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == ILSLPerturbationLPlus2<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << ILS::family() << "LevelPert:LPlus2";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class ILSLPerturbationLPlus2ProbBuilder: public ComponentBuilder<R, ADS>
{
public:
	virtual ~ILSLPerturbationLPlus2ProbBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NS<R, ADS>* ns;
		hf.assign(ns, scanner.nextInt(), scanner.next()); // reads backwards!

		return new ILSLPerturbationLPlus2Prob<R, ADS>(*eval, *ns, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NS<R, ADS>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == ILSLPerturbationLPlus2Prob<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << ILS::family() << "LevelPert:LPlus2Prob";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_ILSLPerturbation_HPP_*/
