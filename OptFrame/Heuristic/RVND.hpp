#ifndef RVND_HPP_
#define RVND_HPP_

#include <algorithm>

#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

//#define RVND_DEBUG

#ifdef  METHOD_DEBUG
#define RVND_DEBUG
#endif


template<class R, class M>
class RVND : public Heuristic<R,M>
{
public:
	RVND(Evaluator<R,M>* avaliador, vector< NSEnum<R,M>* > vizinhancas);
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);
	
private:
	vector< NSEnum<R,M>* > v;
    Evaluator<R,M>* avaliador;
};



template<class R, class M>
RVND<R,M>::RVND(Evaluator<R,M>* avaliador, vector< NSEnum<R,M>* > vizinhancas)
{
    this->avaliador = avaliador;
    this->v = vizinhancas;
}


template<class R, class M>
Solution<R>* RVND<R,M>::exec(double efv, long timelimit, Solution<R>* s)
{
	cout << "RVND exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);

#ifdef  VND_DEBUG
	cout << "============================================\n";
	cout << "            Metodo RVND - Inicio            \n";
	cout << "============================================\n";
#endif

	// =======================================
	// Random Neighborhoods
	// =======================================

	vector< NSEnum<R,M>* > vizinhancas(v.size());
	for(unsigned int i=0;i<v.size();i++)
		vizinhancas[i]=v[i];

	std::random_shuffle( vizinhancas.begin(), vizinhancas.end() ); // shuffle elements

	// =======================================

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
	cout << "            Metodo RVND - Termino           \n";
	cout << "============================================\n";
#endif

	// ==== APENAS INFORMATIVO (PODEMOS REMOVER APOS INCORPORAR LOGs
	// Informacao de motivo de saida
	if (avaliador->betterThan(e->evaluation(),efv))
	{
		cout << ">>>>>>>> Saida do RVND por FO!!" << endl;
		cout << e->evaluation()<< " melhor que "<<efv << endl;
	}
    // ===============================================================

	//cout << "Termino do VND: " << e->evaluation() << endl << endl;

    // Limpa a melhor fo
	delete e;

    // Retorna 's'
    return s;
}



#endif /*RVND_HPP_*/
