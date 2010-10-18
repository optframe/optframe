#ifndef OPTFRAME_BI_HPP_
#define OPTFRAME_BI_HPP_

#include "../Heuristic.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class M>
class BestImprovement : public Heuristic<R,M>
{
public:
	BestImprovement(Evaluator<R,M>*, NSSeq<R,M>*);
	virtual Solution<R>* exec(double, long, Solution<R>*, Evaluation<M>*);

private:
	NSSeq<R,M>* nsseq;
	Evaluator<R,M>* eval;
};

template<class R, class M>
BestImprovement<R,M>::BestImprovement(Evaluator<R,M>* _eval, NSSeq<R,M>* _nsseq)
{
	this->eval = _eval;
	this->nsseq = _nsseq;
};

template<class R, class M>
Solution<R>* BestImprovement<R,M>::exec(double, long, Solution<R>* s, Evaluation<M>* e)
{
	Move<R,M>* m;
	if(e)
		m = nsseq->bestMove(e,eval,s);
	else
		m = nsseq->bestMove(eval,s);

	if(m)
	{
		Move<R,M>* r;
		if(e)
			r = m->apply(e,s);
		else
			r = m->apply(s);

		delete r;
		delete m;
	}

	return s;
};

#endif /*OPTFRAME_BI_HPP_*/
