#ifndef ILSL_HPP_
#define ILSL_HPP_

#include <math.h>
#include <vector>

#include "../../Heuristic.hpp"
#include "../../Evaluator.hpp"

#include "IteratedLocalSearch.hpp"
#include "PerturbationLevels.hpp"


//#define ILSL_DEBUG

#ifdef  METHOD_DEBUG
#define ILSL_DEBUG
#endif

using namespace std;

// par de pares
// primeiro par: Iter, Level
// segundo par : IterMax, LevelMax




template<class R, class M>
class AcceptanceCriterionLevel: public AcceptanceCriterion< R, levelHistory >
{
private:
	Evaluator<R,M>* eval;
	
public: 

    AcceptanceCriterionLevel(Evaluator<R,M>* _eval)
    {
    	this->eval = _eval;    	
    }
      
	virtual Solution<R>* exec (Solution<R>* sStar, Solution<R>* s2, levelHistory* history)
	{
		//cout << "sStar = " << eval->evaluate(sStar)->evaluation() << endl;
		//cout << "s2 = " << eval->evaluate(s2)->evaluation() << endl;


		if(eval->betterThan(s2,sStar))
		{
			// =======================
			//   Melhor solucao: 's2'
			// =======================
			Evaluation<M>* e = eval->evaluate(s2);
			cout << "Best fo: "<< e->evaluation();
			cout << " on [iter "<<history->first.first<<" of level "<<history->first.second<<"]"<<endl;
			delete e; 

			// =======================
			//  Atualiza o historico
			// =======================
			// iter = 0
			history->first.first = 0;			
			// level = 0
			history->first.second = 0;
			
						
			// =======================
			//    Retorna s2
			// =======================			
			return s2;			
		}
		else
			return sStar;
	}
};




class TerminationConditionLevels : public TerminationCondition< levelHistory >
{
public:
    // Default implementation
    virtual bool terminate (levelHistory* history)
    {
		int level = history->first.second;
    	int levelMax = history->second.second;
    	
    	return (level >= levelMax);
    }
};



template<class R, class M>
class IteratedLocalSearchLevels : public Heuristic<R,M>
{
public:
	IteratedLocalSearchLevels(Evaluator<R,M>* e, Heuristic<R,M>* l, PerturbationLevels<R>* p, int _iterMax, int _levelMax)
	{
		evaluator = e;
		localSearch = l;
		perturbation = p;
		iterMax = _iterMax;
		levelMax = _levelMax;		
	}

	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s)
	{
		// ============================================
		//        Inicializacao do historico 'h'
		// ============================================
		
		// Iter e Level
		pair<int,int> vars(0,0);
		
		// IterMax e LevelMax
		pair<int,int> maxs(iterMax,levelMax);
		
		levelHistory* h = new levelHistory(vars,maxs);
						
		// ============================================
		// Criterio de aceitacao
		// ============================================
		
		AcceptanceCriterion<R, levelHistory >* ac = new AcceptanceCriterionLevel<R,M>(evaluator);
		
		// ============================================
		// Condicao de parada
		// ============================================
		
		TerminationCondition< levelHistory >* term = new TerminationConditionLevels();
		
		// ============================================
		// Iterated Local Search
		// ============================================
		
		IteratedLocalSearch<R,M, levelHistory > ils (evaluator, localSearch, perturbation, ac, term, h );
		
		Solution<R>* sol = ils.searchFT(efv, timelimit, s);
		
		delete s;
		
		return sol;	
	}

private:
    Evaluator<R,M>* evaluator;
    Heuristic<R,M>* localSearch;
    Perturbation<R, levelHistory >* perturbation;
    int iterMax;
    int levelMax;
};

#endif /*ILSL_HPP_*/
