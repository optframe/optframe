#ifndef EFP_METHODPARAMETERS_HPP_
#define EFP_METHODPARAMETERS_HPP_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

using namespace std;
using namespace scannerpp;

namespace EFP
{

class methodParameters
{
private:
	int esMaxG, esMU, esLambda, esInitialDesv, esMutationDesv;
	int construtiveMethod, construtivePrecision;
	vector<double> vAlphaACFlimits;
	int evalFOMinimizer; //Index of which fo metric will be used
	int evalAprox; //Aprox

public:
	methodParameters()
	{
		esMaxG = -1;
		esMU = -1;
		esLambda = -1;
		esInitialDesv = -1;
		esMutationDesv = -1;
		construtiveMethod = -1;
		construtivePrecision = -1;
		evalFOMinimizer = -1; //Index of which fo metric will be used
		evalAprox = -1; //Aprox

		vAlphaACFlimits.push_back(-1);
	}

	void setESMaxG(int _esMaxG)
	{
		esMaxG = _esMaxG;
	}

	int getESMaxG()
	{
		return esMaxG;
	}

	void setESMU(int _esMU)
	{
		esMU = _esMU;
	}

	int getESMU()
	{
		return esMU;
	}

	void setESLambda(int _esLambda)
	{
		esLambda = _esLambda;
	}

	int getESLambda()
	{
		return esLambda;
	}

	void setESInitialDesv(int _esInitialDesv)
	{
		esInitialDesv = _esInitialDesv;
	}

	int getESInitialDesv()
	{
		return esInitialDesv;
	}

	void setESMutationDesv(int _esMutationDesv)
	{
		esMutationDesv = _esMutationDesv;
	}

	int getESMutationDesv()
	{
		return esMutationDesv;
	}

	void setConstrutiveMethod(int _construtiveMethod)
	{
		construtiveMethod = _construtiveMethod;
	}

	int getConstrutiveMethod()
	{
		return construtiveMethod;
	}

	void setConstrutivePrecision(int _construtivePrecision)
	{
		construtivePrecision = _construtivePrecision;
	}

	int getConstrutivePrecision()
	{
		return construtivePrecision;
	}

	void setEvalFOMinimizer(int _evalFOMinimizer)
	{
		evalFOMinimizer = _evalFOMinimizer;
	}

	int getEvalFOMinimizer()
	{
		return evalFOMinimizer;
	}

	void setEvalAprox(int _evalAprox)
	{
		evalAprox = _evalAprox;
	}

	int getEvalAprox()
	{
		return evalAprox;
	}

	void setConstrutiveLimitAlphaACF(vector<double> _vAlphaACFlimits)
	{
		vAlphaACFlimits = _vAlphaACFlimits;
	}

	vector<double> getConstrutiveLimitAlphaACF()
	{
		return vAlphaACFlimits;
	}

	/*
	 void setX(int _X)
	 {
	 X = _X;
	 }

	 int getX()
	 {
	 return X;
	 }*/

};

}

#endif /*EFP_METHODPARAMETERS_HPP_*/

