#ifndef OPTFRAME_ILS_HPP_
#define OPTFRAME_ILS_HPP_

#include <math.h>
#include <vector>

#include "../Heuristic.hpp"
#include "../Evaluator.hpp"

template<class H, class R, class M = OPTFRAME_DEFAULT_MEMORY>
class IteratedLocalSearch: public Heuristic<R, M>
{
protected:
	Evaluator<R, M>& evaluator;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	IteratedLocalSearch(Evaluator<R, M>& _evaluator) :
		evaluator(_evaluator)
	{
	}

	virtual H& initializeHistory() = 0;

	virtual void localSearch(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f) = 0;

	virtual void perturbation(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f, H& history) = 0;

	virtual Solution<R>& acceptanceCriterion(const Solution<R>& s1, const Solution<R>& s2, H& history) = 0;

	virtual bool terminationCondition(H& history) = 0;

	void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = evaluator.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		cout << "ILS exec(" << target_f << "," << timelimit << ")" << endl;

		long tini = time(NULL);

		H* history = &initializeHistory();

		// 's0' <- GenerateSolution
		// 's*' <- localSearch 's'

		long tnow = time(NULL);

		localSearch(s, e, (timelimit - (tnow - tini)), target_f);

		Solution<R>* sStar = &s.clone();
		Evaluation<M>* eStar = &e.clone();

		cout << "ILS starts: ";
		e.print();

		do
		{
			Solution<R>* s1 = &sStar->clone();
			Evaluation<M>* e1 = &eStar->clone();

			tnow = time(NULL);
			perturbation(*s1, *e1, (timelimit - (tnow - tini)), target_f, *history);

			tnow = time(NULL);
			localSearch(*s1, *e1, (timelimit - (tnow - tini)), target_f);

			Solution<R>* s2 = s1;
			Evaluation<M>* e2 = e1;

			Solution<R>* sStar1 = &acceptanceCriterion(*sStar, *s2, *history);

			delete sStar;
			delete eStar;
			delete s2;
			delete e2;

			sStar = sStar1;
			eStar = &evaluator.evaluate(*sStar);

			tnow = time(NULL);
		} while (evaluator.betterThan(target_f, eStar->evaluation()) && !terminationCondition(*history) && ((tnow - tini) < timelimit));

		e = *eStar;
		s = *sStar;

		delete eStar;
		delete sStar;

		delete history;
	}
};

#endif /*OPTFRAME_ILS_HPP_*/
