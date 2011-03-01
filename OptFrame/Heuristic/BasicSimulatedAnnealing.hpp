#ifndef OPTFRAME_BSA_HPP_
#define OPTFRAME_BSA_HPP_

#include "../Heuristic.hpp"
#include <math.h>

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class BasicSimulatedAnnealing: public Heuristic<R, M>
{
private:
	Evaluator<R, M>& evaluator;
	vector<NS<R, M>*> neighbors;
	RandGen& rg;
	double alpha;
	int SAmax;
	double Ti;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	BasicSimulatedAnnealing(Evaluator<R, M>& _evaluator, vector<NS<R, M>*> _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
		evaluator(_evaluator), neighbors(_neighbors), rg(_rg)
	{
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);

	}

	BasicSimulatedAnnealing(Evaluator<R, M>& _evaluator, NS<R, M>* _neighbors, double _alpha, int _SAmax, double _Ti, RandGen& _rg) :
		evaluator(_evaluator), rg(_rg)
	{
		neighbors.push_back(_neighbors);
		alpha = (_alpha);
		SAmax = (_SAmax);
		Ti = (_Ti);
	}

	virtual ~BasicSimulatedAnnealing()
	{
	}

	void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = evaluator.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		cout << "SA exec(" << target_f << "," << timelimit << ")" << endl;

		Timer tnow;

		double T = Ti;
		int iterT = 0;
		Solution<R>* sStar = &s.clone();
		Evaluation<M>* eStar = &e.clone();

		while ((T > 0) && (tnow.now() < timelimit))
		{
			while ((iterT < SAmax) && (tnow.now() < timelimit))
			{
				int n = rg.rand(neighbors.size());
				Move<R, M>* move = &(neighbors[n]->move(s));

				while (!(move->canBeApplied(e, s)))
				{
					delete move;
					move = &(neighbors[n]->move(s));
				}

				Solution<R>* sCurrent = &s.clone();
				Evaluation<M>* eCurrent = &e.clone();
				move->apply(*eCurrent, *sCurrent);
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
	}

};

/*

 Procedure originally proposed by Kirkpatrick et al.

 S. Kirkpatrick, D.C. Gellat, and M.P. Vecchi. Optimization by Simulated Annealing.
 Science, 220:671–680, 1983.

 LATEX:

 */

#endif /*OPTFRAME_BSA_HPP_*/
