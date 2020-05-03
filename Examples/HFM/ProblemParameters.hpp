#ifndef HFM_PROBLEMPARAMETERS_HPP_
#define HFM_PROBLEMPARAMETERS_HPP_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

using namespace std;
using namespace scannerpp;

namespace HFM
{

struct ForecastingOptions
{
	bool roundNegative;
	bool round;
	bool binary;

	ForecastingOptions(bool _roundNegative = false, bool _round = false, bool _binary = false) :
			roundNegative(_roundNegative), round(_round), binary(_binary)
	{
	}

	friend ostream & operator<<(ostream & os, const ForecastingOptions& fOptions)
	{

		os << "=======================================" << endl;
		os << "==========PRINTING PATH ===============" << endl;
		os << "=======================================" << endl;
		os << " allowNegative:" << fOptions.roundNegative << " ; round:" << fOptions.round << " ; binary:" << fOptions.binary << endl;
		os << "=======================================" << endl;

		return os;
	}

};

class ProblemParameters
{
private:
	// Your private vars
	//int options;
	int precision;
	int stepsAhead;
	int function;
	vector<int> vNotUsedForTests;
	//If maxUpperLag is greater than 0 model uses predicted data: ( t - (-K) ) => (t + K) | K !=0
	vector<int> vMaxLag, vMaxUpperLag;
	string instancePath;
	string validationPath;
	int nFiles;
	ForecastingOptions fOptions;
	int targetFile;
//
//	vector<pair<int, int> > oForesSP; //Options to Forecasting with single points
//	vector<pair<int, vector<int> > > oForesMP; //Options to Forecasting with mean points
//	vector<pair<int, vector<int> > > oForesDP; //Options to Forecasting with derivate points
	vector<bool> forceSampleLearningWithEndogenous;

public:
//	ProblemParameters(string parametersFile)
//	{
//		readFile(parametersFile);
//		targetFile = 0;
//		vMaxLag.resize(1,0);
//		vMaxUpperLag.resize(1,0);
//	}

	ProblemParameters(int _nExplanationVariables)
	{
		targetFile = 0;
		vMaxLag.resize(_nExplanationVariables, 0);
		vMaxUpperLag.resize(_nExplanationVariables, 0);
		forceSampleLearningWithEndogenous.resize(_nExplanationVariables, false);
	}

	ProblemParameters()
	{
		targetFile = 0;
		vMaxLag.resize(1, 0);
		vMaxUpperLag.resize(1, 0);
	}

	//the target file is always the first time series
	int getTargetFile()
	{
		return targetFile;
	}

	virtual ~ProblemParameters()
	{
	}

	int getStepsAhead()
	{
		return stepsAhead;
	}

	vector<int> getVMaxLag()
	{
		return vMaxLag;
	}

	vector<int> getVMaxUpperLag()
	{
		return vMaxUpperLag;
	}

	//If  no ExpVar to be set is passed, then, all ExpVar receives the provided maxLagUpperBound (t - (-K))
	void setMaxUpperLag(int _maxLagUpperBound, int _expVariable = -1)
	{
		if (_expVariable == -1)
		{
			for (int l = 0; l < (int) vMaxUpperLag.size(); l++)
				vMaxUpperLag[_expVariable] = _maxLagUpperBound;
		}
		else
		{
			assert(_expVariable < (int) vMaxUpperLag.size());
			vMaxUpperLag.at(_expVariable) = _maxLagUpperBound;
		}
	}

	//If  no ExpVar to be set is passed, then, all ExpVar receives the provided maxLag (t - K)
	void setMaxLag(int _maxLag, int _expVariable = -1)
	{
		if (_expVariable == -1)
		{
			for (int l = 0; l < (int) vMaxLag.size(); l++)
				vMaxLag[_expVariable] = _maxLag;
		}
		else
		{
			assert(_expVariable < (int) vMaxLag.size());
			vMaxLag.at(_expVariable) = _maxLag;
		}
	}

	//This do not contain -1 in order to force the request of specific maxUpperLag
	int getMaxUpperLag(int _expVariable)
	{
		assert((int) vMaxUpperLag.size() > _expVariable);
		return vMaxUpperLag.at(_expVariable);
	}

