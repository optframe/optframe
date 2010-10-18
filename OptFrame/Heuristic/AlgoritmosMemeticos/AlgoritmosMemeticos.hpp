#ifndef ALGORITMOSMEMETICOS_HPP_
#define ALGORITMOSMEMETICOS_HPP_

#include "../../Heuristic.hpp"
#include "../../NS.h"
#include "../../Evaluator.hpp"

#include "CrossoverAM.h"

//#define AM_DEBUG

#ifdef  METHOD_DEBUG
#define AM_DEBUG
#endif


template<class R, class M>
class AlgoritmosMemeticos : public Heuristic<R,M>
{
public:
	AlgoritmosMemeticos(Heuristic<R,M>* h, Evaluator<R,M>* avaliador, NS<R,M>* mutacao, CrossoverAM<R>* recombinacao, double pRecomb, double pMut, int tamPop, int numGeracoes);
	virtual Solution<R>* exec(double efv, unsigned long timelimit, Solution<R>* s);
	
private:
	NS<R,M>* mutacao;
	CrossoverAM<R>* recombinacao;
    Evaluator<R,M>* avaliador;
    Heuristic<R,M>* h;
    
    double pRecomb;
    double pMut;
    
    int tamPop;
    int numGeracoes;
};

template<class R, class M>
AlgoritmosMemeticos<R,M>::AlgoritmosMemeticos(Heuristic<R,M>* h, Evaluator<R,M>* avaliador, NS<R,M>* mutacao, CrossoverAM<R>* recombinacao, double pRecomb, double pMut, int tamPop, int numGeracoes)
{
    this->avaliador = avaliador;
    this->mutacao = mutacao;
    this->recombinacao = recombinacao;
    this->pRecomb = pRecomb;
    this->pMut = pMut;
    this->tamPop = tamPop;
    this->numGeracoes = numGeracoes;
    this->h = h;
}


template<class R, class M>
Solution<R>* AlgoritmosMemeticos<R,M>::exec(double efv, unsigned long timelimit, Solution<R>* sBase)
{
	Solution<R>* sStar = sBase;

	Evaluation<M>* eStar = avaliador->evaluate(sStar);

    // Populacao inicial
    vector<Solution<R>*> p(tamPop);

    for(int i=0;i<tamPop;i++)
    	p[i] = sBase->clone();

    for(int g=0;g<numGeracoes;g++)
    {
    	// =======================
    	//        Mutacao
    	// =======================

    	// Um individuo eh escolhido
    	int k = rand%(p.size());

    	Solution<R>* s = p[k];
    	p.erase(p.begin()+k);

    	// Mutacao ?
    	double xm = (rand()%1000)/1000;

    	if(xm<pMut)
    	{
    		Move<R,M>* mut = mutacao->move();

    		if(mut->canBeApplied(s))
    			mut->apply(s);
    	}

    	p.push_back(s);

		// =======================
		//      Recombinacao
		// =======================

    	// Um individuo eh escolhido
    	int k1 = rand%(p.size());

    	Solution<R>* s1 = p[k1];
    	p.erase(p.begin()+k1);

    	// Um individuo eh escolhido
    	int k2 = rand%(p.size());

    	Solution<R>* s2 = p[k2];
    	p.erase(p.begin()+k2);


    	// Recombinacao ?
    	double xp = (rand()%1000)/1000;

    	if(xp<pRecomb)
    	{
    		pair<Solution<R>*,Solution<R>*> filhos;
    		filhos = recombinacao->crossover(s1,s2);

    		if(filhos.first)
    			p.push_back(filhos.first);
    		else
    			cout << "Aviso: sem primeiro filho!" << endl;

    		if(filhos.second)
    			p.push_back(filhos.second);

    		// Retorna os pais
    		p.push_back(s1);
    		p.push_back(s2);

    	}


   		// Selecao Natural
   		// Elitismo

   		vector<Evaluation<M>*> ev(p.size());

   		for(int i=0;i<p.size();i++)
   			ev[i] = avaliador->evaluate(p[i]);

   		while(p.size() > tamPop)
   		{
   			int maximo = 0;
   			for(int i=1;i<p.size();i++)
   				if(ev[i].evaluation() > ev[maximo].evaluation())
   					maximo = i;

   			p.erase(p.begin()+maximo);
   			ev.erase(ev.begin()+maximo);
   		}
    }

    // =======================
    // Melhor de todos
	// =======================

	int melhor = 0;

    vector<Evaluation<M>*> ev(p.size());

   	for(int i=0;i<p.size();i++)
   		ev[i] = avaliador->evaluate(p[i]);

   	for(int i=1;i<p.size();i++)
   		if(ev[i].evaluation() > ev[melhor].evaluation())
   			melhor = i;

   	delete sStar;

   	sStar = p[melhor];

	while(p.size() > 0)
	{
		Solution<R>* s = p[0];
		delete s;
   		p.erase(p.begin()+0);
   	}

	while(ev.size() > 0)
	{
		Evaluation<M>* e = ev[0];
		delete ev;
   		ev.erase(ev.begin()+0);
   	}

#ifdef  AM_DEBUG
    cout << "=======================================\n";
    cout << "         Metodo de AM - Termino        \n";
    cout << "=======================================\n";
#endif
    // Retorna anterior
    return sStar;
}



#endif /*ALGORITMOSMEMETICOS_HPP_*/
