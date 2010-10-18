#ifndef ALGORITMOSGENETICOS_HPP_
#define ALGORITMOSGENETICOS_HPP_

#include "../../Heuristic.hpp"
#include "../../NS.h"
#include "../../Evaluator.hpp"

#include "CrossoverAG.h"

//#define AG_DEBUG

#ifdef  METHOD_DEBUG
#define DESCIDA_DEBUG
#endif


template<class R, class M>
class AlgoritmosGeneticos : public Heuristic<R,M>
{
public:
	AlgoritmosGeneticos(Evaluator<R,M>* avaliador, NS<R,M>* mutacao, CrossoverAG<R>* recombinacao, double pMut, double pRecomb, int tamPop, int numGeracoes);
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);
	
private:
	NS<R,M>* mutacao;
	CrossoverAG<R>* recombinacao;
    Evaluator<R,M>* avaliador;
    
    double pRecomb;
    double pMut;
    
    int tamPop;
    int numGeracoes;
};

/*
bool compara(Evaluation<M>* e1, Evaluation<M>* e2)
{
	return (e1->evaluation()) < (e2->evaluation());
}
*/

template<class R, class M>
AlgoritmosGeneticos<R,M>::AlgoritmosGeneticos(Evaluator<R,M>* avaliador, NS<R,M>* mutacao, CrossoverAG<R>* recombinacao, double pMut, double pRecomb, int tamPop, int numGeracoes)
{
    this->avaliador = avaliador;
    this->mutacao = mutacao;
    this->recombinacao = recombinacao;
    this->pRecomb = pRecomb;
    this->pMut = pMut;
    this->tamPop = tamPop;
    this->numGeracoes = numGeracoes;
}


template<class R, class M>
Solution<R>* AlgoritmosGeneticos<R,M>::exec(double efv, long timelimit, Solution<R>* sBase)
{
	cout << "AG exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);


	Solution<R>* sStar = sBase->clone();

	Evaluation<M>* eStar = avaliador->evaluate(sStar);

    // Populacao inicial
    vector<Solution<R>*> p(tamPop);

    for(int i=0;i<tamPop;i++)
    	p[i] = sBase->clone();

    int g = 0;

	long tatual = time(NULL);

    while(g<numGeracoes && ((tatual-tini)<timelimit))
    {
    	//cout << "Geracao (AG) "<<g<<endl;

    	for(int iter=0;iter<50;iter++)
    	{

    	// =======================
    	//        Mutacao
    	// =======================

    	int tam = (p.size());

    	// Um individuo eh escolhido
    	int k = rand()%tam;

    	Solution<R>* s = p[k];
    	p.erase(p.begin()+k);

    	//cout << "sorteando mut" << endl;

    	// Mutacao ?
    	double xm = (rand()%1000)/1000.0;


    	if(xm<pMut)
    	{
    		//cout << "xm = "<<xm << " (<"<<pMut<<")" << endl;
    		//cout << "\t>>>Mutacao!" << endl;
    		Move<R,M>* mut = mutacao->move();

    		if(mut->canBeApplied(s))
    			mut->apply(s);

    		Evaluation<M>* etemp = avaliador->evaluate(s);

    		if(avaliador->betterThan(etemp,eStar))
    		{
    			cout << "Geneticos geracao="<<g<<" (Mutacao): ";
    			delete eStar;
    			delete sStar;

    			sStar = s->clone();
    			eStar = etemp;

    			eStar->print();
    			g = -1;
    		}
    		else
    			delete etemp;

    	}

    	p.push_back(s);

		// =======================
		//      Recombinacao
		// =======================

    	// Um individuo eh escolhido
    	int k1 = rand()%(p.size());

    	Solution<R>* s1 = p[k1];
    	p.erase(p.begin()+k1);

    	// Um individuo eh escolhido
    	int k2 = rand()%(p.size());

    	Solution<R>* s2 = p[k2];
    	p.erase(p.begin()+k2);

    	//cout << "sorteando recomb" << endl;

    	// Recombinacao ?
    	double xp = (rand()%1000)/1000.0;

    	if(xp<pRecomb)
    	{
    		//cout << "xp = "<<xp << " (<"<<pRecomb<<")" << endl;
    		//cout << "\t>>>Recombinacao!" << endl;

    		pair<Solution<R>*,Solution<R>*> filhos;
    		filhos = recombinacao->crossover(s1,s2);

    		if(filhos.first)
    		{
    			p.push_back(filhos.first);

    			Evaluation<M>* etemp = avaliador->evaluate(filhos.first);

    			if(avaliador->betterThan(etemp,eStar))
    			{
    				cout << "Geneticos geracao="<<g<<" (Recombinacao f1): ";
    				delete eStar;
    				delete sStar;

    				sStar = (filhos.first)->clone();
    				eStar = etemp;

    				eStar->print();
    				g = -1;
    			}
    			else
    				delete etemp;

    		}
    		else
    			cout << "Aviso: sem primeiro filho!" << endl;

    		if(filhos.second)
    		{
    			p.push_back(filhos.second);

    			Evaluation<M>* etemp = avaliador->evaluate(filhos.second);

    			if(avaliador->betterThan(etemp,eStar))
    			{
    				cout << "Geneticos geracao="<<g<<" (Recombinacao f2): ";
    				delete eStar;
    				delete sStar;

    				sStar = (filhos.second)->clone();
    				eStar = etemp;

    				eStar->print();
    				g = -1;
    			}
    			else
    				delete etemp;


    		}


    	}

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
   			{
   				double fa = ev.at(i)->evaluation();
   				double fb = ev.at(maximo)->evaluation();
   				if(fa > fb)
   					maximo = i;
   			}

   			Solution<R>* sdel = p[maximo];
   			Evaluation<M>* edel = ev[maximo];
   			p.erase(p.begin()+maximo);
   			ev.erase(ev.begin()+maximo);

   			delete sdel;
   			delete edel;
   		}

   		// Avanca a geracao
   		g++;

   		// Atualiza o tempo
   		tatual = time(NULL);
    }

    // =======================
    // Melhor de todos (sStar)
	// =======================
	/*
	int melhor = 0;

    vector<Evaluation<M>*> ev(p.size());

   	for(int i=0;i<p.size();i++)
   		ev[i] = avaliador->evaluate(p[i]);

   	for(int i=1;i<p.size();i++)
   		if(ev.at(i)->evaluation() > ev.at(melhor)->evaluation())
   			melhor = i;

   	delete sStar;

   	sStar = p[melhor]->clone();
	*/

	while(p.size() > 0)
	{
		Solution<R>* s = p[0];
		delete s;
   		p.erase(p.begin()+0);
   	}

	/*
	while(ev.size() > 0)
	{
		Evaluation<M>* e = ev[0];
		delete e;
   		ev.erase(ev.begin()+0);
   	}
    */

#ifdef  AG_DEBUG
    cout << "=======================================\n";
    cout << "         Metodo de AG - Termino        \n";
    cout << "=======================================\n";
#endif
    // Retorna anterior
    return sStar;
}



#endif /*ALGORITMOSGENETICOS_HPP_*/
