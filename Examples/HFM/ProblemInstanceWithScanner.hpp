#ifndef HFM_PROBLEMINSTANCE_SCANNER_HPP_
#define HFM_PROBLEMINSTANCE_SCANNER_HPP_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

using namespace std;
using namespace scannerpp;

namespace HFM
{

class HFMProblemInstance
{
private:
	// Your private vars
	vector<double> means;
	vector<double> stdDesvs;
	//int precision;
	//int options;
	//int function;
	int stepsHead;
	ProblemParameters problemParam;
	vector<vector<double> > forecastings;

public:
	ProblemInstance(vector<Scanner*>& scannerFiles, ProblemParameters& _problemParam) :
			problemParam(_problemParam)
	{
		//precision = problemParam.getPrecision();
		//options = problemParam.getOptions();
		//function = problemParam.getFunction();
		stepsHead = problemParam.getStepsAhead();

		// Put here your code
		// You can read the input data from the 'scanner' object

		//number of different explanatory variables
		int nExVar = scannerFiles.size();

		vector<int> nForecastings;

		nForecastings.resize(nExVar);
		forecastings.resize(nExVar);
		means.resize(nExVar);
		stdDesvs.resize(nExVar);

		//vector with the number of forecasting in each file

		for (int exVar = 0; exVar < nExVar; exVar++)
		{
			Scanner scanner = *scannerFiles[exVar];

			nForecastings[exVar] = scanner.nextInt();
			cout << "nForecastings[" << exVar << "]: " << nForecastings[exVar] << endl;

			for (int i = 0; i < nForecastings[exVar]; i++)
			{
				double input;
				input = scanner.nextDouble();
				cout << input << endl;
				forecastings[exVar].push_back(input);
			}

			means[exVar] = 0;
			stdDesvs[exVar] = 0;
		}

		//Average Calc
		for (int exVar = 0; exVar < nExVar; exVar++)
		{
			for (int i = 0; i < nForecastings[exVar]; i++)
			{
				means[exVar] += getForecastings(exVar, i);

			}

			means[exVar] = means[exVar] / nForecastings[exVar];

			//cout << "Media = " << mean << endl;

			for (int i = 0; i < nForecastings[exVar]; i++)
			{
				double desv = getForecastings(exVar, i) - means[exVar];
				stdDesvs[exVar] += desv * desv;
			}

			stdDesvs[exVar] = stdDesvs[exVar] / nForecastings[exVar];
			stdDesvs[exVar] = sqrt(stdDesvs[exVar]);
		}
		cout << means << endl;
		cout << stdDesvs << endl;
		//getchar();

		// ==================== Check if #stepsAhead are possible to be done
		int notUsedForTest = problemParam.getNotUsedForTest();

		/*
		 stringstream ss;
		 ss << problemParam.getValidationPath();
		 File* file;

		 try
		 {
		 file = new File(ss.str());
		 } catch (FileNotFound& f)
		 {
		 cout << "File '" << ss.str() << "' not found" << endl;
		 }

		 Scanner scannerBlind(file);

		 int nForecastingsBlind = scannerBlind.nextInt();
		 cout << "nBlind: " << nForecastingsBlind << endl;
		 */

		//================================================
	}

	double getMean(int file)
	{
		return means[file];
	}

	vector<double> getMeans()
	{
		return means;
	}

	vector<double> getStdDesvs()
	{
		return stdDesvs;
	}

	double getStdDesv(int file)
	{
		return stdDesvs[file];
	}

	int getForecatingsSizeFile(int file)
	{
		return forecastings[file].size();
	}

	double getForecastings(int file, int i)
	{
		return forecastings[file][i];
	}

	virtual ~HFMProblemInstance()
	{
	}

};

}

#endif /*HFM_PROBLEMINSTANCE_SCANNER_HPP_*/

