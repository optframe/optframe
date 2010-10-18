#ifndef NSSEQ_HPP_
#define NSSEQ_HPP_

#include <vector>

#include "Evaluator.hpp"
#include "NS.h"

// =============================
// Neighborhood Structure (Seq)
// Abstract Class
// =============================

using namespace std;

template<class R, class M>
class NSSeq  : public NS<R,M>
{
public:

	virtual void init(Solution<R>* s)
	{
		init(s->getRepresentation());
	}
	virtual void init(R*) = 0;

	virtual Move<R,M>* next(Solution<R>* s)
	{
		return next(s->getRepresentation());
	}
	virtual Move<R,M>* next(R*) = 0;

	virtual bool hasNext(Solution<R>* s)
	{
		return hasNext(s->getRepresentation());
	}

	// TODO: Usar Buffer para armazenar next e resolver de forma geral o hasNext
	virtual bool hasNext(R*) = 0;

	// =============================================================================

	virtual Solution<R>* bestImprovement(Evaluation<M>*, Evaluator<R,M>*, Solution<R>*);
	virtual Solution<R>* bestImprovement(Evaluator<R,M>*, Solution<R>*);

	virtual Move<R,M>* bestMove(Evaluation<M>*, Evaluator<R,M>*, vector<Move<R,M>*>, Solution<R>*);
	virtual Move<R,M>* bestMove(Evaluation<M>*, Evaluator<R,M>*, Solution<R>*);
	virtual Move<R,M>* bestMove(Evaluator<R,M>*, vector<Move<R,M>*>, Solution<R>*);
	virtual Move<R,M>* bestMove(Evaluator<R,M>*, Solution<R>*);

	// =============================================================================
	// =============================================================================

	virtual Solution<R>* firstImprovement(Evaluation<M>*, Evaluator<R,M>*, Solution<R>*);
	virtual Solution<R>* firstImprovement(Evaluator<R,M>* av, Solution<R>* base);

	virtual Move<R,M>* firstMove(Evaluation<M>*, Evaluator<R,M>*, vector<Move<R,M>*>, Solution<R>*);
	virtual Move<R,M>* firstMove(Evaluation<M>*, Evaluator<R,M>*, Solution<R>*);
	virtual Move<R,M>* firstMove(Evaluator<R,M>*, vector<Move<R,M>*>, Solution<R>*);
	virtual Move<R,M>* firstMove(Evaluator<R,M>*, Solution<R>*);

	// =============================================================================

	virtual void print() = 0;

//private:
	bool inList(Move<R,M>* move, vector<Move<R,M>*> list)
	{
		for(unsigned int i=0;i<list.size();i++)
			if(move->equals(list[i]))
				return true;
		return false;
	}

};

// =========================================================================================

template<class R, class M>
Move<R,M>* NSSeq<R,M>::bestMove(Evaluator<R,M>* av, Solution<R>* s)
{
	vector<Move<R,M>*> v;
	return bestMove(av,v,s);
};


template<class R, class M>
Move<R,M>* NSSeq<R,M>::bestMove(Evaluator<R,M>* av, vector<Move<R,M>*> blacklist, Solution<R>* s)
{
	// Inicializa a estrutura de vizinhanca
	init(s);

	// Melhor movimento eh o primeiro e unico!
	Move<R,M>* melhorMov = next(s);

	// Se nao existe movimento, desiste!!
	if(!melhorMov)
		return NULL;


	while(!(melhorMov->canBeApplied(s)) || inList(melhorMov,blacklist))
	{
		delete melhorMov;

		if(hasNext(s))
			melhorMov = next(s);
		else
			return NULL; // Desiste!
	}

	double melhorCusto = av->custoMovimento(melhorMov,s);


	while(hasNext(s))
	{
		Move<R,M>* mov = next(s);

		if(mov->canBeApplied(s) && !(inList(mov,blacklist)))
		{
			double custoMov = av->custoMovimento(mov,s);

			if(av->betterThan(custoMov,melhorCusto))
			{
				delete melhorMov;
				melhorMov = mov;
				melhorCusto = custoMov;
			}
			else
				delete mov;
		}
		else
			delete mov;

	}

	return melhorMov;
}

template<class R, class M>
Move<R,M>* NSSeq<R,M>::bestMove(Evaluation<M>* e, Evaluator<R,M>* av, vector<Move<R,M>*> blacklist, Solution<R>* s)
{
	// Inicializa a estrutura de vizinhanca
	init(s);

	// Melhor movimento eh o primeiro e unico!
	Move<R,M>* melhorMov = next(s);

	// Se nao existe movimento, desiste!!
	if(!melhorMov)
		return NULL;

	while(!(melhorMov->canBeApplied(s)) || inList(melhorMov,blacklist))
	{
		delete melhorMov;

		if(hasNext(s))
			melhorMov = next(s);
		else
			return NULL; // Desiste!
	}

	double melhorCusto = av->custoMovimento(e,melhorMov,s);

	while(hasNext(s))
	{
		Move<R,M>* mov = next(s);

		if(mov->canBeApplied(s) && !(inList(mov,blacklist)))
		{
 			double custoMov = av->custoMovimento(e,mov,s);

			if(av->betterThan(custoMov,melhorCusto))
			{
				delete melhorMov;
				melhorMov = mov;
				melhorCusto = custoMov;
			}
			else
				delete mov;
		}
		else
			delete mov;

	}
	return melhorMov;
}

