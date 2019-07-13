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

#ifndef OPTFRAME_BSA_HPP_
#define OPTFRAME_BSA_HPP_

#include "../../SingleObjSearch.hpp"
#include <math.h>

#include "SA.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicSimulatedAnnealing: public SingleObjSearch<R, ADS>
{
private:
	Evaluator<R, ADS>& evaluator;
	Constructive<R, ADS>& constructive;
	vector<NS<R, ADS>*> neighbors;
	RandGen& rg;
	double alpha;
	int SAmax;
	double Ti;

public:

	BasicSimulatedAnnealing(Evaluator<R, ADS>& _evaluator, Constructive<R, ADS>& _constructive, vector<NS<R, ADS>*> _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
		evaluator(_evaluator), constructive(_constructive), neighbors(_neighbors), rg(_rg)
	{
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);

	}

	BasicSimulatedAnnealing(Evaluator<R, ADS>& _evaluator, Constructive<R, ADS>& _constructive, NS<R, ADS>& _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
		evaluator(_evaluator), constructive(_constructive), rg(_rg)
	{
		neighbors.push_back(&_neighbors);
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);
	}

	virtual ~BasicSimulatedAnnealing()
	{
	}

	pair<Solution<R, ADS>, Evaluation>* search(SOSC& stopCriteria, const Solution<R, ADS>* _s = nullptr,  const Evaluation* _e = nullptr)
	{
		double timelimit = stopCriteria.timelimit;
		double target_f = stopCriteria.target_f;
		cout << "SA search(" << target_f << "," << timelimit << ")" << endl;

		Timer tnow;

                // store initial value in s (TODO: remove workaround to deal with pointer directly)
		Solution<R, ADS>* sP = constructive.generateSolution(timelimit);
		Solution<R, ADS> s(std::move(*sP)); // workaround
                delete sP;                         // workaround
		Evaluation e = evaluator.evaluateSolution(s);

		double T = Ti;
		int iterT = 0;
		Solution<R, ADS>* sStar = &s.clone();
		Evaluation* eStar = &e.clone();

		while ((T > 0.000001) && (tnow.now() < timelimit))
		{
			while ((iterT < SAmax) && (tnow.now() < timelimit))
			{
				int n = rg.rand(neighbors.size());
				Move<R, ADS>* move = neighbors[n]->validRandomMoveSolution(s);

				if(!move)
				{
					if(Component::warning)
						cout << "SA warning: no move in iter=" << iterT << " T=" << T << "! continue..." << endl;
					continue;
				}

				Solution<R, ADS>* sCurrent = &s.clone();
				Evaluation* eCurrent = &e.clone();
				Component::safe_delete(move->applyUpdateSolution(*eCurrent, *sCurrent));
				evaluator.reevaluateSolution(*eCurrent, *sCurrent);

				if (evaluator.betterThan(*eCurrent, e))
				{
					e = *eCurrent;
					s = *sCurrent;
					delete sCurrent;
					delete eCurrent;

					if (evaluator.betterThan(e, *eStar))
					{
						delete sStar;
						sStar = &s.clone();
						delete eStar;
						eStar = &e.clone();

						cout << "Best fo: " << e.evaluation() << " Found on Iter = " << iterT << " and T = " << T;
						cout << endl;
					}
				}
				else
				{
					double x = rg.rand01();
					double delta = ::fabs(eCurrent->evaluation() - e.evaluation());

					if (x < exp(-delta / T))
					{
						s = *sCurrent;
						e = *eCurrent;
						delete sCurrent;
						delete eCurrent;
					}
					else
					{
						delete sCurrent;
						delete eCurrent;
					}
				}

				iterT++;
				delete move;
			}
			T = alpha * T;
			iterT = 0;
		}
                cout << "T=" << T << endl;

		s = *sStar;
		e = *eStar;
		delete sStar;
		delete eStar;

		return new pair<Solution<R, ADS>, Evaluation> (s, e);
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS>::idComponent() << ":SA:BasicSA";
		return ss.str();
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BasicSimulatedAnnealingBuilder: public SA, public SingleObjSearchBuilder<R, ADS>
{
public:
	virtual ~BasicSimulatedAnnealingBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS>* build(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		Evaluator<R, ADS>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<R, ADS>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		vector<NS<R, ADS>* > hlist;
		hf.assignList(hlist, scanner.nextInt(), scanner.next()); // reads backwards!

		double alpha = scanner.nextDouble();
		int SAmax = scanner.nextInt();
		double Ti = scanner.nextDouble();

		return new BasicSimulatedAnnealing<R, ADS> (*eval, *constructive, hlist, alpha, SAmax, Ti, hf.getRandGen());
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<R, ADS>::idComponent(), "constructive heuristic"));
		stringstream ss;
		ss << NS<R, ADS>::idComponent() << "[]";
		params.push_back(make_pair(ss.str(), "list of NS"));
		params.push_back(make_pair("OptFrame:double", "cooling factor"));
		params.push_back(make_pair("OptFrame:int", "number of iterations for each temperature"));
		params.push_back(make_pair("OptFrame:double", "initial temperature"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicSimulatedAnnealing<R, ADS>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS>::idComponent() << ":" << SA::family() << "BasicSA";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

/*

 Procedure originally proposed by Kirkpatrick et al.

 S. Kirkpatrick, D.C. Gellat, and M.P. Vecchi. Optimization by Simulated Annealing.
 Science, 220:671–680, 1983.

 LATEX:

 */

}

#endif /*OPTFRAME_BSA_HPP_*/
