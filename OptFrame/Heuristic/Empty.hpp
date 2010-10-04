#ifndef OPTFRAME_EMPTY_HPP_
#define OPTFRAME_EMPTY_HPP_

#include "../Heuristic.hpp"

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class Empty : public Heuristic<R,M>
{
public:

	using Heuristic<R, M>::exec; // prevents name hiding

	Empty(){};
    virtual void exec(Solution<R>&, double timelimit, double target_f){};
	virtual void exec(Solution<R>&, Evaluation<M>&, double timelimit, double target_f){};
};

#endif /*OPTFRAME_EMPTY_HPP_*/
