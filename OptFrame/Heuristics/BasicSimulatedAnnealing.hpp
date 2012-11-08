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

#ifndef OPTFRAME_BSA_HPP_
#define OPTFRAME_BSA_HPP_

#include "../SingleObjSearch.hpp"
#include <math.h>

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class BasicSimulatedAnnealing: public SingleObjSearch<R, ADS, M>
{
private:
	Evaluator<R, ADS, M>& evaluator;
	Constructive<R, ADS>& constructive;
	vector<NS<R, ADS, M>*> neighbors;
	RandGen& rg;
	double alpha;
	int SAmax;
	double Ti;

public:

	BasicSimulatedAnnealing(Evaluator<R, ADS, M>& _evaluator, Constructive<R, ADS>& _constructive, vector<NS<R, ADS, M>*> _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
		evaluator(_evaluator), constructive(_constructive), neighbors(_neighbors), rg(_rg)
	{
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);

	}

	BasicSimulatedAnnealing(Evaluator<R, ADS, M>& _evaluator, Constructive<R, ADS>& _constructive, NS<R, ADS, M>* _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
		evaluator(_evaluator), constructive(_constructive), rg(_rg)
	{
		neighbors.push_back(_neighbors);
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);
	}

	virtual ~BasicSimulatedAnnealing()
	{
	}

	pair<Solution<R, ADS>&, Evaluation<M>&>* search(double timelimit = 100000000, double target_f = 0)
	{
		cout << "SA search(" << target_f << "," << timelimit << ")" << endl;

		Timer tnow;

		Solution<R, ADS>& s = constructive.generateSolution();
		Evaluation<M>& e = evaluator.evaluate(s);

		double T = Ti;
		int iterT = 0;
		Solution<R, ADS>* sStar = &s.clone();
		Evaluation<M>* eStar = &e.clone();

		while ((T > 0) && (tnow.now() < timelimit))
		{
			while ((iterT < SAmax) && (tnow.now() < timelimit))
			{
				int n = rg.rand(neighbors.size());
				Move<R, ADS, M>* move = &(neighbors[n]->move(s));

				while (!(move->canBeApplied(s)))
				{
					delete move;
					move = &(neighbors[n]->move(s));
				}

				Solution<R, ADS>* sCurrent = &s.clone();
				Evaluation<M>* eCurrent = &e.clone();
				delete &move->apply(*eCurrent, *sCurrent);
				evaluator.evaluate(*eCurrent, *sCurrent);

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

						cout << "Best fo: " << e.evaluation() << " Found on Iter = " << iterT << " and T = " << T << endl;
					}
				}
				else
				{
					double x = rg.rand01();
					double delta = fabs(eCurrent->evaluation() - e.evaluation());

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
			}
			T = alpha * T;
			iterT = 0;
		}

		s = *sStar;
		e = *eStar;
		delete sStar;
		delete eStar;

		return new pair<Solution<R, ADS>&, Evaluation<M>&> (s, e);
	}

	virtual string id() const
	{
		return idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS, M>::idComponent() << "SA:basic_sa";
		return ss.str();

	}

};

/*

 Procedure originally proposed by Kirkpatrick et al.

 S. Kirkpatrick, D.C. Gellat, and M.P. Vecchi. Optimization by Simulated Annealing.
 Science, 220:671–680, 1983.

 LATEX:

 */

#endif /*OPTFRAME_BSA_HPP_*/
