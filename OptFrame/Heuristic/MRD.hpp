#ifndef MRD_HPP_
#define MRD_HPP_


#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

//#define MRD_DEBUG

#ifdef  METHOD_DEBUG
#define MRD_DEBUG
#endif


template<class R, class M>
class MRD : public Heuristic<R,M>
{
public:
	MRD(Evaluator<R,M>* avaliador, NS<R,M>* vizinhanca, int iterMax);
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);
	
private:
	NS<R,M>* vizinhanca;
    Evaluator<R,M>* avaliador;
    int iterMax;
};


template<class R, class M>
MRD<R,M>::MRD(Evaluator<R,M>* avaliador, NS<R,M>* vizinhanca, int iterMax)
{
    this->avaliador = avaliador;
    this->vizinhanca = vizinhanca;
    this->iterMax = iterMax;
}


template<class R, class M>
Solution<R>* MRD<R,M>::exec(double efv, long timelimit, Solution<R>* s)
{
	//cout << "MRD [iterMax = "<< iterMax << "] exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);

    int iter = 0;

    long tatual = time(NULL);
    while(iter < iterMax && ((tatual-tini)<timelimit))
    {
        // Conferir destrutores virtuais =P
        // Possivel leak de memoria

    	Move<R,M>* move = vizinhanca->move(s);

		double custo = 0;

    	if(move->canBeApplied(s))
    		custo = avaliador->custoMovimento(move,s);
    	else
    	{
    		iter++;
    		delete move;

    		tatual = time(NULL);

    		continue;
    	}

    	if(avaliador->betterThan(custo, 0))
    	{
    		move->apply(s);
    		iter = -1;
    	}

    	delete move;
    	iter++;

    	tatual=time(NULL);
    }


#ifdef  MRD_DEBUG
    cout << "============================================\n";
    cout << "         MRD - Termino        \n";
    cout << "============================================\n";
#endif

    return s;
}


#endif /*MRD_HPP_*/
