#ifndef OPTFRAME_EVALUATOR_HPP_
#define OPTFRAME_EVALUATOR_HPP_

#include "Solution.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"

#include <iostream>

//#define EPSILON 0.0001

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class Evaluator
{
public:

	virtual ~Evaluator()
	{
	}

	Evaluation<M>& evaluate(const Solution<R>& s)
	{
		return evaluate(s.getR());
	}

	virtual Evaluation<M>& evaluate(const R& r) = 0;

	void evaluate(Evaluation<M>& e, const Solution<R>& s)
	{
		evaluate(e, s.getR());
	}

	virtual void evaluate(Evaluation<M>& e, const R& r)
	{
		Evaluation<M>& e1 = evaluate(r);
		e = e1;
		delete &e1;
	}

	// Apply movement considering a previous evaluation => Faster.
	// Update evaluation 'e'
	Move<R, M>& applyMove(Evaluation<M>& e, Move<R, M>& m, Solution<R>& s)
	{
		Move<R, M>& rev = m.apply(e, s);
		evaluate(e, s);
		return rev;
	}

	// Apply movement without considering a previous evaluation => Slower.
	// Return new evaluation 'e'
	pair<Move<R, M>&, Evaluation<M>&>& applyMove(Move<R, M>& m, Solution<R>& s)
	{
		Move<R, M>& rev = m.apply(s);
		return *new pair<Move<R, M>&, Evaluation<M>&> (rev, evaluate(s));
	}

	// Movement cost based on reevaluation of 'e'
	double moveCost(Evaluation<M>& e, Move<R, M>& m, Solution<R>& s)
	{
		Move<R, M>& rev = applyMove(e, m, s);
		double e_end = e.evaluation();

		Move<R, M>& ini = applyMove(e, rev, s);
		double e_ini = e.evaluation();

		// Difference: new - original
		double diff = e_end - e_ini;

		delete &rev;
		delete &ini;

		return diff;
	}

	// Movement cost based on complete evaluation
	double moveCost(Move<R, M>& m, Solution<R>& s)
	{
		pair<Move<R, M>&, Evaluation<M>&>& rev = applyMove(m, s);

		pair<Move<R, M>&, Evaluation<M>&>& ini = applyMove(rev.first, s);

		// Difference: new - original
		double diff = rev.second.evaluation() - ini.second.evaluation();

		delete &rev.first;
		delete &rev.second;
		delete &ini.first;
		delete &ini.second;

		delete &rev;
		delete &ini;

		return diff;
	}

	// true if 's1' is better than 's2'
	bool betterThan(const Solution<R>& s1, const Solution<R>& s2)
	{
		Evaluation<M>& e1 = evaluate(s1);
		Evaluation<M>& e2 = evaluate(s2);

		double f1 = e1.evaluation();
		double f2 = e2.evaluation();

		delete &e1;
		delete &e2;

		return betterThan(f1, f2);
	}

	// true if 'e1' is better than 'e2'
	bool betterThan(const Evaluation<M>& e1, const Evaluation<M>& e2)
	{
		return betterThan(e1.evaluation(), e2.evaluation());
	}

	// minimization problem: true when a < b
	// maximization problem: true when a > b
	virtual bool betterThan(double a, double b) = 0;
};

#endif /*OPTFRAME_EVALUATOR_HPP_*/
