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

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class BasicIteratedLocalSearch: public IteratedLocalSearch<BasicHistory, R, ADS, S>
{
protected:
	LocalSearch<R, ADS, S>& ls;
	BasicILSPerturbation<R, ADS, S>& p;
	int iterMax;

public:

	BasicIteratedLocalSearch(Evaluator<R, ADS, S>& e, Constructive<S>& constructive, LocalSearch<R, ADS, S>& _ls, BasicILSPerturbation<R, ADS, S>& _p, int _iterMax) :
		IteratedLocalSearch<BasicHistory, R, ADS, S> (e, constructive), ls(_ls), p(_p), iterMax(_iterMax)
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

	virtual void localSearch(S& s, Evaluation& e, SOSC& sosc) override
	{
		ls.exec(s, e, sosc);
	}

	virtual void perturbation(S& s, Evaluation& e, SOSC& sosc, BasicHistory& history) override
	{
		int iter = history;

		p.perturb(s, e, sosc);

		// Incrementa a iteracao
		iter++;

		// Atualiza o historico
		history = iter;
	}

	virtual bool acceptanceCriterion(const Evaluation& e1, const Evaluation& e2, BasicHistory& history)
	{
		if (IteratedLocalSearch<BasicHistory, R, ADS, S>::evaluator.betterThan(e1, e2))
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
		return (s == idComponent()) || (SingleObjSearch<R, ADS, S>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << IteratedLocalSearch<BasicHistory, R, ADS, S>::idComponent() << "BasicILS";
		return ss.str();
	}
};

template<Representation R, Structure ADS = _ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
class BasicIteratedLocalSearchBuilder : public ILS, public SingleObjSearchBuilder<R, ADS, S>
{
public:
	virtual ~BasicIteratedLocalSearchBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
	{
		Evaluator<R, ADS, S>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<S>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS, S>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS, S>* h = method.first;

		scanner = Scanner(method.second);

		BasicILSPerturbation<R, ADS, S>* pert;
		hf.assign(pert, scanner.nextInt(), scanner.next()); // reads backwards!

		int iterMax = scanner.nextInt();

		return new BasicIteratedLocalSearch<R, ADS, S>(*eval, *constructive, *h, *pert, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, S>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
		params.push_back(make_pair(LocalSearch<R, ADS, S>::idComponent(), "local search"));
		params.push_back(make_pair(BasicILSPerturbation<R, ADS, S>::idComponent(), "ils perturbation"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations without improvement"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicIteratedLocalSearch<R, ADS, S>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS, S>::idComponent() << ":" << ILS::family() << "BasicILS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BASICILS_HPP_*/
