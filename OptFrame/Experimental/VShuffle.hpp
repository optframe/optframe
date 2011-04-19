#ifndef VShuffle_HPP_
#define VShuffle_HPP_

#include <math.h>
#include <vector>

#include "../Heuristic.hpp"
#include "../NS.hpp"
#include "../Evaluator.hpp"
#include "../Solution.hpp"

//#define VShuffle_DEBUG

#ifdef  METHOD_DEBUG
#define VShuffle_DEBUG
#endif


template<class R, class M>
class VShuffle : public Heuristic<R,M>
{
public:
	using Heuristic<R, M>::exec; // prevents name hiding

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		cerr << "VShuffle exec("<<target_f<<","<<timelimit<<")" << endl;
		random_shuffle(s.getR().begin(),s.getR().end());
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		cerr << "VShuffle exec("<<target_f<<","<<timelimit<<")" << endl;
		random_shuffle(s.getR().begin(),s.getR().end());
	}

private:
};

#endif
