#ifndef SIMULATEDANNEALING_HPP_
#define SIMULATEDANNEALING_HPP_


#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

#include <math.h>

//#define DESCIDA_DEBUG

#ifdef  METHOD_DEBUG
#define SA_DEBUG
#endif


template<class R, class M>
class SimulatedAnnealing : public Heuristic<R,M>
{
public:
	SimulatedAnnealing(Evaluator<R,M>* avaliador, NS<R,M>* vizinhanca, double alfa, int SAmax, double Ti);
	SimulatedAnnealing(Evaluator<R,M>* avaliador, vector<NS<R,M>* > vizinhancas, double alfa, int SAmax, double Ti);
	virtual Solution<R>* exec(double efv, long timelimit, Solution<R>* s);

private:
	vector<NS<R,M>* > vizinhancas;
	Evaluator<R,M>* avaliador;
	double alfa;
	int SAmax;
	double Ti;
};


template<class R, class M>
SimulatedAnnealing<R,M>::SimulatedAnnealing(Evaluator<R,M>* _avaliador, NS<R,M>* _vizinhanca, double _alfa, int _SAmax, double _Ti)
{
	avaliador = _avaliador;
	vizinhancas.push_back(_vizinhanca);
	alfa = (_alfa);
	SAmax = (_SAmax);
	Ti=(_Ti);

};

template<class R, class M>
SimulatedAnnealing<R,M>::SimulatedAnnealing(Evaluator<R,M>* _avaliador, vector<NS<R,M>* > _vizinhancas, double _alfa, int _SAmax, double _Ti)
{
	avaliador = _avaliador;
	vizinhancas = (_vizinhancas);
	alfa = (_alfa);
	SAmax = (_SAmax);
	Ti=(_Ti);

};

template<class R, class M>
Solution<R>* SimulatedAnnealing<R,M>::exec(double efv, long timelimit, Solution<R>* sBase)
{
	cout << "SA "<< " exec("<<efv<<","<<timelimit<<")" << endl;

	long tini = time(NULL);

	Solution<R>* s = sBase->clone();

	delete sBase;

	Solution<R>* sStar = s->clone();


	Evaluation<M> * evalSStar = avaliador->evaluate(sStar);

	int IterT = 0;

	double T = Ti;

	long tatual = time(NULL);

	while( (T>0.001) && ((tatual-tini) < timelimit ))
	{
		//cout << "Temperatura = "<<T<<endl;


		while( (IterT<SAmax) && ((tatual-tini) < timelimit))
		{
			//cout << "Temp "<<T<<" IterT " <<IterT << endl;


			Solution<R>* s1;


			IterT = IterT + 1;
			// Gere um vizinho qualquer s1 de s

			s1 = s->clone();

			
			int numTries = 0;

         int v = rand()%vizinhancas.size();

			Move<R,M>* m = vizinhancas[v]->move(s1);

			while(!m->canBeApplied(s1))
			{
				numTries++;
				if(numTries>10*SAmax)
					cerr << "Warning: "<<10*SAmax<<" unsuccesful moves already generated in Simulated Annealing!" <<endl;

				delete m;
				
				v = rand()%vizinhancas.size();
				m = vizinhancas[v]->move(s1);
			}

			// ================================

			delete m->apply(s1);

			delete m;

			Evaluation<M> * evalS;
			Evaluation<M> * evalS1;

			evalS1 = avaliador->evaluate(s1);
			evalS  = avaliador->evaluate(s);

			double delta = evalS1->evaluation() - evalS->evaluation();

			delete evalS1;
			delete evalS;

			if(avaliador->betterThan(delta, 0))
			{
				//cout << "melhorou! continuar com S1 dps!" << endl;
				delete s;
				s = s1;

				Evaluation<M>* evalS1 = avaliador->evaluate(s1);

				if(avaliador->betterThan(evalS1,evalSStar))
				{
					cout << "Temp = "<<T<<"\t-> Melhora de fo: ";
					evalS1->print();

					delete sStar;
					sStar = s1->clone();

					delete evalSStar;
					evalSStar = avaliador->evaluate(sStar);
				}

				delete evalS1;
			}
			else
				if (((rand() % 1000)/1000.0) < exp(-fabs(delta)/T))
				{
					//cout << "aceitando piora! ";
					//cout << "nao melhorou, mas continuar com S1 msm!" << endl;
					delete s;
					s = s1;
				}
				else
					delete s1;

			tatual = time(NULL);
		}

		T = alfa * T;

		IterT = 0;
	}

	//cout << "Termino Annealing" << endl;

	delete s;
	s = sStar;

	return s;
}



#endif /*SIMULATEDANNEALING_HPP_*/
