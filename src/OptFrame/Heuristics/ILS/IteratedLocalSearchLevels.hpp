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

#ifndef OPTFRAME_ILSL_HPP_
#define OPTFRAME_ILSL_HPP_

#include <math.h>
#include <vector>

#include "IteratedLocalSearch.hpp"
#include "ILSLPerturbation.hpp"

#include "ILS.h"

namespace optframe
{

typedef pair<pair<int, int> , pair<int, int> > levelHistory;

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class IteratedLocalSearchLevels: public IteratedLocalSearch<levelHistory, R, ADS >
{
protected:
	LocalSearch<R, ADS>& ls;
	ILSLPerturbation<R, ADS>& p;
	int iterMax, levelMax;

public:

	IteratedLocalSearchLevels(Evaluator<R, ADS>& e, Constructive<R, ADS>& constructive, LocalSearch<R, ADS>& _ls, ILSLPerturbation<R, ADS>& _p, int _iterMax, int _levelMax) :
		IteratedLocalSearch<levelHistory, R, ADS > (e, constructive), ls(_ls), p(_p), iterMax(_iterMax), levelMax(_levelMax)
	{
	}

	virtual ~IteratedLocalSearchLevels()
	{
	}

	virtual levelHistory& initializeHistory()
	{
		//cout << "initializeHistory()" << endl;
		pair<int, int> vars(0, 0);

		// IterMax e LevelMax
		pair<int, int> maxs(iterMax, levelMax);

		return *new levelHistory(vars, maxs);
	}

	virtual void localSearch(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria)
	{
		//cout << "localSearch(.)" << endl;
		ls.exec(s, e, stopCriteria);
	}

	virtual void perturbation(Solution<R, ADS>& s, Evaluation& e, SOSC& stopCriteria, levelHistory& history)
	{
		//cout << "perturbation(.)" << endl;

		int iter = history.first.first;
		int level = history.first.second;
		int iterMax = history.second.first;
		//int levelMax = history.second.second;

		//cout << "level = " << level << " e iter = " << iter << endl;

		// nivel atual: 'level'
		p.perturb(s, e, stopCriteria, level);

		// Incrementa a iteracao
		iter++;

		if(Component::debug)
			cout << "ILSL::iter " << iter << endl;

		if (iter >= iterMax)
		{
			iter = 0;
			level++;
			if(Component::information)
				cout << "ILSL::level " << level << ".." << endl;
		}

		// Atualiza o historico
		history.first.first = iter;
		history.first.second = level;
	}

	virtual bool acceptanceCriterion(const Evaluation& e1, const Evaluation& e2, levelHistory& history)
	{
		//cout << "acceptanceCriterion(.)" << endl;

		if (IteratedLocalSearch<levelHistory, R, ADS >::evaluator.betterThan(e1, e2))
		{
			if(Component::information)
			{
				cout << "ILSL::Best fo: on [iter " << history.first.first << " of level " << history.first.second << "] => ";
				e1.print();
			}

			// =======================
			//  Atualiza o historico
			// =======================
			// iter = 0
			history.first.first = 0;
			// level = 0
			history.first.second = 0;

			// =======================
			//    Retorna s2
			// =======================
			return true;
		}
		else
			return false;
	}

	virtual bool terminationCondition(levelHistory& history)
	{
		//cout << "terminationCondition(.)" << endl;
		int level = history.first.second;
		int levelMax = history.second.second;

		return (level >= levelMax);
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << IteratedLocalSearch<levelHistory, R, ADS >::idComponent() << "ilsl";
		return ss.str();

	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class IteratedLocalSearchLevelsBuilder : public ILS, public SingleObjSearchBuilder<R, ADS>
{
public:
	virtual ~IteratedLocalSearchLevelsBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval = nullptr;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!
		if(!eval)
			return nullptr;

		Constructive<R, ADS>* constructive = nullptr;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!
		if(!constructive)
			return nullptr;

		string rest = scanner.rest();

		pair<LocalSearch<R, ADS>*, std::string> method;
		method = hf.createLocalSearch(rest);

		LocalSearch<R, ADS>* h = method.first;

		scanner = Scanner(method.second);
		if(!h)
			return nullptr;

		ILSLPerturbation<R, ADS>* pert;
		hf.assign(pert, scanner.nextInt(), scanner.next()); // reads backwards!
		if(!pert)
			return nullptr;

		int iterMax = -1;
		try
		{
			iterMax = scanner.nextInt();
		}
		catch (ConversionError& e)
		{
			return nullptr;
		}

		int levelMax = -1;
		try
		{
			levelMax = scanner.nextInt();
		}
		catch (ConversionError& e)
		{
			return nullptr;
		}

		return new IteratedLocalSearchLevels<R, ADS>(*eval, *constructive, *h, *pert, iterMax,levelMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<R, ADS>::idComponent(), "constructive heuristic"));
		params.push_back(make_pair(LocalSearch<R, ADS>::idComponent(), "local search"));
		params.push_back(make_pair(ILSLPerturbation<R, ADS>::idComponent(), "ilsL perturbation"));
		params.push_back(make_pair("int", "max number of iterations without improvement"));
		params.push_back(make_pair("int", "levelMax of perturbation"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == IteratedLocalSearchLevels<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS>::idComponent() << ILS::family() << "ILSLevels";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_ILSL_HPP_*/
