/*
 * treatForecasts.h
 *
 *  Created on: 04/09/2014
 *      Author: vitor
 */

#ifndef TREATFORECASTS_H_
#define TREATFORECASTS_H_

namespace EFP
{

class treatForecasts
{
private:
	vector<vector<double> > forecastings;

public:

	treatForecasts(vector<string> explanatoryVariables)
	{
		vector<Scanner*> scannerFiles;
		for (int v = 0; v < explanatoryVariables.size(); v++)
		{
			File* fileP;

			try
			{
				fileP = new File(explanatoryVariables[v]);
			} catch (FileNotFound& f)
			{
				cout << "File '" << explanatoryVariables[v] << "' not found" << endl;
				exit(1);
			}

			Scanner* scanner = new Scanner(fileP);
			scannerFiles.push_back(scanner);
		}

		readFromScanner(scannerFiles);
	}

	void readFromScanner(vector<Scanner*>& scannerFiles)
	{
		int nExVar = scannerFiles.size();

		vector<int> nForecastings;

		nForecastings.resize(nExVar);
		forecastings.resize(nExVar);

		//vector with the number of forecasting in each file

		for (int exVar = 0; exVar < nExVar; exVar++)
		{
			Scanner scanner = *scannerFiles[exVar];

			nForecastings[exVar] = scanner.nextInt();
			//cout << "nForecastings[" << exVar << "]: " << nForecastings[exVar] << endl;

			for (int i = 0; i < nForecastings[exVar]; i++)
			{
				double input;
				input = scanner.nextDouble();
				//cout << input << endl;

				forecastings[exVar].push_back(input);
			}

			delete scannerFiles[exVar];
		}


	}

	int getForecastsDataSize()
	{
		return forecastings[0].size();
	}

	int getForecastsSize(int file)
	{
		return forecastings[file].size();
	}


	vector<vector<double> > readQuantilesFromFile(string quantilFile, int fileStepsAhead)
	{
		vector < vector<double> > quantiles(fileStepsAhead);

		File* fileP;

		try
		{
			fileP = new File(quantilFile);
		} catch (FileNotFound& f)
		{
			cout << "File '" << quantilFile << "' not found" << endl;
			getchar();
			return quantiles;
		}

		Scanner* scanner = new Scanner(fileP);

		for (int sa = 0; sa < fileStepsAhead; sa++)
		{
			vector<double> sampleQuantile;
			for (int q = 1; q < 100; q++)
			{
				double qa = scanner->nextDouble();
				sampleQuantile.push_back(qa);
			}
			quantiles[sa] = sampleQuantile;
		}

		return quantiles;

	}

	vector<double> getPartsForecastsBeginToEnd(int file, int b, int nPoints)
	{
		vector<double> partsOfForecasts(forecastings[file].begin() + b, forecastings[file].begin() + b + nPoints);

		return partsOfForecasts;
	}

	vector<double> getPartsForecastsBeginToEnd(vector<double> forecasts, int b, int nPoints)
	{
		vector<double> partsOfForecasts(forecasts.begin() + b, forecasts.begin() + b + nPoints);

		return partsOfForecasts;
	}

	vector<double> getPartsForecastsEndToBegin(int file, int e, int nPoints)
	{
		vector<double> partsOfForecasts(forecastings[file].end() - nPoints - e, forecastings[file].end() - e);

		return partsOfForecasts;
	}

	vector<double> getPartsForecastsEndToBegin(vector<double> forecasts, int e, int nPoints)
	{
		vector<double> partsOfForecasts(forecasts.end() - nPoints - e, forecasts.end() - e);

		return partsOfForecasts;
	}
	vector<double> getLastForecasts(int file, int lasts)
	{
		vector<double> partsOfForecasts(forecastings[file].end() - lasts, forecastings[file].end());

		return partsOfForecasts;
	}

	vector<double> getLastForecasts(vector<double> forecasts, int lasts)
	{
		vector<double> partsOfForecasts(forecasts.end() - lasts, forecasts.end());

		return partsOfForecasts;
	}
	vector<double> getFirstForecasts(int file, int firsts)
	{
		vector<double> partsOfForecasts(forecastings[file].begin(), forecastings[file].begin() + firsts);

		return partsOfForecasts;
	}

	vector<vector<double> > splitForecasts(vector<double> forecast, int pieces)
	{
		int totalForecasts = forecast.size();
		int sizePieces = totalForecasts / pieces;

		vector < vector<double> > vForecastPartbyPart;

		for (int p = 0; p < pieces; p++)
		{
			vector<double> vForecastPart;
			for (int i = 0; i < sizePieces; i++)
			{
				double forecastValue;
				forecastValue = forecast[p * sizePieces + i];
				vForecastPart.push_back(forecastValue);
			}
			vForecastPartbyPart.push_back(vForecastPart);
		}

		return vForecastPartbyPart;
	}

