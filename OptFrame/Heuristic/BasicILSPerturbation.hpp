#ifndef OPTFRAME_BASICILSPerturbation_HPP_
#define OPTFRAME_BASICILSPerturbation_HPP_

#include <math.h>
#include <vector>

#include "../NS.hpp"
#include "../RandGen.hpp"

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class BasicILSPerturbation
{
private:
	vector<NS<R, M>*> ns;
	Evaluator<R, M>& evaluator;
	int pMax;
	RandGen& rg;

public:
	BasicILSPerturbation(Evaluator<R, M>& e, int _pMax, NS<R, M>& _ns, RandGen& _rg) :
		evaluator(e), pMax(_pMax), rg(_rg)
	{
		ns.push_back(&_ns);
	}

	void add_ns(NS<R, M>& _ns)
	{
		ns.push_back(&_ns);
	}

	void perturb(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		int f = 0; // number of failures

		while (f < pMax)
		{
			int x = rg.rand(ns.size());

			Move<R, M>& m = ns[x]->move(s);

			if (m.canBeApplied(e, s))
			{
				delete &m.apply(e, s);
				delete &m;
				break;
			}
			else
				f++;

			delete &m;
		}

		if (f == pMax)
			cout << "ILS Warning: perturbation had no effect in " << pMax << " tries!" << endl;

		evaluator.evaluate(e, s); // updates 'e'
	}
};

#endif /*OPTFRAME_BASICILSPerturbation_HPP_*/
