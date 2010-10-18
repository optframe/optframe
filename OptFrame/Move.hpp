#ifndef MOVE_HPP_
#define MOVE_HPP_

#include "Solution.h"
#include "Evaluation.hpp"

using namespace std;

/// Classe abstrata para a definição de um movimento
template<class R, class M>
class Move
{
public:

	/* ====================================================
	 *
	 * bool canBeApplied(Solution)
	 *
	 * Retorna verdadeiro se o movimento pode ser aplicado
	 * a solucao do parametro, no contexto atual.
	 *
	 * Pode tambem ser utilizado para conferir possiveis
	 * inconsistencias na geracao dos movimentos.
	 * ====================================================
	 */

	virtual bool canBeApplied(Solution<R>* s)
	{
		return canBeApplied(s->getRepresentation());
	}
	virtual bool canBeApplied(R*) = 0;


	/* ====================================================
	 *
	 * Move* apply(Avaliacao*, Solution*)
	 *
	 * Aplica o movimento atual a solucao em parametro e
	 * retorna o movimento reverso (capaz de desfazer o
	 * efeito desta aplicacao atual), e tambem atualiza a
	 * a avaliacao em parametro para uma reavaliacao
	 * posterior. Por default, ignora a avaliação anterior.
	 *
	 * ====================================================
	 */

	virtual Move<R,M>* apply(Evaluation<M>* e, Solution<R>* s)
    {
		// Default Implementation!
		// call: Move<R,M>* apply(Evaluation<M>* e, R* r)

		return apply(e, s->getRepresentation());
    }

	virtual Move<R,M>* apply(Evaluation<M>* e, R* r)
    {
		// Default Implementation!
		// call: Move<R,M>* apply(Solution<R>* s)

		Solution<R>* s = new TestSolution<R>(r);

		Move<R,M>* m = apply(s);
		s->setRepresentation(NULL);
		delete s;

		return m;
    }

	/* ====================================================
	 *
	 * Move* apply(Solution)
	 *
	 * Aplica o movimento atual a solucao em parametro e
	 * retorna o movimento reverso (capaz de desfazer o
	 * efeito desta aplicacao atual).
	 *
	 * ====================================================
	 */

	virtual Move<R,M>* apply(Solution<R>* s)
    {
		// Default Implementation!
		// call: Move<R,M>* apply(R*)

		return apply(s->getRepresentation());
    }

	virtual Move<R,M>* apply(R* r)
	{
		// Default Implementation!
		// call: Move<R,M>* apply(Evaluation<M>* e, Solution<R>* s)

		Solution<R>* s = new TestSolution<R>(r);

		Evaluation<M>* default_eval = new Evaluation<M>;
		Move<R,M>* m = apply(default_eval,s);
		delete default_eval;

		s->setRepresentation(NULL);
		delete s;

		return m;
	}


	/* ====================================================
	 *
	 * bool equals(Move*)
	 *
	 * Retorna a igualdade entre um movimento da mesma classe.
	 * ====================================================
	 */
	virtual bool equals(Move<R,M>* m) = 0;


	/* ====================================================
	 *
	 * void print()
	 *
	 * Representacao visual deste movimento. (Necessaria
	 * para operacoes de debug na estrutura de vizinhanca).
	 *
	 * ====================================================
	 */
	virtual void print() = 0;
};

#endif /*MOVE_HPP_*/
