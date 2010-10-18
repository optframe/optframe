#ifndef VRPTW_INITIALSOLUTION_CheapestInsertion_HPP_
#define VRPTW_INITIALSOLUTION_CheapestInsertion_HPP_

#include "../../InitialSolution.h"
#include "../../Solution/TestSolution.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>

#include <values.h>

using namespace std;

class CheapestInsertionInitialSolutionVRPTW: public InitialSolution<RepVRPTW>
{
private:
   VRPTWProblemInstance* input;
	bool debug;

   // Your private vars

public:
	
	CheapestInsertionInitialSolutionVRPTW(VRPTWProblemInstance* input) // If necessary, add more parameters
        {
           this->input = input;
           // Put the rest of your code here
		debug = false;
	};

	// solucao
	typedef vector<int> S;


	SolutionVRPTW* generateSolution()
	{

		S* s = new S(input->v+1,0);

		//cout << *s << "\n";

		VRPTWEvaluator eval(input); eval.setDebug(false);

		for ( int c = 1 ; c < input->n ; c++ )
		{
			int besti=-1;
			double beste=INT_MAX;

			for ( int i = 1 ; i < s->size()-1 ; i++ )
			{
				s->insert(s->begin()+i,c);
				vector<vector<int> > *rep = converte_vi_vvi(*s);
				pair<double,bool> e = eval.eval(*rep);
				delete rep;
				s->erase(s->begin()+i);
				if (e.first < beste)
				{
					//if (e.second) {besti = i; beste = e.first;} // Só aceita soluções viáveis
					besti = i; beste = e.first; // aceita soluções inviáveis
				}
			}

			if (besti==-1) { cout << "AVISO (CheapestInsertionInitialSolution::generateSolutionVRPTW()): todas inserções inviáveis"<< endl; exit(1); }

			s->insert(s->begin()+besti,c);
			//cout << *s << "\n";
		}

//		// conversão [int] -> [[int]] TODO
		vector< vector<int> > * rep = converte_vi_vvi(*s);
//		SolVRPTW * solution = new SolVRPTW(rep);
//		cout << *s << "\n\n";
		delete s;
//		return new SolVRPTW(rep);

		for (int i = 0 ; i < rep->size() ; i++)
		{
			(*rep)[i].pop_back();
			(*rep)[i].erase((*rep)[i].begin());
		}

	   return new TestSolution<RepVRPTW>(rep);			
	}

	vector< vector<int> > * converte_vi_vvi(vector<int> &v)
	{
		vector<int>::iterator it;
		vector<int> r;
		vector< vector<int> > * rs = new vector< vector<int> >;
		for (it = v.begin()+1; it != v.end(); it++)
		{
			if (*it!=0)
			{
				r.push_back(*it);
			}
			else
			{
				r.insert(r.begin(),0); // colocando 0's no início de cada vetor
				r.push_back(0);        // colocando 0's no fim de cada vetor

				rs->push_back(r);
				r.clear();
			}
		}
		return rs;
	};

	vector<int> * converte_vvi_vi(vector< vector<int> > &vs)
	{
		vector<int> * r = new vector<int>;
		for (int i = 0 ; i < vs.size() ; i++)
		{
			r->push_back(0);
			for (int j = 0 ; j < vs[i].size() ; j++)
			{
				r->push_back(vs[i][j]);
			}
		}
		r->push_back(0);
		return r;
	};

	void setDebug(bool d){debug=d;};

	
};






class CheapestInsertionSeedInitialSolutionVRPTW: public InitialSolution<RepVRPTW>
{
private:
   VRPTWProblemInstance* input;
	bool debug;

   // Your private vars

public:

	CheapestInsertionSeedInitialSolutionVRPTW(VRPTWProblemInstance* input) // If necessary, add more parameters
        {
           this->input = input;
           // Put the rest of your code here
		debug = false;
	};

	// solucao
	typedef vector<int> S;


	SolutionVRPTW* generateSolution()
	{

		S* s = new S(input->v+1,0);

		//cout << *s << "\n";

		VRPTWEvaluator eval(input); eval.setDebug(false);

		vector<int> LC;
		for ( int c = 1 ; c < input->n ; c++ ) LC.push_back(c);

		/*Seed*/ // Se não quiser inserir semente em cada rota basta comentar este trecho
		for ( int v = input->v ; v > 0 ; v-- )
		{
			int rClient = rand() % LC.size();
			s->insert(s->begin()+v,LC[rClient]);
			LC.erase(LC.begin()+rClient);
		}
		/**/

		while (!LC.empty())
		{
			int beste=INT_MAX,besti=-1,bestpc=1;

			//vector<int>::iterator it;
			for ( int pc = 0 ; pc < LC.size() ; pc++ )
			{
				for ( int i = 1 ; i < s->size()-1 ; i++ )
				{
					s->insert(s->begin()+i,LC[pc]);
					vector<vector<int> > *rep = converte_vi_vvi(*s);
					pair<double,bool> e = eval.eval(*rep);
					delete rep;
					s->erase(s->begin()+i);
					if (e.first < beste)
					{
						//if (e.second) {besti = i; beste = e.first; bestpc=pc;} // Só aceita soluções viáveis
						besti = i; beste = e.first; bestpc=pc; // aceita soluções inviáveis
					}
				}
			}

			if (besti==-1) { cout << "AVISO (CheapestInsertionSeedInitialSolution::generateSolutionVRPTW()): todas inserções inviáveis"<< endl; exit(1); }

			s->insert(s->begin()+besti,LC[bestpc]);
			//cout << *s << "\n";
			LC.erase(LC.begin()+bestpc);
		}

//		// conversão [int] -> [[int]] TODO
		vector< vector<int> > * rep = converte_vi_vvi(*s);
//		SolVRPTW * solution = new SolVRPTW(rep);
//		cout << *s << "\n\n";
		delete s;
//		return new SolVRPTW(rep);

		for (int i = 0 ; i < rep->size() ; i++)
		{
			(*rep)[i].pop_back();
			(*rep)[i].erase((*rep)[i].begin());
		}

	   return new TestSolution<RepVRPTW>(rep);
	}

	vector< vector<int> > * converte_vi_vvi(vector<int> &v)
	{
		vector<int>::iterator it;
		vector<int> r;
		vector< vector<int> > * rs = new vector< vector<int> >;
		for (it = v.begin()+1; it != v.end(); it++)
		{
			if (*it!=0)
			{
				r.push_back(*it);
			}
			else
			{
				r.insert(r.begin(),0); // colocando 0's no início de cada vetor
				r.push_back(0);        // colocando 0's no fim de cada vetor

				rs->push_back(r);
				r.clear();
			}
		}
		return rs;
	};

	vector<int> * converte_vvi_vi(vector< vector<int> > &vs)
	{
		vector<int> * r = new vector<int>;
		for (int i = 0 ; i < vs.size() ; i++)
		{
			r->push_back(0);
			for (int j = 0 ; j < vs[i].size() ; j++)
			{
				r->push_back(vs[i][j]);
			}
		}
		r->push_back(0);
		return r;
	};

	void setDebug(bool d){debug=d;};


};


#endif /*VRPTW_INITIALSOLUTION_CheapestInsertion_HPP_*/