	vector<vector<vector<double> > > splitForecastsVectors(vector<vector<double> > vForecast, int pieces)
	{
		vector < vector<vector<double> > > vForecastsPartbyPart;
		int nForecastVectors = vForecast.size();
		for (int n = 0; n < nForecastVectors; n++)
		{
			vector < vector<double> > forecastPartbyPart;
			forecastPartbyPart = splitForecasts(vForecast[n], pieces);
			vForecastsPartbyPart.push_back(forecastPartbyPart);
		}

		return vForecastsPartbyPart;
	}

	vector<vector<double> > divideByWeekDays(vector<double> forecast)
	{
		int totalForecasts = forecast.size();

		vector < vector<double> > vForecastDayByDay(7);

		int d = 0;
		for (int i = 0; i < totalForecasts; i = i + 24)
		{
			for (int h = 0; h < 24; h++)
			{
				if ((totalForecasts - 1 - h - i) >= 0)
					vForecastDayByDay[d].push_back(forecast[totalForecasts - 1 - h - i]);
			}
			d++;
			if (d == 7)
				d = 0;
		}

		for (int i = 0; i < 7; i++)
			reverse(vForecastDayByDay[i].begin(), vForecastDayByDay[i].end());

		return vForecastDayByDay;
	}

	double getMape(vector<double> forecasts, vector<double> realValue)
	{
		double mape = 0;
		for (int samples = 0; samples < forecasts.size(); samples++)
		{
			mape += (abs(forecasts[samples] - realValue[samples]) / abs(realValue[samples]));
		}
		mape = mape * 100;
		mape = mape / forecasts.size();
		return mape;

	}

	double getPinball(vector<vector<double> > quantisForecasts, vector<double> realValues)
	{
		double pinball = 0;

		for (int samples = 0; samples < quantisForecasts.size(); samples++)
		{
			double pinballFunctionQuantils = 0;
			for (int a = 0; a < 99; a++)
			{
				double qa = quantisForecasts[samples][a];

				if (realValues[samples] < qa)
					pinballFunctionQuantils += (1 - ((a + 1) / 100.0)) * (qa - realValues[samples]);

				if (realValues[samples] >= qa)
					pinballFunctionQuantils += ((a + 1) / 100.0) * (realValues[samples] - qa);
			}
			pinballFunctionQuantils /= 99.0;
			pinball += pinballFunctionQuantils;
		}

		//cout << quantisForecasts.size() << endl;
		pinball = pinball / quantisForecasts.size();
		return pinball;

	}
	vector<double> mixWeekDays(vector<vector<double> > forecasts, int samplesToMix)
	{

		vector<double> finalResult;

		int iter = 0;
		for (int i = 0; i < samplesToMix; i = i + 168)
		{
			for (int d = 0; d < 7; d++)
				for (int h = 0; h < 24; h++)
				{
					if (finalResult.size() < samplesToMix)
						finalResult.push_back(forecasts[d][h + iter * 24]);
				}
			iter++;
		}

		return finalResult;
	}

	void generateProbabilisticOnFile(vector<double> forecasts, string outFile)
	{

		for (double quantilError = 0; quantilError <= 1; quantilError = quantilError + 0.05)
		{
			vector < vector<double> > forecastings;
			for (int i = 0; i < forecasts.size(); i++)
			{
				double prev;
				prev = forecasts[i];

				vector<double> quantils;
				for (int a = 1; a < 100; a++)
				{
					double quantilFactor = (50 - a) / 100.0;
					double qa = prev - prev * quantilError * quantilFactor;
					quantils.push_back(qa);
				}

				forecastings.push_back(quantils);
			}

			stringstream ss;
			ss << outFile << quantilError;

			string output = ss.str();

			FILE* fResults = fopen(output.c_str(), "w");
			for (int i = 0; i < forecastings.size(); i++)
			{
				for (int a = 0; a < 99; a++)
				{
					fprintf(fResults, "%.7f\t", forecastings[i][a]);

				}
				fprintf(fResults, "\n");
			}
			fclose(fResults);
		}

	}

