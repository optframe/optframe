#ifndef VARIABLEHEURISTICDESCENT_HPP_
#define VARIABLEHEURISTICDESCENT_HPP_


#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

//#define VND_DEBUG

#ifdef  METHOD_DEBUG
#define VND_DEBUG
#endif


template<class R, class M>
class VariableHeuristicDescent : public Heuristic<R,M>
{
public:
	VariableHeuristicDescent(Evaluator<R,M>* avaliador, vector< Heuristic<R,M>* > localSearches);
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);
	
private:
	vector< Heuristic<R,M>* > localSearches;
    Evaluator<R,M>* avaliador;
};



template<class R, class M>
VariableHeuristicDescent<R,M>::VariableHeuristicDescent(Evaluator<R,M>* avaliador, vector< Heuristic<R,M>* > localSearches)
{
    this->avaliador = avaliador;
    this->localSearches = localSearches;
}


template<class R, class M>
Solution<R>* VariableHeuristicDescent<R,M>::exec(double efv, long timelimit, Solution<R>* s)
{
	cout << "VHD "<< " exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);

#ifdef  VND_DEBUG
	cout << "============================================\n";
	cout << "             Metodo VND - Inicio            \n";
	cout << "============================================\n";
#endif

	int r = localSearches.size();

	int k = 1;

	Evaluation<M>* e = avaliador->evaluate(s);

	long tatual = time(NULL);

	while(avaliador->betterThan(efv, e->evaluation()) &&  (k <= r) && ((tatual-tini) < timelimit))
	{
#ifdef  VND_DEBUG
		cout << "k e r => " << k << " " << r << ": ";
		//localSearchs[k-1]->toString();
		cout << endl;
#endif
		// Limpa a melhor fo atual
		delete e;

		Solution<R>* sl = localSearches[k-1]->searchFT(efv, (timelimit-(tatual-tini)), s);   //bestNeighbor(avaliador, s);

		if(avaliador->betterThan(sl, s))
		{
			delete s;
			s = sl;
			k = 1;
		}
		else
		{
			delete sl;
			k = k + 1;
		}

		e = avaliador->evaluate(s);

		tatual = time(NULL);
	}

#ifdef  VND_DEBUG
	cout << "============================================\n";
	cout << "             Metodo VND - Termino           \n";
	cout << "============================================\n";
#endif

	// ==== APENAS INFORMATIVO (PODEMOS REMOVER APOS INCORPORAR LOGs
	// Informacao de motivo de saida
	if (avaliador->betterThan(e->evaluation(),efv))
		cout << ">>>>>>>> Saida do VND por FO!!" << endl;
    // ===============================================================

    // Limpa a melhor fo
	delete e;

    // Retorna 's'
    return s;
}




#endif /*VARIABLEHEURISTICDESCENT_HPP_*/
