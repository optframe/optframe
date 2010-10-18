#ifndef DESCIDA_HPP_
#define DESCIDA_HPP_

#include "../Heuristic.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

//#define DESCIDA_DEBUG

#ifdef  METHOD_DEBUG
#define DESCIDA_DEBUG
#endif


template<class R, class M>
class Descida : public Heuristic<R,M>
{
public:
	Descida(Evaluator<R,M>*, NSSeq<R,M>*);
	virtual Solution<R>* exec(double, long, Solution<R>*);
	
private:
	NSSeq<R,M>* vizinhanca;
    Evaluator<R,M>* avaliador;
};


template<class R, class M>
Descida<R,M>::Descida(Evaluator<R,M>* avaliador, NSSeq<R,M>* vizinhanca)
{
    cout << "Descida is deprecated. Try using HillClimbing+BI instead." << endl;

    this->avaliador = avaliador;
    this->vizinhanca = vizinhanca;
}


template<class R, class M>
Solution<R>* Descida<R,M>::exec(double efv, long timelimit, Solution<R>* s)
{
	//cout << "Descida " << " exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);

    Solution<R>* anterior = s->clone();

    Evaluation<M>* e = avaliador->evaluate(s);

    Solution<R>* s1 = vizinhanca->bestImprovement(e, avaliador,s);

    delete s;
    s = s1;

    long tatual = time(NULL);

    while((avaliador->betterThan(s,anterior)) && ((tatual-tini) < timelimit))
    {
        delete anterior;
        anterior = s->clone();

        s1 = vizinhanca->bestImprovement(e, avaliador, s);

        delete s;
        s = s1;

        tatual = time(NULL);
    }

    delete s;

    return anterior;
}

#endif /*DESCIDA_HPP_*/