template<class R, class M>
Move<R,M>* NSSeq<R,M>::bestMove(Evaluation<M>* e, Evaluator<R,M>* av, Solution<R>* s)
{
	vector<Move<R,M>*> v;
	return bestMove(e,av,v,s);
}

// =========================================================================================
// =========================================================================================

template<class R, class M>
Solution<R>* NSSeq<R,M>::bestImprovement(Evaluation<M>* e, Evaluator<R,M>* av, Solution<R>* base)
{
	Solution<R>* s = base->clone();

	Move<R,M>* m = bestMove(e,av,s);

	// Se existe movimento
	if(m)
	{
		Move<R,M>* r = m->apply(e,s);
		delete r;
		delete m;
	}

	return s;
}


template<class R, class M>
Solution<R>* NSSeq<R,M>::bestImprovement(Evaluator<R,M>* av, Solution<R>* base)
{
	Solution<R>* s = base->clone();

	Move<R,M>* m = bestMove(av,s);

	// Se existe movimento
	if(m)
	{
		Move<R,M>* r = m->apply(s);
		delete r;
		delete m;
	}

	return s;
}

// =========================================================================================
// =========================================================================================

template<class R, class M>
Move<R,M>* NSSeq<R,M>::firstMove(Evaluator<R,M>* av, Solution<R>* s)
{
	vector<Move<R,M>*> v;
	return firstMove(av,v,s);
};

template<class R, class M>
Move<R,M>* NSSeq<R,M>::firstMove(Evaluation<M>* e, Evaluator<R,M>* av, vector<Move<R,M>*> blacklist, Solution<R>* s)
{
	// Inicializa a estrutura de vizinhanca
	init(s);

	// Melhor movimento eh o primeiro e unico!
	Move<R,M>* melhorMov = next(s);

	// Se nao existe movimento, desiste!!
	if(!melhorMov)
		return NULL;


	while(!(melhorMov->canBeApplied(s)) || inList(melhorMov,blacklist))
	{
		delete melhorMov;

		if(hasNext(s))
			melhorMov = next(s);
		else
			return NULL;
	}

	double melhorCusto = av->custoMovimento(e,melhorMov,s);

	while(hasNext(s))
	{
		Move<R,M>* mov = next(s);

		if(mov->canBeApplied(s) && !(inList(mov,blacklist)))
		{
 			double custoMov = av->custoMovimento(e,mov,s);

			if(av->betterThan(custoMov,melhorCusto))
			{
				delete melhorMov;
				melhorMov = mov;
				melhorCusto = custoMov;

				break; // Sai do metodo com o primeiro melhor encontrado
			}
			else
				delete mov;
		}
		else
			delete mov;

	}

	return melhorMov;
}

template<class R, class M>
Move<R,M>* NSSeq<R,M>::firstMove(Evaluation<M>* e, Evaluator<R,M>* av, Solution<R>* s)
{
	vector<Move<R,M>*> v;
	return firstMove(e,av,v,s);
}

template<class R, class M>
Move<R,M>* NSSeq<R,M>::firstMove(Evaluator<R,M>* av, vector<Move<R,M>*> blacklist, Solution<R>* s)
{
	// Inicializa a estrutura de vizinhanca
	init(s);

	// Melhor movimento eh o primeiro e unico!
	Move<R,M>* melhorMov = next(s);

	// Se nao existe movimento, desiste!!
	if(!melhorMov)
		return NULL;


	while(!(melhorMov->canBeApplied(s)) || inList(melhorMov,blacklist))
	{
		delete melhorMov;

		if(hasNext(s))
			melhorMov = next(s);
		else
			return NULL; // Desiste!
	}

	double melhorCusto = av->custoMovimento(melhorMov,s);


	while(hasNext(s))
	{
		Move<R,M>* mov = next(s);

		if(mov->canBeApplied(s) && !(inList(mov,blacklist)))
		{
			double custoMov = av->custoMovimento(mov,s);

			if(av->betterThan(custoMov,melhorCusto))
			{
				delete melhorMov;
				melhorMov = mov;
				melhorCusto = custoMov;

				break; // Sai do metodo com o primeiro melhor encontrado
			}
			else
				delete mov;
		}
		else
			delete mov;

	}

	return melhorMov;
}


template<class R, class M>
Solution<R>* NSSeq<R,M>::firstImprovement(Evaluation<M>* e, Evaluator<R,M>* av, Solution<R>* base)
{
	Solution<R>* s = base->clone();

	Move<R,M>* m = firstMove(e,av,s);

	// Se existe movimento
	if(m)
	{
		Move<R,M>* r = m->apply(e,s);
		delete r;
		delete m;
	}

	return s;
}

template<class R, class M>
Solution<R>* NSSeq<R,M>::firstImprovement(Evaluator<R,M>* av, Solution<R>* base)
{
	Solution<R>* s = base->clone();

	Move<R,M>* m = firstMove(av,s);

	// Se existe movimento
	if(m)
	{
		Move<R,M>* r = m->apply(s);
		delete r;
		delete m;
	}

	return s;
}

// =========================================================================================
// =========================================================================================

#endif /*NSSEQ_HPP_*/