	int getMaxLag(int _expVariable)
	{
		assert(_expVariable < (int) vMaxLag.size());
		return vMaxLag[_expVariable];
	}


	bool getForceSampleLearningWithEndogenous(int _expVariable)
	{
		assert(_expVariable != getTargetFile());
		assert((int) forceSampleLearningWithEndogenous.size() >= _expVariable);
		return forceSampleLearningWithEndogenous[_expVariable];
	}

	void setForceSampleLearningWithEndogenous(bool _forcing, int _expVariable = -1)
	{
		if (_expVariable == -1)
		{
			for (int v = 1; v < (int) forceSampleLearningWithEndogenous.size(); v++)
				forceSampleLearningWithEndogenous[v] = _forcing;
		}
		else
		{
			assert(_expVariable < (int) forceSampleLearningWithEndogenous.size());
			forceSampleLearningWithEndogenous[_expVariable] = _forcing;
		}
	}

	bool getRoundedForecastsStatus()
	{
		return fOptions.round;
	}

	bool getNonNegativeForecastsStatus()
	{
		return fOptions.roundNegative;
	}

	bool getBinaryForecastsStatus()
	{
		return fOptions.binary;
	}

	void setToBinaryForecasts(bool _desiredRounding)
	{
		fOptions.binary = _desiredRounding;
	}

	void setToRoundedForecasts(bool _desiredRounding)
	{
		fOptions.round = _desiredRounding;
	}

	void setToNonNegativeForecasts(bool _desiredRounding)
	{
		fOptions.roundNegative = _desiredRounding;
	}



	void setStepsAhead(int _stepsAhead)
	{
		stepsAhead = _stepsAhead;
	}

	void setInstancePath(string _instancePath)
	{
		instancePath = _instancePath;
	}

	void setValidationPath(string _validationPath)
	{
		validationPath = _validationPath;
	}

	string getInstancePath()
	{
		return instancePath;
	}

