#ifndef EMPTY_HPP_
#define EMPTY_HPP_


#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

//#define EMPTY_DEBUG

#ifdef  METHOD_DEBUG
#define EMPTY_DEBUG
#endif


template<class R, class M>
class Empty : public Heuristic<R,M>
{
public:
	Empty(){};
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s){return s;}
};

#endif /*EMPTY_HPP_*/
