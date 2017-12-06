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

#ifndef OPTFRAME_BASICILS_HPP_
#define OPTFRAME_BASICILS_HPP_

#include <math.h>
#include <vector>

#include "../../LocalSearch.hpp"

#include "IteratedLocalSearch.hpp"
#include "BasicILSPerturbation.hpp"

#include "ILS.h"

namespace optframe
{

typedef int BasicHistory;

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicIteratedLocalSearch: public IteratedLocalSearch<BasicHistory, R, ADS>
{
protected:
	LocalSearch<R, ADS>& ls;
	BasicILSPerturbation<R, ADS>& p;
	int iterMax;

public:

	BasicIteratedLocalSearch(Evaluator<R, ADS>& e, Constructive<R, ADS>& constructive, LocalSearch<R, ADS>& _ls, BasicILSPerturbation<R, ADS>& _p, int _iterMax) :
		IteratedLocalSearch<BasicHistory, R, ADS > (e, constructive), ls(_ls), p(_p), iterMax(_iterMax)
	{
	}

	virtual ~BasicIteratedLocalSearch()
	{
	}

	virtual BasicHistory& initializeHistory()
	{
		int& iter = *new int;
		iter = 0;

		return iter;
	}

	virtual void localSearch(Solution<R, ADS>& s, Evaluation& e, double timelimit, double target_f)
	{
		ls.exec(s, e, timelimit, target_f);
	}

	virtual void perturbation(Solution<R, ADS>& s, Evaluation& e, double timelimit, double target_f, BasicHistory& history)
	{
		int iter = history;

		p.perturb(s, e, timelimit, target_f);

		// Incrementa a iteracao
		iter++;

		// Atualiza o historico
		history = iter;
	}

	virtual bool acceptanceCriterion(const Evaluation& e1, const Evaluation& e2, BasicHistory& history)
	{
		if (IteratedLocalSearch<BasicHistory, R, ADS >::evaluator.betterThan(e1, e2))
		{
			// =======================
			//   Melhor solucao: 's2'
			// =======================
			cout << "Best fo: " << e1.evaluation();
			cout << " on [iter " << history << "]" << endl;

			// =======================
			//  Atualiza o historico
			// =======================
			// iter = 0
			history = 0;

			// =======================
			//    Retorna s2
			// =======================
			return true;
		}
		else
			return false;
	}

	virtual bool terminationCondition(BasicHistory& history)
	{
		int iter = history;

		return (iter >= iterMax);
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (SingleObjSearch<R, ADS>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << IteratedLocalSearch<BasicHistory, R, ADS >::idComponent() << "BasicILS";
		return ss.str();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicIteratedLocalSearchBuilder : public ILS, public SingleObjSearchBuilder<R, ADS>
{
public:
	virtual ~BasicIteratedLocalSearchBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<R, ADS>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS>* h = method.first;

		scanner = Scanner(method.second);

		BasicILSPerturbation<R, ADS>* pert;
		hf.assign(pert, scanner.nextInt(), scanner.next()); // reads backwards!

		int iterMax = scanner.nextInt();

		return new BasicIteratedLocalSearch<R, ADS>(*eval, *constructive, *h, *pert, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<R, ADS>::idComponent(), "constructive heuristic"));
		params.push_back(make_pair(LocalSearch<R, ADS>::idComponent(), "local search"));
		params.push_back(make_pair(BasicILSPerturbation<R, ADS>::idComponent(), "ils perturbation"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations without improvement"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicIteratedLocalSearch<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS>::idComponent() << ":" << ILS::family() << "BasicILS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BASICILS_HPP_*/