	string getValidationPath()
	{
		return validationPath;
	}

//	vector<pair<int, int> > getOForesSP()
//	{
//		return oForesSP;
//	}
//
//	vector<pair<int, vector<int> > > getOForesMP()
//	{
//		return oForesMP;
//	}
//
//	vector<pair<int, vector<int> > > getOForesDP()
//	{
//		return oForesDP;
//	}
//TODO DEPRECATED
//	void readFile(string parametersFile)
//	{
//		vNotUsedForTests.clear();
//		oForesSP.clear();
//		oForesMP.clear();
//		oForesDP.clear();
//		File* file;
//		try
//		{
//			file = new File(parametersFile);
//		} catch (FileNotFound& f)
//		{
//			cout << "Parameters File '" << parametersFile << "' not found" << endl;
//			exit(1);
//		}
//		Scanner scanner(file);
//
//		cout << scanner.nextLine() << endl;
//		cout << scanner.nextLine() << endl;
//		precision = scanner.nextInt();
//		cout << "precision = " << precision << endl;
//		cout << scanner.nextLine() << endl;
//		stepsAhead = scanner.nextInt();
//		cout << "stepsAhead = " << stepsAhead << endl;
//		cout << scanner.nextLine() << endl;
//		function = scanner.nextInt();
//		cout << "function = " << function << endl;
//
//		cout << scanner.nextLine() << endl;
//		cout << scanner.nextLine() << endl;
//		cout << scanner.nextLine() << endl;
//
//		int nSP = scanner.nextInt();
//		cout << "nSP = " << nSP << endl;
//
//		cout << scanner.nextLine() << endl;
//
//		for (int i = 0; i < nSP; i++)
//		{
//			int file = scanner.nextInt();
//			int K = scanner.nextInt();
//			oForesSP.push_back(make_pair(file, K));
//		}
//
//		cout << oForesSP << endl;
//
//		cout << scanner.nextLine() << endl;
//		cout << scanner.nextLine() << endl;
//
//		int nMP = scanner.nextInt();
//		cout << "nMP = " << nMP << endl;
//
//		cout << scanner.nextLine() << endl;
//
//		for (int i = 0; i < nMP; i++)
//		{
//			int file = scanner.nextInt();
//			int nAverageP = scanner.nextInt();
//
//			vector<int> meansP;
//			for (int n = 0; n < nAverageP; n++)
//			{
//				int K = scanner.nextInt();
//				meansP.push_back(K);
//			}
//			oForesMP.push_back(make_pair(file, meansP));
//		}
//
//		cout << oForesMP << endl;
//
//		cout << scanner.nextLine() << endl;
//		cout << scanner.nextLine() << endl;
//
//		int nDP = scanner.nextInt();
//		cout << "nDP = " << nDP << endl;
//
//		cout << scanner.nextLine() << endl;
//
//		for (int i = 0; i < nDP; i++)
//		{
//			int file = scanner.nextInt();
//			int K1 = scanner.nextInt();
//			int K2 = scanner.nextInt();
//
//			vector<int> derivativeP;
//			derivativeP.push_back(K1);
//			derivativeP.push_back(K2);
//
//			oForesDP.push_back(make_pair(file, derivativeP));
//		}
//
//		cout << oForesDP << endl;
//
//		cout << scanner.nextLine() << endl;
//		cout << scanner.nextLine() << endl;
//
//		//cout << "===== options = nSP + nMP + nDP =====" << endl;
//		//options = nSP + nMP + nDP;
//		//cout << "options = " << options << endl;
//
//		cout << scanner.nextLine() << endl;
//		cout << scanner.nextLine() << endl;
//
//		//number of Single Points + Average Points + Derivative and Integral Ones
//
//		//Checking number of files
//		//Checking the minimum number of points to perform a forecast #notUsedForTest
//		int oldestStep = 0;
//		nFiles = 0;
//
//		for (int i = 0; i < (int) oForesSP.size(); i++)
//		{
//			if (oForesSP[i].first > nFiles)
//				nFiles = oForesSP[i].first;
//
//			if (oForesSP[i].second > oldestStep)
//				oldestStep = oForesSP[i].second;
//		}
//
//		for (int i = 0; i < (int) oForesMP.size(); i++)
//		{
//			vector<int> means = oForesMP[i].second;
//
//			if (oForesMP[i].first > nFiles)
//				nFiles = oForesMP[i].first;
//
//			for (int m = 0; m < (int) means.size(); m++)
//			{
//				if (means[m] > oldestStep)
//					oldestStep = means[m];
//			}
//		}
//
//		for (int i = 0; i < (int) oForesDP.size(); i++)
//		{
//			vector<int> means = oForesDP[i].second;
//
//			if (oForesDP[i].first > nFiles)
//				nFiles = oForesDP[i].first;
//
//			for (int m = 0; m < (int) means.size(); m++)
//			{
//				if (means[m] > oldestStep)
//					oldestStep = means[m];
//			}
//
//		}
//
//		vMaxLag[0] = oldestStep;
//		//cout << "nFiles =" << nFiles << endl;
//		//cout << "notUsedForTest = " << notUsedForTest << endl;
//		vNotUsedForTests.resize(nFiles);
//
//		for (int i = 0; i < (int) oForesSP.size(); i++)
//		{
//			vNotUsedForTests[oForesSP[i].first - 1] = max(oForesSP[i].second, vNotUsedForTests[oForesSP[i].first - 1]);
//		}
//
//		for (int i = 0; i < (int) oForesMP.size(); i++)
//		{
//			vector<int> means = oForesMP[i].second;
//
//			for (int m = 0; m < (int) means.size(); m++)
//			{
//				vNotUsedForTests[oForesMP[i].first - 1] = max(means[m], vNotUsedForTests[oForesMP[i].first - 1]);
//			}
//		}
//
//		for (int i = 0; i < (int) oForesDP.size(); i++)
//		{
//			vector<int> means = oForesDP[i].second;
//
//			for (int m = 0; m < (int) means.size(); m++)
//			{
//				vNotUsedForTests[oForesDP[i].first - 1] = max(means[m], vNotUsedForTests[oForesDP[i].first - 1]);
//			}
//
//		}
//	}

};

}

#endif /*HFM_PROBLEMPARAMETERS_HPP_*/

