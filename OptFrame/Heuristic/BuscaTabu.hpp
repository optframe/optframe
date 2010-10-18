#ifndef BUSCATABU_HPP_
#define BUSCATABU_HPP_


#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

//#include "FuncaoAspiracao.h"

//#define BUSCATABU_DEBUG

#ifdef  METHOD_DEBUG
#define BUSCATABU_DEBUG
#endif


template<class R, class M>
class BuscaTabu : public Heuristic<R,M>
{
public:
    BuscaTabu(Evaluator<R,M>* _avaliador, NSEnum<R,M>* _vizinhanca, int _tamT, int _BTmax)
    {
    	avaliador = _avaliador;
    	vizinhanca = _vizinhanca;
    	tamT = _tamT;
    	BTmax = _BTmax;    	
    };

	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);

private:
	NSEnum<R,M>* vizinhanca;
    Evaluator<R,M>* avaliador;
    int tamT;
    int BTmax;
};




template<class R, class M>
Solution<R>* BuscaTabu<R,M>::exec(double efv, long timelimit, Solution<R>* s)
{
	cout << "BuscaTabu exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);

    Solution<R>* sStar = s->clone();
    Evaluation<M>* evalSStar = avaliador->evaluate(sStar);

    Evaluation<M>* evalS = avaliador->evaluate(s);

    int Iter = 0;

    int MelhorIter = 0;

    vector<Move<R,M>* > tabuList;

    //Inicialize a funcao de aspiracao A

	long tatual = time(NULL);

    while( Iter - MelhorIter <= BTmax && ((tatual-tini)<timelimit))
    {
    	Iter = Iter + 1;

    	cout << "Iter "<<Iter <<" ("<<(Iter - MelhorIter - 1)<<" sem melhora)"<< endl;

    	// =====================================================
    	// Primeiro caso: ver se aspira
    	// =====================================================

    	Move<R,M>* bestMove = vizinhanca->bestMove(avaliador,s);

    	Solution<R>* s1 = s->clone();

    	Move<R,M>* newTabu = bestMove->apply(s1);
    	Evaluation<M>* evalS1 = avaliador->evaluate(s1);

    	// Vamos ver se melhora a global...
    	if(avaliador->betterThan(evalS1,evalSStar))
    	{
    		// Melhorou! =)

    		/*
    		//Atualiza a global
    		delete sStar;
    		delete evalSStar;
    		sStar=s1;
    		evalSStar=evalS1;
    		*/

    		//Vamos (ainda..) ver se aspiramos o movimento da lista!
    		for(unsigned int i=0;i<tabuList.size();i++)
    			if(tabuList[i]->equals(bestMove) )
    				{
    					tabuList.erase(tabuList.begin()+i);
    					break;
    				}

    	}
    	else
    	{
    		// Nao melhorou a global =(
    		// Vai nos tabu msm =P

    		delete s1;
    		delete evalS1;
    		delete newTabu;
    		delete bestMove;

    		bestMove = vizinhanca->bestMove(avaliador,tabuList,s);
    		s1 = s->clone();
    		newTabu = bestMove->apply(s1);
    	    evalS1 = avaliador->evaluate(s1);

    	    delete bestMove;
    	}

    	// =====================================================
    	//    |T| <= tamT
    	// =====================================================

    	// E vamos inserir o novo tabu
    	tabuList.push_back(newTabu);

    	// Atualiza tamanho da lista
    	if(tabuList.size()>tamT)
    		tabuList.erase(tabuList.begin());

    	//-----------------------------------------------------------
    	//  Conferencia de seguranca...
    	//-----------------------------------------------------------
    	if(tabuList.size()>tamT)
    		throw string("ERRO no Busca Tabu!! Elementos a mais...");
    	//-----------------------------------------------------------


    	// =====================================================
    	//        's' <- 's1';
    	// =====================================================

    	delete s;
    	delete evalS;

    	s = s1;
    	evalS = evalS1;


    	// =====================================================
    	//        's' better than 's*' (?)
    	// =====================================================


    	if(avaliador->betterThan(evalS,evalSStar))
    	{
    		delete sStar;
    		delete evalSStar;

    		sStar =	s->clone();
    		evalSStar = avaliador->evaluate(sStar);

    		MelhorIter = Iter;

    		cout << "Melhora em "<< MelhorIter <<": fo="<<evalSStar->evaluation()<<endl;
    	}

    	tatual = time(NULL);
    }

    // ===========
    // 's' <- 's*'
    // ===========

    delete s;
	delete evalS;
	delete evalSStar;

	s = sStar;

    // Return 's'
    return s;
}




#endif /*BUSCATABU_HPP_*/
