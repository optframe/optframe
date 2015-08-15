#ifndef SVRPDSP_CONSTRUCTIVE_Random_HPP_
#define SVRPDSP_CONSTRUCTIVE_Random_HPP_

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

class ConstructiveRandom: public Constructive<RepSVRPDSP, AdsSVRPDSP>
{
private:
	ProblemInstance& pSVRPDSP;

	// Your private vars

public:

	ConstructiveRandom(ProblemInstance& _pSVRPDSP) : // If necessary, add more parameters
			pSVRPDSP(_pSVRPDSP)
	{
	}

	virtual ~ConstructiveRandom()
	{
	}

	Solution<RepSVRPDSP, AdsSVRPDSP>& generateSolution()
	{
		vector<int> d_cust(pSVRPDSP.n);
		for (unsigned int i = 0; i < pSVRPDSP.n; i++)
			d_cust[i] = i + 1;

		vector<int> p_cust(pSVRPDSP.n);
		for (unsigned int i = 0; i < pSVRPDSP.n; i++)
			p_cust[i] = i + 1 + pSVRPDSP.n;

		std::random_shuffle(d_cust.begin(), d_cust.end());
		std::random_shuffle(p_cust.begin(), p_cust.end());

		RepSVRPDSP repAux;

		for (unsigned int i = 0; i < d_cust.size(); i++)
			repAux.push_back(d_cust[i]);

		//unsigned nPickup = rand()%p_cust.size();
		unsigned nPickup = p_cust.size();

		for (unsigned int i = 0; i < nPickup; i++)
			repAux.push_back(p_cust[i]);

		RepSVRPDSP rep;
		rep.push_back(0); // start depot

		std::random_shuffle(repAux.begin(), repAux.end());

		for (unsigned i = 0; i < repAux.size(); i++)
			rep.push_back(repAux[i]);

		rep.push_back(0); // back to depot

		for (unsigned i = nPickup; i < p_cust.size(); i++)
			rep.push_back(p_cust[i]);

		// USING THIS!

		RepSVRPDSP repAux2;
		repAux2.push_back(0);

		for (unsigned int i = 0; i < d_cust.size(); i++)
			repAux2.push_back(d_cust[i]);

		repAux2.push_back(0);

		for (unsigned int i = 0; i < p_cust.size(); i++)
			repAux2.push_back(p_cust[i]);

		MySolution* s = new MySolution(repAux2);

		if (!s->syncADS(pSVRPDSP))
		{
			cout << "error syncronizing ADS (Const. Random)" << endl;
			cout << "rep: " << s->getR() << endl;
			exit(1);
		}

		return *s;
	}

	void print() const
	{
		cout << "ConstructiveRandom" << endl;
	}

};

}

#endif /*SVRPDSP_CONTRUCTIVE_Random_HPP_*/
