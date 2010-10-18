#ifndef HEURISTIC_HPP_
#define HEURISTIC_HPP_

#include <iostream>

using namespace std;

#include "./Solution.h"
#include "./Evaluation.hpp"
#include "./Util/Runnable/Runnable.hpp"

template<class R, class M>
class Heuristic: public Runnable
{
protected:
	Solution<R>* log_solution;
	vector<Solution<R>*> log_solution_set;
	long log_timelimit;
	double log_efv;

public:

	Heuristic():Runnable()
	{
		log_solution=NULL;
	};
	virtual ~Heuristic(){};

	static const int SAFE_SEARCH_TOLERANCE = 2; // tolerance in seconds

	virtual void run()
	{
		if(log_solution) // if not null
			log_solution = exec(log_efv, log_timelimit,log_solution);
		else
			log_solution_set = exec(log_efv, log_timelimit,log_solution_set);
	}

	/// SafeSearch guarantees that your method will not loop! But your method may become very slow... avoid using this method.
	/**
		safeSearch é um método Solução -> Solução, parametrizado pelo tempo
		máximo de execução desejado para a heurística.
		Para garantir que a heurística terminará no tempo limite desejado o
		método é iniciado em uma thread que é encerrada logo após esse tempo.\n
		Caso o método termine normalmente sua solução é retornada; caso contrário
		uma solução NULL é retornada.
	 */
	virtual Solution<R>* safeSearch(double efv, long timelimit, Solution<R>* base)
	{
		if(timelimit<=0)
			return base->clone();

		log_solution = base->clone();
		log_solution_set.clear();
		log_timelimit = timelimit;
		log_efv = efv;

		start();

		for(int t=0;t<timelimit;t++)
			if(!finished())
				sleep(1);
			else
				return log_solution;

		for(int t=0;t<SAFE_SEARCH_TOLERANCE;t++)
			if(!finished())
			{
				cout << "Warning: Process not finished yet. Safe Search Tolerance ("<<t<<" secs.)"<<endl;
				sleep(1);
			}
			else
				return log_solution;

		stop();

		cout << "Warning: Safe Search saved method from looping! (maybe +1 solution memory lack)" << endl;
		return base->clone();
	}

	virtual vector<Solution<R>*> safeSearch(double efv, long timelimit, vector<Solution<R>*> base)
	{
		if(timelimit<=0)
			return base;

		vector<Solution<R>*> v;
		for(int i=0;i<base.size();i++)
			v.push_back(base[i]->clone());

		log_solution = NULL;
		log_solution_set = v;
		log_timelimit = timelimit;
		log_efv = efv;

		start();

		for(int t=0;t<timelimit;t++)
			if(!finished())
				sleep(1);
			else
				return log_solution_set;

		for(int t=0;t<SAFE_SEARCH_TOLERANCE;t++)
			if(!finished())
			{
				cout << "Warning: Process not finished yet. Safe Search Tolerance ("<<t<<" secs.)"<<endl;
				sleep(1);
			}
			else
				return log_solution_set;

		stop();

		cout << "Warning: Safe Search saved method from looping! (maybe +1 solution memory lack)" << endl;
		return base;
	}

	///Executa a heurística, dentro de um tempo limite.
	virtual Solution<R>* searchFT(double efv, long timelimit, Solution<R>* base)
	{
		if(timelimit<=0)
			return base->clone();

		// Cópia da solução base

		Solution<R>* s = base->clone();

		//cout << "pos clone()";
		// Executa heurística de acordo com o tempo limite
		return exec(efv, timelimit,s);
	}

	///Executa a heurística, dentro de um tempo limite.
	virtual vector<Solution<R>*> searchFT(double efv, long timelimit, vector<Solution<R>*> base)
	{
		if(timelimit<=0)
			return base;

		vector<Solution<R>*> v;
		for(int i=0;i<base.size();i++)
			v.push_back(base[i]->clone());

		// Executa heurística de acordo com o tempo limite
		return exec(efv, timelimit,v);
	}

	///Executa a heurística, sem tempo limite.
	virtual Solution<R>* search(Solution<R>* s)
	{
		// Heurística executada sem tempo limite
		//unsigned long maxtimelimit = ULONG_MAX;
		long maxtimelimit = 1000000000;

		//Heurística executada sem limite de fo
		//O valor alvo de fo passado por default é 0
		double fo_alvo = 0;

		// Executa heurística com o tempo limite de 120 anos aprox. =]
		return searchFT(fo_alvo, maxtimelimit, s);
	}

	virtual vector<Solution<R>*> search(vector<Solution<R>*> ss)
	{
		// Heurística executada sem tempo limite
		//unsigned long maxtimelimit = ULONG_MAX;
		long maxtimelimit = 1000000000;

		//Heurística executada sem limite de fo
		//O valor alvo de fo passado por default é 0
		double fo_alvo = 0;

		// Executa heurística com o tempo limite de 120 anos aprox. =]
		return searchFT(fo_alvo, maxtimelimit, ss);
	}


	///Método abstrato de execução da heurística.
	/**
	 exec é o método que deve ser implementado para executar a heurística desejada.
	 A solução 's' de parâmetro já é uma solução copiada ao ser repassada para esse
	 método, logo podes alterá-la que não influenciará no contexto geral do sistema.

	 */


	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s)
	{
		return exec(efv,timelimit,s,NULL);
	}

	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s, Evaluation<M>* e) // 'e' can be 'null'
	{
		vector<Solution<R>*> v;
		v.push_back(s);
		v = exec(efv,timelimit,v);
		return v[0];
	}

	virtual vector<Solution<R>*> exec(double efv, long timelimit, vector<Solution<R>*> ss)
	{
		cout << "exec_ss" << endl;
		Solution<R>* s = ss[0];
		s = exec(efv,timelimit,s);
		vector<Solution<R>* > v;
		v.push_back(s);
		return v;
	}
};

#endif /* HEURISTIC_HPP_ */
