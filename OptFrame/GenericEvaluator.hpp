#ifndef GENERICEVALUATOR_HPP_
#define GENERICEVALUATOR_HPP_

#include "Evaluator.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"

#include <iostream>

#define EPSILON 0.0001

using namespace std;

template<class R, class M>
class GenericEvaluator : public Evaluator<R,M>
{
protected:
	vector< Evaluator<R,M> * > partialEvaluators;
public:

	GenericEvaluator(){};
	GenericEvaluator(vector< Evaluator<R,M> * > p)
	{
		partialEvaluators = p;
	}

	void add(Evaluator<R,M>* e){ partialEvaluators.push_back(e); }

	virtual Evaluation<M>* evaluate(Solution<R>* s)
	{
		Evaluation<M>* e = new Evaluation<M>;
		evaluate(e,s);
		return e;
	}

	virtual void evaluate(Evaluation<M>* e, Solution<R>* s)
	{
		double objFunction = 0;
		double infMeasure  = 0;

		for (int i = 0 ; i < partialEvaluators.size() ; i++)
		{
			partialEvaluators[i]->evaluate(e,s);

			objFunction += e->getObjFunction();
			infMeasure  += e->getInfMeasure();
		}

		e->setObjFunction(objFunction);
		e->setInfMeasure(infMeasure);
	}

	virtual bool betterThan(double a, double b)
	{
		return (a < (b-EPSILON));
	}

};


#endif /*GENERICEVALUATOR_HPP_*/
