#ifndef SVRPDSP_TSP_EVALUATOR_HPP_
#define SVRPDSP_TSP_EVALUATOR_HPP_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include "../../OptFrame/Evaluation.hpp"
#include "../../OptFrame/Evaluator.hpp"

#include "Representation.h"
#include "ADS.h"
#include "Delta.h"
#include "MySolution.hpp"

#include "ProblemInstance.hpp"

#define EPSILON_S_TSP 0.0001

namespace SVRPDSP
{

class S_TSPEvaluator: public Evaluator<RepSVRPDSP, AdsSVRPDSP>
{
private:
	ProblemInstance& pSVRPDSP;

public:

	using Evaluator<RepSVRPDSP, AdsSVRPDSP>::evaluate;

	S_TSPEvaluator(ProblemInstance& _pSVRPDSP) : // If necessary, add more parameters
		pSVRPDSP(_pSVRPDSP)
	{
	}

	Evaluation& evaluate(const RepSVRPDSP& rep)
	{
		cout << "DEPRECATED:DO NOT USE THIS METHOD!" << endl;
		exit(1);
		const AdsSVRPDSP ads1;
		return evaluate(rep, ads1);
	}

	Evaluation& evaluate(const RepSVRPDSP& rep2, const AdsSVRPDSP& ads)
	{
#ifdef  ALL_INTEGER
                int INF_CAP_WEIGHT = 100000;
                int INF_NO_DELIVERY_WEIGHT = 100000*100;

		long long fo = 0;
		long long fo_inf = 0;

		long long fo_c = 0;
#endif
#ifndef ALL_INTEGER
                double INF_CAP_WEIGHT = 100000;
                double INF_NO_DELIVERY_WEIGHT = 100000;

		double fo = 0;
		double fo_inf = 0;

		double fo_c = 0;
#endif

		if (rep2.size() == 0)
		{
			cout << "Evaluator Error: rep.size() == 0!" << endl;
			exit(1);
		}

		vector<int> dlry_rep = rep2;

		/*
		 for (int i = 0; i < rep2.size(); i++)
		 if (rep2[i] <= pSVRPDSP.n)
		 dlry_rep.push_back(rep2[i]);
		 */

		int num_d_cust = 0;

		// depot to first customer
		if (dlry_rep.at(1) <= pSVRPDSP.n)
			fo_c = pSVRPDSP.c(0, dlry_rep.at(1));
		else
			fo_c = pSVRPDSP.c(0, dlry_rep.at(1) - pSVRPDSP.n);

		for (int i = 1; i < dlry_rep.size(); i++)
			if (dlry_rep[i] == 0)
				break;
			else if (dlry_rep[i] <= pSVRPDSP.n)
			{
				// delivery customer
				int d_cust = dlry_rep[i];

				num_d_cust++;

				int next = dlry_rep[i + 1];
				if (next > pSVRPDSP.n)
					next -= pSVRPDSP.n;

				fo_c += pSVRPDSP.c(d_cust, next);
			}
			else
			{
				// pickup customer
				int p_cust = dlry_rep[i] - pSVRPDSP.n;

				int next = dlry_rep[i + 1];
				if (next > pSVRPDSP.n)
					next -= pSVRPDSP.n;

				fo_c += pSVRPDSP.c(p_cust, next);

			}

		if (num_d_cust < pSVRPDSP.n)
        {
			fo_inf += (pSVRPDSP.n - num_d_cust) * INF_NO_DELIVERY_WEIGHT;
            cout << "MISSING DELIVERY CUSTOMERS! " << num_d_cust << "<" << pSVRPDSP.n << endl;
            exit(1);
        }

		fo = fo_c;

		return *new Evaluation(fo, fo_inf);
	}

	virtual bool betterThan(double a, double b)
	{
		// MINIMIZATION
		return (a < (b - EPSILON_S_TSP));
	}

	virtual bool isMinimization() const
	{
		return true;
	}

};

}

#endif /*SVRPDSP_TSP_EVALUATOR_HPP_*/
