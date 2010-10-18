#ifndef IILSL_HPP_
#define IILSL_HPP_

#include <math.h>
#include <vector>

#include "../../Heuristic.hpp"
#include "../../Evaluator.hpp"

#include "IteratedLocalSearch.hpp"
#include "IteratedLocalSearchLevels.hpp"
#include "IntensifiedIteratedLocalSearch.hpp"
#include "PerturbationLevels.hpp"


#define IILSL_DEBUG

#ifdef  METHOD_DEBUG
#define IILSL_DEBUG
#endif

using namespace std;

// par de pares
// primeiro par: Iter, Level
// segundo par : IterMax, LevelMax

template<class R, class M>
class IntensifiedIteratedLocalSearchLevels : public Heuristic<R,M>
{
private:
	Evaluator<R,M>* evaluator;
	Heuristic<R,M>* localSearch;
	Perturbation<R, levelHistory >* perturbation;
	int iterMax;
	int levelMax;
	Intensification<R,M,levelHistory>* intensification;

public:
	IntensifiedIteratedLocalSearchLevels(Evaluator<R,M>* e, Heuristic<R,M>* l, PerturbationLevels<R>* p, Intensification<R,M,levelHistory>* _intens, int _iterMax, int _levelMax)
	{
		evaluator = e;
		localSearch = l;
		perturbation = p;
		iterMax = _iterMax;
		levelMax = _levelMax;
		intensification = _intens;
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
		// Intensified Iterated Local Search
		// ============================================

		intensification->setHistory(h);

		IntensifiedIteratedLocalSearch<R,M, levelHistory > ils (evaluator, localSearch, perturbation, ac, term, h, intensification );

		Solution<R>* sol = ils.searchFT(efv, timelimit, s);

		delete s;

		return sol;	
	}
};

#endif /*IILSL_HPP_*/
