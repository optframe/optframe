#ifndef PERTURBATIONLPLUS2_HPP_
#define PERTURBATIONLPLUS2_HPP_

#include<vector>

#include "PerturbationLevels.hpp"

template<class R, class M>
class PerturbationLPlus2 : public PerturbationLevels<R>
{
private:
	vector<NS<R,M>*> vizs;
public:
	PerturbationLPlus2(NS<R,M>* _viz)
	{
		vizs.push_back(_viz);
	}

	PerturbationLPlus2(vector<NS<R,M>*> _vizs)
	{
		vizs = _vizs;
	}


	void exec(int n, Solution<R>* s)
	{
		for(int i=0;i<n+2;i++)
		{
			int v = rand()%vizs.size();

			Move<R,M>* m =	vizs[v]->move(s);
			if(m->canBeApplied(s))
				m->apply(s);
			delete m;
		}		
	}
};

#endif /*PERTURBATIONLPLUS2_HPP_*/
