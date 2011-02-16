#ifndef INTENSIFICATION_HPP_
#define INTENSIFICATION_HPP_

#include "../Heuristic.hpp"

template<class R,class M>
class Intensification : public Heuristic<R,M>
{
public:

	virtual void exec(Solution<R>& _s, double timelimit, double target_f) = 0;

	virtual void addSolution(const Solution<R>&) = 0;
};


#endif /*INTENSIFICATION_HPP_*/
