#ifndef VRPTW_EVALUATOR_HPP_
#define VRPTW_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../Evaluation.hpp"
#include "../../Evaluator.hpp"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

double maxdouble(double a,double b){return a>=b?a:b;}

class VRPTWEvaluator: public Evaluator<RepVRPTW, MemVRPTW>
{
private:
	VRPTWProblemInstance* input;

   // Your private vars
	bool debug;


public:
	VRPTWEvaluator(VRPTWProblemInstance* input) // If necessary, add more parameters
	{
           this->input = input;

           // Put the rest of your code here
	}

	virtual EvaluationVRPTW* evaluate(SolutionVRPTW* s)
	{
            double fo = eval(*s->getRepresentation()).first;
            MemVRPTW* reval = NULL; // Memory for fast Re-evaluation			
            return new Evaluation<MemVRPTW>(fo,reval);
	}

	pair<double,bool> eval(RepVRPTW & rep)
    {
		//completeWithZeros(rep);

            //RepVRPTW* rep = s->getRepresentation(); // Representation of the Solution

            double fo=0, t=0, w=0, d=0, e=0; // Evaluation Function Value

			/* Avaliação de uma solução */
	        if (debug) cout << "VRPTW Analytics\n" << endl;
			for (int i = 0 ; i < rep.size() ; i++)
			{
				if (debug) cout << "========================\nRoute " << i << "\n------------------------" << endl;
				if (rep[i].size()==0) continue;
				t+=totalTime((rep)[i]);
				//w+=waittime((rep)[i]);
				d+=delay((rep)[i]);
				e+=excLoad((rep)[i]);
			}
	        if (debug) cout << "========================\nFinal Analysis" << endl;
			fo=t+100*w+200*d+300*e;
			/*  */

			//removeZeros(rep);

			return pair<double,bool>(fo,w==0&&d==0&&e==0);
    }
	
	double totalTime(vector<int> &route) // d(s)
	{
		if (route.size()==0) return 0;

		double v = 0;

		//for (int i = 0 ; i < route.size()-1 ; i++) { v += input->dist[route[i]][route[i+1]]; } // [0 .. 0]

		v = input->dist[0][route[0]];
		for (int i = 0 ; i < route.size()-1 ; i++) { v += input->dist[route[i]][route[i+1]]; }
		v += input->dist[route[route.size()-1]][0];

		if (debug) cout << "Total Time:\t\t" << v << endl;
		return v;
	}
	double maxLoad(vector<int> &route) // q(s)
	{
        int carga = 0, cargamax = -1;
        //for (int i = 0 ; i < route.size()-1 ; i++) 
        for (int i = 0 ; i < route.size() ; i++) 
        {
            carga += input->d[route[i]];
            if (carga > cargamax) { cargamax = carga; }
        }
		if (debug) cout << "Max. Load:\t\t" << cargamax << endl;        
		return cargamax;
	}
	double excLoad(vector<int> &route) // q(s) (fo)
	{
		double v = maxdouble(0,maxLoad(route)-input->c);
		if (debug) cout << "Exc. Max. Load:\t\t" << v << endl; 
		return v;
	}
	pair<double,double> wait_delay(vector<int> &route) // w(s) (fo)
	{
		route.insert(route.begin(),0);
		route.push_back(0);

        double duracao = 0;
		// janela de tempo
		int ant=0,atr=0;
		vector<int> tis(route.size(),0);

		for (unsigned int i = 1 ; i < route.size() ; i++) // cap
		{
				int a = route[i-1], b = route[i];

				int dab = input->dist[a][b]; // dist

				tis[i] = tis[i-1] // saída do anterior
                       //+ input->vs[route[i-1]].trota // tempo de serviço
				       + dab; // tempo até o atual

				tis[i] < input->ti[route[i]] ? tis[i] = input->ti[route[i]]: true; // espera em caso de adiantamento

                /* Duração da Rota */
                int espera = 0;
                espera = max(0,input->ti[route[i]] - tis[i]); // conferir 
                //cout << "espera=" << espera << endl;
                duracao += input->dist[route[i-1]][route[i]] + /*input->vs[route[i-1]].trota +*/ espera;
                /**/

			ant += max(0,input->ti[route[i]]-tis[i]);

			// calculando inicio na solucao
			//if (s[i]==0) { tis[i]=0; } else { tis[i] = input.ts[s[i]] + tis[i-1]; } // considera que s[0]=0
			atr += max(0,tis[i]-input->tf[route[i]]);
		}

		route.pop_back();
		route.erase(route.begin());
		return pair<double,double>(ant,atr);
	}
	double waittime(vector<int> &route) // w(s) (fo)
	{
		double v = wait_delay(route).first;
		if (debug) cout << "Wait:\t\t\t" << v << endl; 
		return v;
	}
	double delay(vector<int> &route) // w(s) (fo)
	{
		double v = wait_delay(route).second;
		if (debug) cout << "Delay:\t\t\t" << v << endl; 
		return v;
	}
	void setDebug(bool b){debug=b;}

	void completeWithZeros(vector< vector<int> > &rep)
	{
		for (int i = 0 ; i < rep.size() ; i++)
		{
			rep[i].insert(rep[i].begin(),0);
			rep[i].push_back(0);
		}
	}

	void removeZeros(vector< vector<int> > &rep)
	{
		for (int i = 0 ; i < rep.size() ; i++)
		{
			rep[i].pop_back();
			rep[i].erase(rep[i].begin());
		}
	}
};



class VRPTWDistanceEvaluator: public VRPTWEvaluator
{
public:

	VRPTWDistanceEvaluator(VRPTWProblemInstance* input):VRPTWEvaluator(input){};
	
	virtual EvaluationVRPTW* evaluate(SolutionVRPTW* s)
	{
		RepVRPTW * rep = s->getRepresentation();

		//completeWithZeros(*rep);

		double distance = 0;
		for (int i = 0 ; i < rep->size() ; i++) if (rep->size() > 2)
			distance += totalTime((*rep)[i]);

		//removeZeros(*rep);

		MemVRPTW* reval = NULL; // Memory for fast Re-evaluation			
		return new Evaluation<MemVRPTW>(distance,reval);
	}
};

class VRPTWExcLoadEvaluator: public VRPTWEvaluator
{
public:

	VRPTWExcLoadEvaluator(VRPTWProblemInstance* input):VRPTWEvaluator(input){};
	
	EvaluationVRPTW* evaluate(SolutionVRPTW* s)
	{
		RepVRPTW * rep = s->getRepresentation();

		//completeWithZeros(*rep);

		double e = 0;
		for (int i = 0 ; i < rep->size() ; i++) if (rep->size() > 2)
			e += excLoad((*rep)[i]);

		//removeZeros(*rep);

		MemVRPTW* reval = NULL; // Memory for fast Re-evaluation			
		return new Evaluation<MemVRPTW>(e*300,reval);
	}
};

class VRPTWDelayEvaluator: public VRPTWEvaluator
{
public:

	VRPTWDelayEvaluator(VRPTWProblemInstance* input):VRPTWEvaluator(input){};
	
	EvaluationVRPTW* evaluate(SolutionVRPTW* s)
	{
		RepVRPTW * rep = s->getRepresentation();

		//completeWithZeros(*rep);

		double d = 0;
		for (int i = 0 ; i < rep->size() ; i++) if (rep->size() > 2)
			d += delay((*rep)[i]);

		//removeZeros(*rep);

		MemVRPTW* reval = NULL; // Memory for fast Re-evaluation			
		return new Evaluation<MemVRPTW>(d*200,reval);
	}
};


#endif /*VRPTW_EVALUATOR_HPP_*/
