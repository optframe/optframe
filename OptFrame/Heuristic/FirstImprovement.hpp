#ifndef OPTFRAME_FI_HPP_
#define OPTFRAME_FI_HPP_

#include "../Heuristic.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class M>
class FirstImprovement : public Heuristic<R,M>
{
public:
	FirstImprovement(Evaluator<R,M>*, NSSeq<R,M>*);
	virtual Solution<R>* exec(double, long, Solution<R>*, Evaluation<M>*);

private:
	NSSeq<R,M>* nsseq;
	Evaluator<R,M>* eval;
};

template<class R, class M>
FirstImprovement<R,M>::FirstImprovement(Evaluator<R,M>* _eval, NSSeq<R,M>* _nsseq)
{
	this->eval = _eval;
	this->nsseq = _nsseq;
};

template<class R, class M>
Solution<R>* FirstImprovement<R,M>::exec(double, long, Solution<R>* s, Evaluation<M>* e)
{
	Move<R,M>* m;
	if(e)
		m = nsseq->firstMove(e,eval,s);
	else
		m = nsseq->firstMove(eval,s);

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

#endif /*OPTFRAME_FI_HPP_*/
