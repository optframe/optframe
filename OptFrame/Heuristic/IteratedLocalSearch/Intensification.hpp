#ifndef INTENSIFICATION_HPP_
#define INTENSIFICATION_HPP_

#include "../../Heuristic.hpp"

//#define DESCIDA_DEBUG

#ifdef  METHOD_DEBUG
#define DESCIDA_DEBUG
#endif


template<class R, class M, class H>
class Intensification : public Heuristic<R,M>
{
public:
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s) = 0;
	void setHistory(H* h){history=h;}
	
protected:
	H* history;
};


#endif /*INTENSIFICATION_HPP_*/
