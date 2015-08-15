#ifndef SVRPDSP_CONSTRUCTIVE_GRandTSPOptimalGLO_HPP_
#define SVRPDSP_CONSTRUCTIVE_GRandTSPOptimalGLO_HPP_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "ADS.h"
#include "MySolution.hpp"

#include "Evaluator.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

namespace SVRPDSP
{

class ConstructiveGRandTSPOptimalGLO: public Constructive<RepSVRPDSP, AdsSVRPDSP>
{
private:
	ProblemInstance& pSVRPDSP;

	string str_tsp;
	string str_knapsack;
	double alpha;
	int option;

public:

	static bool my_sort(pair<pair<int, int> , double> p1, pair<pair<int, int> , double> p2)
	{
		return p1.second < p2.second;
	}


	ConstructiveGRandTSPOptimalGLO(ProblemInstance& _pSVRPDSP, Scanner& _tsp, Scanner& _knapsack, int _option = 0, double _alpha = 1) :
		pSVRPDSP(_pSVRPDSP), str_tsp(_tsp.rest()), str_knapsack(_knapsack.rest()), option(_option)
	{
		alpha = _alpha;

		if (alpha < 0.0001)
			alpha = 0.0001;
		if (alpha > 1)
			alpha = 1;
	}

	virtual ~ConstructiveGRandTSPOptimalGLO()
	{
	}

	Solution<RepSVRPDSP, AdsSVRPDSP>& generateSolution()
	{
		Scanner tsp(str_tsp);
		Scanner knapsack(str_knapsack);

		RepSVRPDSP rep;

		rep.push_back(0); // depot

		// Read TSP

		int n_d = tsp.nextInt() - 1;
		tsp.nextInt(); // drop depot

		vector<bool> missing(pSVRPDSP.n * 2 + 1, true);

		for (int i = 0; i < n_d; i++)
		{
			int d = tsp.nextInt();

			rep.push_back(d);
			missing[d] = false;
		}

		rep.push_back(0); // depot
		missing[0] = false;

		// Read Knapsack

		int n_k = knapsack.nextInt();
		vector<bool> knp(n_k, false);

		for (int i = 0; i < n_k; i++)
			if (knapsack.nextInt())
				knp[i] = true;

		// greedy randomized step

		vector<pair<pair<int, int> , double> > cList;

		do
		{
			cList.clear();

			for (int c = 1; c < knp.size(); c++)
				if (knp[c]) // available candidate
				{
					double q = pSVRPDSP.Q;

					for (int i = 0; i < rep.size() - 1; i++)
					{
						int rep_i = rep[i];
						if (rep_i > pSVRPDSP.n)
							rep_i -= pSVRPDSP.n;

						int rep_ii = rep[i + 1];
						if (rep_ii > pSVRPDSP.n)
							rep_ii -= pSVRPDSP.n;

						// check 'q'

						bool q_ok = true;

						for (int j = 0; j <= i; j++)
							if (rep[j] > pSVRPDSP.n) // pickup
								q += pSVRPDSP.p[rep[j] - pSVRPDSP.n];
							else
								q -= pSVRPDSP.d[rep[j]];

						q += pSVRPDSP.p[c];

						if (q <= pSVRPDSP.Q)
							for (int j = i + 1; j < rep.size() - 1; j++)
								if (rep[j] > pSVRPDSP.n) // pickup
								{
									q += pSVRPDSP.p[rep[j] - pSVRPDSP.n];
									if (q > pSVRPDSP.Q)
									{
										q_ok = false;
										break;
									}
								}
								else
									q -= pSVRPDSP.d[rep[j]];
						else
							q_ok = false;

						if (q_ok) // pickup ok
							switch (option)
							{
							case 0: // distance
							{
								double d1 = pSVRPDSP.c(rep_i, c);
								double d2 = pSVRPDSP.c(c, rep_ii);
								cList.push_back(make_pair(make_pair(c, i), d1 + d2));
								break;
							}
							case 1: // -revenue
							{
								double r = pSVRPDSP.r[c];
								cList.push_back(make_pair(make_pair(c, i), -r));
								break;
							}
							case 2: // distance - revenue
							{
								double d1 = pSVRPDSP.c(rep_i, c);
								double d2 = pSVRPDSP.c(c, rep_ii);
								double r = pSVRPDSP.r[c];
								cList.push_back(make_pair(make_pair(c, i), d1 + d2 - r));
								break;
							}
							case 3: // pickup (best are smaller)
							{
								double p = pSVRPDSP.p[c];
								cList.push_back(make_pair(make_pair(c, i), p));
								break;
							}
							case 4: // -pickup (best are bigger)
							{
								double p = pSVRPDSP.p[c];
								cList.push_back(make_pair(make_pair(c, i), -p));
								break;
							}
							case 5: // balance dist and pickup | (DIST - REV) / ( (cap - A) - PICKUP) | best are smaller pickups
							{
								double d1 = pSVRPDSP.c(rep_i, c);
								double d2 = pSVRPDSP.c(c, rep_ii);
								double r = pSVRPDSP.r[c];
								double p = pSVRPDSP.p[c];

								cList.push_back(make_pair(make_pair(c, i), (d1 + d2 - r) / ((pSVRPDSP.Q - q) - p)));
								break;
							}
							case 6: // balance dist and pickup | (DIST - REV) / (PICKUP) | best are bigger pickups
							{
								double d1 = pSVRPDSP.c(rep_i, c);
								double d2 = pSVRPDSP.c(c, rep_ii);
								double r = pSVRPDSP.r[c];
								double p = pSVRPDSP.p[c];

								cList.push_back(make_pair(make_pair(c, i), (d1 + d2 - r) / p));
								break;
							}
							default:
								cout << "no option " << option << "!" << endl;
								exit(1);
							}
					}
				}

			sort(cList.begin(), cList.end(), my_sort);

			if (cList.size() > 0)
			{
				int rcl = (int) ceil(cList.size() * alpha);
				int x = rand() % rcl;

				int c = cList[x].first.first + pSVRPDSP.n;
				int i = cList[x].first.second;

				rep.insert(rep.begin() + i + 1, c);
				missing[c] = false;
				knp[c - pSVRPDSP.n] = false;
			}

		} while (cList.size() > 0);

		for (int i = 0; i < missing.size(); i++)
			if (missing[i])
				rep.push_back(i);

      MySolution* s = new MySolution(rep);

      if(!s->syncADS(pSVRPDSP))
      {
         cout << "error syncronizing ADS (Const. GLO)" << endl;
         cout << "rep: " << rep << endl;
         exit(1);
      }

      return *s;
	}

};

}

#endif /*SVRPDSP_CONTRUCTIVE_GRandTSPOptimalGLO_HPP_*/
