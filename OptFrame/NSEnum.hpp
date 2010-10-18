#ifndef NSENUM_HPP_
#define NSENUM_HPP_

#include <vector>

#include "Evaluator.hpp"
#include "NSSeq.hpp"

// =============================
// Neighborhood Structure (Enum)
// Abstract Class
// =============================

using namespace std;

template<class R, class M>
class NSEnum  : public NSSeq<R,M>
{
protected:
	int move_k;

public:

	virtual Move<R,M>* move(R* r){return kmove(rand()%size());};

	virtual Move<R,M>* kmove(int k) = 0;

	virtual int size() = 0;

	// Metodo 'init' duplicado por eficiencia

	virtual void init(Solution<R>*)
	{
		move_k=0;
	}

	virtual void init(R*)
	{
		move_k=0;
	}

	// Metodo 'hasNext' duplicado por eficiencia

	virtual bool hasNext(Solution<R>* s)
	{
		return (move_k<size());
	}

	virtual bool hasNext(R* s)
	{
		return (move_k<size());
	}

	// Metodo 'next' duplicado por eficiencia

	virtual Move<R,M>* next(Solution<R>* r)
	{
		if(move_k>=size())
			return NULL; // Movimento nao existe!
		else
			return kmove(move_k++);
	}

	virtual Move<R,M>* next(R* s)
	{
		if(move_k>=size())
			return NULL; // Movimento nao existe!
		else
			return kmove(move_k++);
	}



	// ========================================================================================
	// Metodos herdados de NSSeq
	// ========================================================================================

	//virtual Move<R,M>* bestMove(Evaluator<R,M>* av, vector<Move<R,M>*> blacklist, Solution<R>* s);
	//virtual Move<R,M>* bestMove(Evaluator<R,M>* av, Solution<R>* s);

	//virtual Solution<R>* bestImprovement(Evaluator<R,M>* av, Solution<R>* base);

	// ========================================================================================
	// Metodos herdados de NSSeq
	// ========================================================================================

	//virtual Move<R,M>* firstMove(Evaluator<R,M>* av, vector<Move<R,M>*> blacklist, Solution<R>* s);
	//virtual Move<R,M>* firstMove(Evaluator<R,M>* av, Solution<R>* s);

	//virtual Solution<R>* firstImprovement(Evaluator<R,M>* av, Solution<R>* base);

	// ========================================================================================
	// ========================================================================================

	virtual void print() = 0;
};


#endif /*NSENUM_HPP_*/
