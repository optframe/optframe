#ifndef MULTIHEURISTIC_HPP_
#define MULTIHEURISTIC_HPP_

#include <math.h>
#include <vector>

#include "../Heuristic.hpp"
#include "../NS.hpp"
#include "../Evaluator.hpp"
#include "../Solution.hpp"

//#define MULTIHEURISTIC_DEBUG

#ifdef  METHOD_DEBUG
#define MULTIHEURISTIC_DEBUG
#endif


template<class R, class M>
class MultiHeuristic : public Heuristic<R,M>
{
public:
	using Heuristic<R, M>::exec; // prevents name hiding

	MultiHeuristic(Evaluator<R, M>& _ev, vector<Heuristic<R, M>*> _heuristics) :
		ev(_ev), heuristics(_heuristics)
	{
	}

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = ev.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		cerr << "MultiHeuristic exec("<<target_f<<","<<timelimit<<")" << endl;

		cout << "MultiHeuristic Initial Solution: " ; e.print();
		cout << heuristics.size() << endl;

		long tatual;

		for (int h = 0; h < heuristics.size(); h++)
		{
			cerr << "MultiHeuristic iter "<<h<<":"<<endl;
			(heuristics)[h]->exec(s,timelimit, target_f);
		}
	}

private:
    Evaluator<R,M>& ev;
    vector< Heuristic<R,M>* > heuristics;
};

#endif
