#ifndef ILS_HPP_
#define ILS_HPP_

#include <math.h>
#include <vector>

#include "../../Heuristic.hpp"
#include "../../Evaluator.hpp"


#include "Perturbation.hpp"
#include "AcceptanceCriterion.hpp"
#include "TerminationCondition.hpp"

//#define ILS_DEBUG

#ifdef  METHOD_DEBUG
#define ILS_DEBUG
#endif

template<class R, class M, class H>
class IteratedLocalSearch : public Heuristic<R,M>
{
public:
	IteratedLocalSearch(Evaluator<R,M>* _eval, Heuristic<R,M>* _localSearch, Perturbation<R,H>* _pert, AcceptanceCriterion<R,H>* _ac, TerminationCondition<H>* _t , H* _history);
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);

private: 
	Evaluator<R,M>* evaluator;
	Heuristic<R,M>* localSearch;
	Perturbation<R,H>* perturbation;
	TerminationCondition<H>* termination;
	AcceptanceCriterion<R,H>* acceptance; 
	H* history;
};


//#include "IteratedLocalSearch.hpp"

/*
template<class R, class M>
IteratedLocalSearch<R,M>::IteratedLocalSearch(Evaluator<R,M>* _avaliador, Heuristic<R>* _heuristica, vector< NS <R,M> * > * _vizinhancas, int _iterMax, int _nivelMax)
:avaliador(_avaliador), heuristica(_heuristica), vizinhancas(_vizinhancas), iterMax(_iterMax), nivelMax(_nivelMax) { };
 */

template<class R, class M, class H>
IteratedLocalSearch<R,M,H>::IteratedLocalSearch(Evaluator<R,M>* _eval, Heuristic<R,M>* _localSearch, Perturbation<R,H>* _pert, AcceptanceCriterion<R,H>* _ac, TerminationCondition<H>* _t, H* _history)
{
	this->evaluator=_eval;
	this->localSearch=_localSearch;
	this->perturbation=_pert;
	this->termination=_t;
	this->acceptance=_ac;
	this->history = _history;
};


template<class R, class M, class H>
Solution<R>* IteratedLocalSearch<R,M,H>::exec(double efv, long timelimit, Solution<R>* s)
{
	cout << "ILS exec("<<efv<<","<<timelimit<<")"<<endl;

	long tini = time(NULL);

	// 's0' <- GenerateSolution

	// 's*' <- localSearch 's'

	long tatual = time(NULL);

	Solution<R>* sStar = localSearch->searchFT(efv, (timelimit - (tatual-tini)), s);

	delete s;

	Evaluation<M>* evalSStar = NULL;

	//cout << "First local search: " << evaluator->evaluate(sStar)->evaluation() << endl;


	do
	{
		//cout << "FIRST STAR: " << evaluator->evaluate(sStar)->evaluation() << "address = " << sStar << endl;

		tatual = time(NULL);

		//cout << "ILS: Tempo restante.." << (timelimit - (tatual-tini)) << endl;

		Solution<R>* nova = sStar->clone();

		Solution<R>* s1 = perturbation->perturb(nova,history);
		Solution<R>* s2 = localSearch->searchFT(efv, (timelimit - (tatual-tini))  ,s1);
		delete s1;

		delete nova;

		//cout << "STAR: " << evaluator->evaluate(sStar)->evaluation() << "address = " << sStar;
		//cout << "\t NEW VALUE: " << evaluator->evaluate(sStar)->evaluation() << endl;

		Solution<R>* sStar1 = acceptance->accept(sStar,s2,history);

		//cout << "NEW STAR: " << evaluator->evaluate(sStar1)->evaluation() << endl;


		delete sStar;
		delete s2;

		sStar = sStar1;

		evalSStar = evaluator->evaluate(sStar);

		//cout << "LAST STAR: " << evaluator->evaluate(sStar)->evaluation() << endl << endl;
	}
	while( evaluator->betterThan(efv, evalSStar->evaluation()) && !(termination->terminate(history)) && ((tatual-tini) < timelimit ) );

	delete evalSStar;

	return sStar;
}


#endif /*ILS_HPP_*/
