#ifndef VNS_HPP_
#define VNS_HPP_

// Variable Neighborhood Search

#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"
#include "./IteratedLocalSearch/IteratedLocalSearch.hpp"

//#define VNS_DEBUG

#ifdef  METHOD_DEBUG
#define VNS_DEBUG
#endif


template<class R, class M>
class VNS : public Heuristic<R,M>
{
public:
	VNS(Evaluator<R,M>* avaliador, vector< NS<R,M>* > vizinhancas, Heuristic<R,M>* localSearch);

	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);

private:
	vector< NS<R,M>* > vizs;
	Evaluator<R,M>* avaliador;
	Heuristic<R,M>* localSearch;
};


template<class R, class M>
VNS<R,M>::VNS(Evaluator<R,M>* avaliador, vector< NS<R,M>* > vizinhancas, Heuristic<R,M>* localSearch)
{
	this->avaliador = avaliador;
	this->vizs = vizinhancas;
	this->localSearch = localSearch;
}


template<class R, class M>
Solution<R>* VNS<R,M>::exec(double efv, long timelimit, Solution<R>* s)
{
	cout << "VNS exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);

	int r = vizs.size();

	Evaluation<M>* e = avaliador->evaluate(s);

	long tatual = time(NULL);

	while(avaliador->betterThan(efv, e->evaluation()) && ((tatual-tini)<timelimit))
	{
		int k = 1;

		while(k<=r)
		{
			Solution<R>* sl = s->clone();

			Move<R,M>* m =	vizs[k-1]->move(sl);
			if(m->canBeApplied(sl))
				m->apply(sl);
			delete m;

			Solution<R>* s2 = localSearch->searchFT(efv,timelimit,sl);
			delete sl;
			sl = s2;

			if(avaliador->betterThan(sl,s))
			{
				delete s;
				s = sl;
				k = 1;
			}
			else
			{
				delete sl;
				k = k + 1;
			}

			delete e;

			e = avaliador->evaluate(s);

			tatual = time(NULL);
		}


	}

	// Limpa a melhor fo
	delete e;

	// Retorna 's'
	return s;
}


#endif /*VNS_HPP_*/
