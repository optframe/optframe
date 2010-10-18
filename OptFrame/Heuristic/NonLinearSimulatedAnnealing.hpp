#ifndef NONLINEARSIMULATEDANNEALING_HPP_
#define NONLINEARSIMULATEDANNEALING_HPP_

#include "../Random.h"
#include "../Function.h"

#include <math.h>

//#define DESCIDA_DEBUG

#ifdef  METHOD_DEBUG
#define NLSA_DEBUG
#endif


class NonLinearSimulatedAnnealing : public Heuristic<vector<double> >
{
public:
	NonLinearSimulatedAnnealing(Random* _r, Function* _f, int _a, int _b);

	virtual Solution<vector<double> >* exec(double efv, long timelimit, Solution<vector<double> >* s);

private:
    Function* f;
    Random* r;
    int a;
    int b;

};


NonLinearSimulatedAnnealing::NonLinearSimulatedAnnealing(Random* _r, Function* _f, int _a, int _b)
{
   r = _r;
   f = _f;
   a = _a;
   b = _b;
};


Solution<vector<double> >* NonLinearSimulatedAnnealing::exec(double efv, long timelimit, Solution<vector<double> >* s)
{
/*

	Solution<vector<double> >* sStar = s->clone();
	Evaluation<M> * evalSStar = avaliador->evaluate(sStar);

    Solution<R>* s1 = NULL;

	int IterT = 0;

	double T = Ti;

	while(T > 0.00001)
	{
		//cout << "Temperatura = "<<T<<endl;

		while(IterT < SAmax)
		{
			IterT = IterT + 1;
			// Gere um vizinho qualquer s1 de s

			s1 = s->clone();

			Move<R,M>* m = vizinhanca->move();
			m->apply(s1);

			Evaluation<M> * evalS;
			Evaluation<M> * evalS1;

			evalS1 = avaliador->evaluate(s1);
			evalS  = avaliador->evaluate(s);

			double delta = evalS1->evaluation() - evalS->evaluation();

			if(delta < 0)
			{

				delete s;
				delete evalS;

				s = s1;
				evalS = evalS1;

				if(evalS1->evaluation() < evalSStar->evaluation())
				{
					delete sStar;
					delete evalSStar;

					sStar = s1->clone();
					//evalSStar = evalS1;
					evalSStar = avaliador->evaluate(sStar);

					cout << "Melhora: " << evalSStar->evaluation() << endl;


				}

				s1 = NULL;
				evalS1 = NULL;

			}
			else
			{

				double x = (rand() % 1000)/1000;

				if (x < exp(-delta/T))
				{
					delete s;
					delete evalS;

					s = s1;
					evalS = evalS1;

					s1 = NULL;
					evalS1 = NULL;
				}
				else
				{
					delete s1;
					delete evalS1;

					s1 = NULL;
					evalS1 = NULL;
				}
			}

		}

		T = alfa * T;

		IterT = 0;
	}

	//cout << "Término Annealing" << endl;

	delete s;

	s = sStar;

*/
	return s;
}



#endif /*NONLINEARSIMULATEDANNEALING_HPP_*/
