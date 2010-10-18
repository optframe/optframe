#ifndef MULTISTART_HPP_
#define MULTISTART_HPP_

#include <math.h>
#include <vector>

#include "../Heuristic.hpp"
#include "../NS.h"
#include "../Evaluator.hpp"
#include "../InitialSolution.h"

//#define MULTISTART_DEBUG

#ifdef  METHOD_DEBUG
#define MULTISTART_DEBUG
#endif


template<class R, class M>
class MultiStart : public Heuristic<R,M>
{
public:
	MultiStart(Evaluator<R,M>*, InitialSolution<R>*, Heuristic<R,M>*, int iterMax);
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);

private:
    Evaluator<R,M>* avaliador;
    InitialSolution<R>* solucaoInicial;
    Heuristic<R,M>* heuristica;
    int iterMax;
    double alpha;
};


template<class R, class M>
MultiStart<R,M>::MultiStart(Evaluator<R,M>* _avaliador, InitialSolution<R>* _solucaoInicial, Heuristic<R,M>* _heuristica, int _iterMax)
{
    avaliador=(_avaliador);
    solucaoInicial=(_solucaoInicial);
    heuristica=(_heuristica);
    iterMax=(_iterMax);
};


template<class R, class M>
Solution<R>* MultiStart<R,M>::exec(double efv, long timelimit, Solution<R>* s)
{

	cout << "MultiStart exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);

	// inicializacao
	Solution<R>* sStar = s->clone(); // usuario obrigado a passar solucao inicial
	delete s;

	Evaluation<M> * evalSStar = avaliador->evaluate(sStar);

	cout << "MultiStart Initial Solution: "; evalSStar->print();

	long tatual;

	for (int iter = 0 ; iter < iterMax ; iter++)
	{
		tatual = time(NULL);

		//cout << "MultiStart iter "<<iter<<":"<<endl;

		if((tatual-tini)>=timelimit)
		{
			cout << "tempo limite estourado: " << timelimit << "s" << endl;
			break;
		}

		tatual = time(NULL);

		// Construção parcialmente gulosa
		Solution<R>* spg = solucaoInicial->generateSolution();

		// Refinamento
		Solution<R>* sLinha = heuristica->searchFT(efv, (timelimit - (tatual-tini)) ,spg);

		delete spg;

		Evaluation<M> * evalSLinha = avaliador->evaluate(sLinha);

		// critério de aceitação
		if(avaliador->betterThan(evalSLinha->evaluation(), evalSStar->evaluation()))
		{
			delete sStar;
			delete evalSStar;
			delete evalSLinha;

			sStar = sLinha->clone();
			evalSStar = avaliador->evaluate(sStar);

			cout << "MultiStart iter "<<iter<<": "; evalSStar->print();
		}
		else
		{
			delete evalSLinha;
			delete sLinha;
		}
	}

	cout << "MultiStart Best Solution: "; evalSStar->print();
	return sStar;
}

#endif /*MULTISTART_HPP_*/
