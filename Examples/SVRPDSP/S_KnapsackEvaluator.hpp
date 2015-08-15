#ifndef SVRPDSP_KNP_EVALUATOR_HPP_
#define SVRPDSP_KNP_EVALUATOR_HPP_

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

#define EPSILON_S_KNP 0.0001

namespace SVRPDSP
{

class S_KNPEvaluator: public Evaluator<RepSVRPDSP, AdsSVRPDSP>
{
private:
	ProblemInstance& pSVRPDSP;

public:

	using Evaluator<RepSVRPDSP, AdsSVRPDSP>::evaluate;

	S_KNPEvaluator(ProblemInstance& _pSVRPDSP) : // If necessary, add more parameters
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

	Evaluation& evaluate(const RepSVRPDSP& rep, const AdsSVRPDSP& ads)
	{
#ifdef  ALL_INTEGER
                int INF_CAP_WEIGHT = 100000;
                int INF_NO_DELIVERY_WEIGHT = 100000*100;

		long long fo = 0;
		long long fo_inf = 0;

		long long fo_r = 0;

		long long sum_q = 0;
#endif
#ifndef ALL_INTEGER
                double INF_CAP_WEIGHT = 100000;
                double INF_NO_DELIVERY_WEIGHT = 100000;

		double fo = 0;
		double fo_inf = 0;

		double fo_r = 0;

		double sum_q = 0;
#endif

		if (rep.size() == 0)
		{
			cout << "Evaluator Error: rep.size() == 0!" << endl;
			exit(1);
		}

		for (int i = 1; i < rep.size(); i++)
			if (rep[i] == 0)
				break;
			else if (rep[i] > pSVRPDSP.n)
			{
				int p_cust = rep[i] - pSVRPDSP.n;
                //cout << "PICKUP OF " << p_cust << endl;
                //getchar();

				sum_q += pSVRPDSP.p[p_cust];

				fo_r += pSVRPDSP.r[p_cust];
			}

		if (sum_q > (pSVRPDSP.Q + 0.0001)) // epsilon
			fo_inf += (sum_q - pSVRPDSP.Q) * INF_CAP_WEIGHT; // INFEASIBLE!

		fo = fo_r;

		return *new Evaluation(fo, fo_inf);
	}

	virtual bool betterThan(double a, double b)
	{
		// MINIMIZATION
		return (a < (b - EPSILON_S_KNP));
	}

	virtual bool isMinimization() const
	{
		return true;
	}
};

}

#endif /*SVRPDSP_KNP_EVALUATOR_HPP_*/
