#ifndef SVRPDSP_CONSTRUCTIVE_GRandTSPOptimalSEQ_HPP_
#define SVRPDSP_CONSTRUCTIVE_GRandTSPOptimalSEQ_HPP_

#include "../../OptFrame/Constructive.h"
#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "MySolution.hpp"

#include "Evaluator.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

namespace SVRPDSP
{

class ConstructiveGRandTSPOptimalSEQ: public Constructive<RepSVRPDSP, AdsSVRPDSP>
{
private:
	ProblemInstance& pSVRPDSP;
	string str_tsp;
	string str_knapsack;
	double alpha;
	int option;


public:

	static bool my_sort(pair<int, double> p1, pair<int, double> p2)
	{
		return p1.second < p2.second;
	}

	ConstructiveGRandTSPOptimalSEQ(ProblemInstance& _pSVRPDSP, Scanner& _tsp, Scanner& _knapsack, int _option = 0, double _alpha = 1) :
		pSVRPDSP(_pSVRPDSP), str_tsp(_tsp.rest()), str_knapsack(_knapsack.rest()), option(_option)
	{
		alpha = _alpha;

		if (alpha < 0.0001)
			alpha = 0.0001;
		if (alpha > 1)
			alpha = 1;
	}

	virtual ~ConstructiveGRandTSPOptimalSEQ()
	{
	}

	Solution<RepSVRPDSP, AdsSVRPDSP>& generateSolution()
	{
		Scanner tsp(str_tsp);
		Scanner knapsack(str_knapsack);

		RepSVRPDSP rep;

		rep.push_back(0); // depot

		int n_k = knapsack.nextInt();
		vector<bool> knp(n_k, false);

		for (int i = 0; i < n_k; i++)
			if (knapsack.nextInt())
				knp[i] = true;

		int n_d = tsp.nextInt() - 1;
		tsp.nextInt(); // drop depot

		double q = pSVRPDSP.Q;

		vector<bool> missing(pSVRPDSP.n * 2 + 1, true);

		for (int i = 0; i < n_d; i++)
		{
			int d = tsp.nextInt();

			rep.push_back(d);
			missing[d] = false;

			q -= pSVRPDSP.d[d];

			// greedy randomized step

			vector<pair<int, double> > cList;

			for (int c = 1; c < knp.size(); c++)
				if (knp[c]) // available candidate
					if ((q + pSVRPDSP.p[c]) <= pSVRPDSP.Q) // pickup ok
						switch (option)
						{
						case 0: // distance
							cList.push_back(make_pair(c, pSVRPDSP.c(d, c)));
							break;
						case 1: // -revenue
							cList.push_back(make_pair(c, -pSVRPDSP.r[c]));
							break;
						case 2: // distance - revenue
							cList.push_back(make_pair(c, pSVRPDSP.c(d, c) - pSVRPDSP.r[c]));
							break;
						case 3: // pickup (best are smaller)
							cList.push_back(make_pair(c, pSVRPDSP.p[c]));
							break;
						case 4: // -pickup (best are bigger)
							cList.push_back(make_pair(c, pSVRPDSP.p[c]));
							break;
						case 5: // balance dist and pickup | (DIST - REV) / ( (cap - A) - PICKUP) | best are smaller pickups
							cList.push_back(make_pair(c, (pSVRPDSP.c(d, c) - pSVRPDSP.r[c]) / ((pSVRPDSP.Q - q) - pSVRPDSP.p[c])));
							break;
						case 6: // balance dist and pickup | (DIST - REV) / (PICKUP) | best are bigger pickups
							cList.push_back(make_pair(c, (pSVRPDSP.c(d, c) - pSVRPDSP.r[c]) / pSVRPDSP.p[c]));
							break;
						default:
							cout << "no option " << option << "!" << endl;
							exit(1);
						}

			sort(cList.begin(), cList.end(), my_sort);

			if (cList.size() > 0)
			{
				int rcl = (int) ceil(cList.size() * alpha);
				int x = rand() % rcl;

				rep.push_back(cList[x].first + pSVRPDSP.n);
				missing[cList[x].first + pSVRPDSP.n] = false;
				knp[cList[x].first] = false;
				q += pSVRPDSP.p[cList[x].first];
			}
		}

		rep.push_back(0); // depot
		missing[0] = false;

		for (int i = 0; i < missing.size(); i++)
			if (missing[i])
				rep.push_back(i);

      MySolution* s = new MySolution(rep);

      if(!s->syncADS(pSVRPDSP))
      {
         cout << "error syncronizing ADS (Const. SEQ)" << endl;
         cout << "rep: " << rep << endl;
         exit(1);
      }

      return *s;
	}

};

}

#endif /*SVRPDSP_CONTRUCTIVE_GRandTSPOptimalSEQ_HPP_*/
