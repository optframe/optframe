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

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class BasicIteratedLocalSearch: public IteratedLocalSearch<BasicHistory, XES, XEv>
{
protected:
	LocalSearch<XES, XEv>& ls;
	BasicILSPerturbation<XES, XEv>& p;
	int iterMax;

public:

	//BasicIteratedLocalSearch(Evaluator<XES, XEv>& e, Constructive<S>& constructive, LocalSearch<XES, XEv>& _ls, BasicILSPerturbation<S, XEv>& _p, int _iterMax) :
   BasicIteratedLocalSearch(Evaluator<XES, XEv>& e, InitialSearch<XES>& constructive, LocalSearch<XES, XEv>& _ls, BasicILSPerturbation<XES, XEv>& _p, int _iterMax) :
		IteratedLocalSearch<BasicHistory, XES, XEv> (e, constructive), ls(_ls), p(_p), iterMax(_iterMax)
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

	virtual void localSearch(XES& se, const StopCriteria<XES>& sosc) override
	{
		ls.searchFrom(se, sosc);
	}

	virtual void perturbation(XES& se, const StopCriteria<XES>& sosc, BasicHistory& history) override
	{
		int iter = history;

		p.perturb(se, sosc);

		// Incrementa a iteracao
		iter++;

		// Atualiza o historico
		history = iter;
	}

	virtual bool acceptanceCriterion(const Evaluation<>& e1, const Evaluation<>& e2, BasicHistory& history) override
	{
		if (IteratedLocalSearch<BasicHistory, XES, XEv>::evaluator.betterThan(e1, e2))
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
		return (s == idComponent()) || (SingleObjSearch<XES>::compatible(s));
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << IteratedLocalSearch<BasicHistory, XES, XEv>::idComponent() << "BasicILS";
		return ss.str();
	}
};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class BasicIteratedLocalSearchBuilder : public ILS, public SingleObjSearchBuilder<S, XEv, XES>
{
public:
	virtual ~BasicIteratedLocalSearchBuilder()
	{
	}

	virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "") override
	{
		Evaluator<XES, XEv>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<S>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		string rest = scanner.rest();

		pair<LocalSearch<XES, XEv>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<XES, XEv>* h = method.first;

		scanner = Scanner(method.second);

		BasicILSPerturbation<S, XEv>* pert;
		hf.assign(pert, scanner.nextInt(), scanner.next()); // reads backwards!

		int iterMax = scanner.nextInt();

		return new BasicIteratedLocalSearch<XES, XEv>(*eval, *constructive, *h, *pert, iterMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
		params.push_back(make_pair(LocalSearch<XES, XEv>::idComponent(), "local search"));
		params.push_back(make_pair(BasicILSPerturbation<S, XEv>::idComponent(), "ils perturbation"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations without improvement"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicIteratedLocalSearch<XES, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<S, XEv>::idComponent() << ":" << ILS::family() << "BasicILS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_BASICILS_HPP_*/
