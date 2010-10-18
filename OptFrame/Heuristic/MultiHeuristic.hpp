#ifndef MULTIHEURISTIC_HPP_
#define MULTIHEURISTIC_HPP_

#include <math.h>
#include <vector>

#include "../Heuristic.hpp"
#include "../NS.h"
#include "../Evaluator.hpp"
#include "../Solution.h"

//#define MULTIHEURISTIC_DEBUG

#ifdef  METHOD_DEBUG
#define MULTIHEURISTIC_DEBUG
#endif


template<class R, class M>
class MultiHeuristic : public Heuristic<R,M>
{
public:
	MultiHeuristic(Evaluator<R,M>*, vector<Heuristic<R,M> * > *);
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);

private:
    Evaluator<R,M>* avaliador;
    vector< Heuristic<R,M>* > * heuristicas;
};


template<class R, class M>
MultiHeuristic<R,M>::MultiHeuristic(Evaluator<R,M>* _avaliador, vector< Heuristic<R,M>* > * _heuristicas)
{
    avaliador=(_avaliador);
    heuristicas=(_heuristicas);
};

template<class R, class M>
Solution<R>* MultiHeuristic<R,M>::exec(double efv, long timelimit, Solution<R>* s)
{
	//cout << "MultiHeuristic exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);

	// inicializacao
	Solution<R>* sStar = s->clone(); // usuario obrigado a passar solucao inicial
	delete s;

	Evaluation<M> * evalSStar = avaliador->evaluate(sStar);

	//cout << "MultiHeuristic Initial Solution: "; evalSStar->print();

	long tatual;

	for (int h = 0; h < heuristicas->size(); h++)
	{
		////cout << "MultiHeuristic iter "<<iter<<":"<<endl;

		if((tatual-tini)>=timelimit)
		{
			//cout << "tempo limite estourado: " << timelimit << "s" << endl;
			break;
		}

		tatual = time(NULL);

		// Refinamento
		Solution<R>* sLinha = (*heuristicas)[h]->searchFT(efv, (timelimit - (tatual-tini)) ,sStar);
		delete sStar;
		sStar = sLinha;
	}

	evalSStar = avaliador->evaluate(sStar);

	//cout << "MultiHeuristic Best Solution: "; evalSStar->print();
	delete evalSStar;
	return sStar;
}

#endif /*MULTIHEURISTIC_HPP_*/