	vector<vector<double> > generateQuantisProbabilisticVectorFixedQuantilError(vector<double> forecasts, double quantilError)
	{
		vector < vector<double> > vQuantisResults;

		for (int i = 0; i < forecasts.size(); i++)
		{
			double prev;
			prev = forecasts[i];

			vector<double> quantils;
			for (int a = 1; a < 100; a++)
			{
				double quantilFactor = (50 - a) / 100.0;
				double qa = prev - prev * quantilError * quantilFactor;
				quantils.push_back(qa);
			}

			vQuantisResults.push_back(quantils);
		}

		return vQuantisResults;

	}

	vector<vector<vector<double> > > generateQuantisProbabilisticVector(vector<double> forecasts)
	{
		vector < vector<vector<double> > > vQuantisResults;

		for (double quantilError = 0; quantilError <= 1; quantilError = quantilError + 0.05)
		{
			vector < vector<double> > forecastings;
			for (int i = 0; i < forecasts.size(); i++)
			{
				double prev;
				prev = forecasts[i];

				vector<double> quantils;
				for (int a = 1; a < 100; a++)
				{
					double quantilFactor = (50 - a) / 100.0;
					double qa = prev - prev * quantilError * quantilFactor;
					quantils.push_back(qa);
				}

				forecastings.push_back(quantils);
			}

			vQuantisResults.push_back(forecastings);

		}
		return vQuantisResults;

	}

	void exportQuantisVector(vector<vector<double> > quantis, string outFile)
	{

		FILE* fResults = fopen(outFile.c_str(), "w");
		for (int i = 0; i < quantis.size(); i++)
		{
			for (int a = 0; a < quantis[0].size(); a++)
			{
				fprintf(fResults, "%.7f\t", quantis[i][a]);

			}
			fprintf(fResults, "\n");
		}
		fclose(fResults);

	}

	void exportVectorOfVector(vector<vector<double> > batchesVector, string outFile)
	{

		FILE* fResults = fopen(outFile.c_str(), "w");
		for (int i = 0; i < batchesVector.size(); i++)
		{
			for (int a = 0; a < batchesVector[0].size(); a++)
			{
				fprintf(fResults, "%.7f\t", batchesVector[i][a]);

			}
			fprintf(fResults, "\n");
		}
		fclose(fResults);

	}

	vector<double> generateQuantis(vector<double> forecasts, int firstQuantil, int lastQuantil)
	{

		sort(forecasts.begin(), forecasts.end(), compara); // ordena com QuickSort
		//cout<<forecasts<<endl;
		//getchar();
		int sizeForecasts = forecasts.size();
		int nQuantis = lastQuantil - firstQuantil + 1;
		vector<double> quantis;
		for (int q = firstQuantil; q <= lastQuantil; q++)
		{
			double quantilIndex;
			int quantilIndexMin, quantilIndexMax;
			double quantilIndexMinWeight, quantilIndexMaxWeight;
			quantilIndex = (sizeForecasts + 1.00) * q / nQuantis;

			if ((fmod(quantilIndex, 1)) != 0) // is not integer
			{
				quantilIndexMin = floor(quantilIndex);
				quantilIndexMax = ceil(quantilIndex);
				double decimal = quantilIndex - quantilIndexMin;
				quantilIndexMinWeight = 1 - decimal;
				quantilIndexMaxWeight = decimal;
			}
			else //has no decimal values
			{
				quantilIndexMin = quantilIndex;
				quantilIndexMax = quantilIndex;
				quantilIndexMinWeight = 0;
				quantilIndexMaxWeight = 1;
			}

			double quantilValue;
			while (quantilIndexMin >= sizeForecasts)
				quantilIndexMin--;
			while (quantilIndexMax >= sizeForecasts)
				quantilIndexMax--;
			while (quantilIndexMin <= 0)
				quantilIndexMin++;
			while (quantilIndexMax <= 0)
				quantilIndexMax++;

			quantilValue = (forecasts[quantilIndexMin - 1] * quantilIndexMinWeight + forecasts[quantilIndexMax - 1] * quantilIndexMaxWeight);
			/*cout << q << "\t" << nQuantis << "\t" << sizeForecasts << endl;
			 cout << quantilIndex << endl;
			 cout << fmod(quantilIndex, 1) << endl;
			 cout << quantilIndexMin << endl;
			 cout << quantilIndexMax << endl;
			 cout << quantilIndexMinWeight << endl;
			 cout << quantilIndexMaxWeight << endl;
			 cout << quantilValue << endl;
			 getchar();*/
			quantis.push_back(quantilValue);
		}

		return quantis;
	}

	virtual ~treatForecasts()
	{

	}
}
;

} /* namespace EFP */
#endif /* TREATFORECASTS_H_ */
