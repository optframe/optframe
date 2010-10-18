#ifndef VARIABLENEIGHBORHOODDESCENT_HPP_
#define VARIABLENEIGHBORHOODDESCENT_HPP_


#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

//#define VND_DEBUG

#ifdef  METHOD_DEBUG
#define VND_DEBUG
#endif


template<class R, class M>
class VariableNeighborhoodDescent : public Heuristic<R,M>
{
public:
	VariableNeighborhoodDescent(Evaluator<R,M>* avaliador, vector< NSSeq<R,M>* > vizinhancas);
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);
	
private:
	vector< NSSeq<R,M>* > vizinhancas;
    Evaluator<R,M>* avaliador;
};



template<class R, class M>
VariableNeighborhoodDescent<R,M>::VariableNeighborhoodDescent(Evaluator<R,M>* avaliador, vector< NSSeq<R,M>* > vizinhancas)
{
    this->avaliador = avaliador;
    this->vizinhancas = vizinhancas;
}


template<class R, class M>
Solution<R>* VariableNeighborhoodDescent<R,M>::exec(double efv, long timelimit, Solution<R>* s)
{
	cout << "VND exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);

#ifdef  VND_DEBUG
	cout << "============================================\n";
	cout << "             Metodo VND - Inicio            \n";
	cout << "============================================\n";
#endif

	int r = vizinhancas.size();

	int k = 1;

	Evaluation<M>* e = avaliador->evaluate(s);


	//cout << "Inicio do VND: " << e->evaluation() << endl;

	long tatual = time(NULL);

	while(avaliador->betterThan(efv, e->evaluation()) &&  (k <= r) && ((tatual-tini)<timelimit))
	{
#ifdef  VND_DEBUG
		cout << "k e r => " << k << " " << r << ": ";
		vizinhancas[k-1]->toString();
		cout << endl;
#endif
		// Limpa a melhor fo atual
		//delete e;

		Solution<R>* sl = vizinhancas[k-1]->bestImprovement(e,avaliador, s);

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

		avaliador->evaluate(e,s);

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
	{
		cout << ">>>>>>>> Saida do VND por FO!!" << endl;
		cout << e->evaluation()<< " melhor que "<<efv << endl;
	}
    // ===============================================================

	//cout << "Termino do VND: " << e->evaluation() << endl << endl;

    // Limpa a melhor fo
	delete e;

    // Retorna 's'
    return s;
}



#endif /*VARIABLENEIGHBORHOODDESCENT_HPP_*/
