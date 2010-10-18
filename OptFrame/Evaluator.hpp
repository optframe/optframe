#ifndef EVALUATOR_HPP_
#define EVALUATOR_HPP_

#include "Evaluation.hpp"
#include "Move.hpp"

#include <iostream>

#define EPSILON 0.0001

using namespace std;

template<class R, class M>
class Evaluator
{
public:

	virtual void evaluate(Evaluation<M>* e, Solution<R>* s)
	{
		// Default Implementation!
		// call: void evaluate(Evaluation<M>*, R* r)

		evaluate(e, s->getRepresentation());
	}

	virtual void evaluate(Evaluation<M>* e, R* r)
	{
		// Default Implementation!
		// call: Evaluation<M>* evaluate(Solution<R>* s)

		Solution<R>* s = new TestSolution<R>(r);

		Evaluation<M>* ev = evaluate(s);
		s->setRepresentation(NULL);
		delete s;

		M* m = ev->getMemory();
		ev->setMemory(NULL);

		e->setObjFunction(ev->getObjFunction());
		e->setInfMeasure(ev->getInfMeasure());
		e->setMemory(m);

		delete ev;
	}

	virtual Evaluation<M>* evaluate(Solution<R>* s)
	{
		// Default Implementation!
		// call: Evaluation<M>* evaluate(R* r)

		return evaluate(s->getRepresentation());
	}

	virtual Evaluation<M>* evaluate(R* r)
	{
		// Default Implementation!
		// call: void evaluate(Evaluation<M>* e, Solution<R>* s)

		Solution<R>* s = new TestSolution<R>(r);

		Evaluation<M>* e = new Evaluation<M>; // default
		evaluate(e,s);

		s->setRepresentation(NULL);
		delete s;

		return e;
	}

	// =====================================================================
	// =====================================================================

	// Apply movement considering a previous evaluation => Faster.
	// Update evaluation 'e'
	Move<R,M>* aplicarMovimento(Evaluation<M>* e, Move<R,M>* m, Solution<R>* s)
    {
		Move<R,M>* rev = m->apply(e,s);
		evaluate(e,s);
		return rev;
    }

	// Apply movement without considering a previous evaluation => Slower.
	// Return new evaluation 'e'
	pair<Move<R,M>*,Evaluation<M>*>* aplicarMovimento(Move<R,M>* m, Solution<R>* s)
    {
		Move<R,M>* rev = m->apply(s);
		return new pair<Move<R,M>*,Evaluation<M>*>(rev,evaluate(s));
    }

	// Movement cost based on reevaluation of 'e'
	double custoMovimento(Evaluation<M>* e, Move<R,M>* m, Solution<R>* s)
	{
		Move<R,M>* rev = aplicarMovimento(e,m,s);
		double e_end = e->evaluation();

		Move<R,M>* ini = aplicarMovimento(e,rev,s);
		double e_ini = e->evaluation();

		// Difference: new - original
		double dif = e_end - e_ini;

		delete rev;
		delete ini;

		return dif;
	}


	// Movement cost based on complete evaluation
	double custoMovimento(Move<R,M>* m, Solution<R>* s)
	{
		// Avaliação referente ao movimento já aplicado.
		pair<Move<R,M>*,Evaluation<M>*>* rev = aplicarMovimento(m,s);

		// Avaliação Original
		pair<Move<R,M>*,Evaluation<M>*>* ini = aplicarMovimento(rev->first,s);

		// Diferença: novo - original
		double dif = ((rev->second)->evaluation()) - ((ini->second)->evaluation());

		delete (rev->first);
		delete (rev->second);
		delete (ini->first);
		delete (ini->second);

		delete rev;
		delete ini;

		return dif;
	}

	// ===========
	// Better Than
	// ===========

	virtual bool betterThan(double a, double b)
	{
		return (a < (b-EPSILON));
	}

	bool betterThan(Solution<R>* s1, Solution<R>* s2)
	{
		Evaluation<M>* a1 = evaluate(s1);
		Evaluation<M>* a2 = evaluate(s2);

		double fa1 = a1->evaluation();
		double fa2 = a2->evaluation();

		delete a1;
		delete a2;

		return betterThan(fa1,fa2);
	}

	bool betterThan(Evaluation<M>* a1, Evaluation<M>* a2)
	{
		return betterThan(a1->evaluation(),a2->evaluation());
	}

};

#endif /*EVALUATOR_HPP_*/
