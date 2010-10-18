#ifndef OPTFRAME_HILLCLIMBING_HPP_
#define OPTFRAME_HILLCLIMBING_HPP_

#include "../Heuristic.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class M>
class HillClimbing : public Heuristic<R,M>
{
public:
	HillClimbing(Evaluator<R,M>*, Heuristic<R,M>*);
	virtual Solution<R>* exec(double, long, Solution<R>*);
	
private:
	Heuristic<R,M>* h;
    Evaluator<R,M>* eval;
};

template<class R, class M>
HillClimbing<R,M>::HillClimbing(Evaluator<R,M>* eval, Heuristic<R,M>* h)
{
    this->eval = eval;
    this->h = h;
};

template<class R, class M>
Solution<R>* HillClimbing<R,M>::exec(double efv, long timelimit, Solution<R>* s)
{
	long tini = time(NULL);

    Solution<R>* s0 = s->clone();

    Evaluation<M>* e = eval->evaluate(s);

    s = h->exec(efv,timelimit,s,e);

    long tatual = time(NULL);


    while((eval->betterThan(s,s0)) && ((tatual-tini) < timelimit))
    {
        delete s0;
        s0 = s->clone();

        s = h->exec(efv,timelimit,s,e);

        tatual = time(NULL);
    }

    delete s;
    return s0;
};

#endif /*OPTFRAME_HILLCLIMBING_HPP_*/
