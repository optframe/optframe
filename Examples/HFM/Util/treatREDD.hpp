#ifndef HFM_TREATREEDATASET_HPP_
#define HFM_TREATREEDATASET_HPP_

#include <vector>   // vector
#include <cmath>     // abs
#include <algorithm> // sort
#include <iomanip>
#include <fstream>

namespace HFM
{

//Written by Alexandre
class treatREEDDataset
{
public:

	treatREEDDataset()
	{

	}

	void createInstance(vector<pair<double, double> > data, string outputName)
	{
		ofstream output;

		output.open(outputName.c_str());

		output << data.size() << endl;

		for (int indexData = 0; indexData < (int) data.size(); indexData++)
		{
			output << setprecision(5) << data[indexData].second << endl;
		}
		output.close();
	}

	vector<pair<double, double> > separateThroughIntervals(vector<pair<double, double> > data, int intervalSeconds, string outputName, bool printOutput)
	{
		vector < pair<double, double> > demandSeparated;

		ofstream output;

		if (printOutput)
			output.open(outputName.c_str());

		double watts;
		double times;
		double timesPast;
		bool initialize = true;

		vector<double> wattsVector;

		for (int indexData = 0; indexData < (int) data.size(); indexData++)
		{

			times = data[indexData].first;
			watts = data[indexData].second;

//			cout<<indexData<<endl;
//			getchar();

			wattsVector.push_back(watts);

			if (initialize)
			{
				timesPast = times;
				initialize = false;
			}

			if (times - timesPast >= (intervalSeconds - 1))
			{
				double sum = 0;
				int averagePoints = wattsVector.size();
				for (int i = 0; i < averagePoints; i++)
				{
					sum = sum + wattsVector[i];
				}
				double wattsAverage = sum / averagePoints;
//				cout<<"cont size ="<<cont<<endl;
//				getchar();

				demandSeparated.push_back(make_pair(times, wattsAverage));

				if (printOutput)
					output << setprecision(10) << times << " " << setprecision(5) << wattsAverage << endl;

//				cout << wattsVector.size() << endl;
				wattsVector.clear();
//				cout << wattsVector << endl;
				timesPast = times;
				wattsVector.push_back(watts);
//				getchar();
			}

		}

		if (printOutput)
			output.close();

		return demandSeparated;

	}
	vector<pair<double, double> > interpolate(vector<pair<double, double> > data, int intervalSeconds)
	{
		vector < pair<double, double> > demandOutput;
		double watts;
		double wattsPast;
		double times;
		double timesPast;
		double interpolation;
//		int i = 0, size = 0;

		timesPast = data[0].first;
		wattsPast = data[0].second;

		for (int i = 1; i < (int) data.size(); i++)
		{

			times = data[i].first;
			watts = data[i].second;

			if (timesPast != (times - 1))
			{
				for (int size = 1; size <= times - timesPast; size++)
				{
					interpolation = watts + (((timesPast + size) - times) / (timesPast - times)) * (wattsPast - watts);
					demandOutput.push_back(make_pair(timesPast + size, interpolation));
				}

			}
			else
				demandOutput.push_back(make_pair(times, watts));

			wattsPast = watts;
			timesPast = times;

		}

		return demandOutput;
	}

	vector<pair<double, double> > cutData(string inputName, string outputName)
	{

//		File* file;
//		try
//		{
//			file = new File(inputName);
//		} catch (FileNotFound& f)
//		{
//			cout << "Parameters File '" << inputName << "' not found" << endl;
//			exit(1);
//		}
//		Scanner scanner(inputName);

		vector < pair<double, double> > demandOutput;
		ifstream input;
		ofstream output;
		double watts;
//		double wattsPast;
		double times;
//		double timesPast;
//		double interpolation;
//		bool initialize = true;
//		int i = 0, size = 0;
//		int interval = 60;

//		cout << inputName.c_str() << endl;
		input.open(inputName.c_str());

		if (!input.is_open())
		{
			cout << "Parameters File '" << inputName << "' not found" << endl;
			exit(1);
		}
//		while (scanner.hasNext())
//		{
//
//			times = scanner.nextDouble();
//			watts = scanner.nextDouble();

		while (!input.eof())
		{

			input >> times;
			input >> watts;

//			cout << times << "\t" << watts << endl;
//			getchar();

//1304373540//
			if ((times > 1303164000) && (times < 1304373540))
			{
				demandOutput.push_back(make_pair(times, watts));
			}

		}

		input.close();

		return demandOutput;

	}

};

}

#endif /* HFM_TREATREEDATASET_HPP_ */
