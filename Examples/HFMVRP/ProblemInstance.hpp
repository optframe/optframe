#ifndef HFMVRP_PROBLEMINSTANCE_HPP_
#define HFMVRP_PROBLEMINSTANCE_HPP_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

using namespace std;
using namespace scannerpp;

namespace HFMVRP
{

static bool compare(pair<int, double> p1, pair<int, double> p2)
{
	return p1.second > p2.second;
}

class ProblemInstance
{
private:
	// Your private vars

public:
	ProblemInstance(Scanner& scanner)
	{
		for (int i = 0; i < 4; i++)
			cout << scanner.nextLine() << endl;
		//Problem data
		cout << scanner.next() << " ";
		nodes = scanner.nextDouble();
		cout << nodes << endl;

		cout << scanner.next() << " ";
		nVehicles = scanner.nextDouble();
		cout << nVehicles << endl;

		cout << scanner.next() << " ";
		typesVehicles = scanner.nextDouble();
		cout << typesVehicles << endl;

		//Veihicles types
		for (int i = 0; i < 3; i++)
			cout << scanner.nextLine() << endl;
		double sumCap = 0;
		for (int i = 0; i < typesVehicles; i++)
		{
			cout << scanner.next() << " "; //id
			vehiclesTypeNumber.push_back(scanner.nextDouble());
			vehiclesTypeCap.push_back(scanner.nextDouble());
			vehiclesTypeDistCost.push_back(scanner.nextDouble());
			vehiclesTypeFixedCost1.push_back(scanner.nextDouble());
			vehiclesTypeFixedBoxCost2.push_back(scanner.nextDouble());
			theoricalCap = 22;
			vehiclesTypeFixedCost.push_back(vehiclesTypeFixedCost1[i] + (vehiclesTypeCap[i] - theoricalCap) * vehiclesTypeFixedBoxCost2[i]);
			cout << vehiclesTypeNumber[i] << " " << vehiclesTypeCap[i] << " " << vehiclesTypeFixedCost[i] << " " << vehiclesTypeDistCost[i] << endl;
			sumCap += vehiclesTypeCap[i] * vehiclesTypeNumber[i];
		}


		//Stores IDS
		for (int i = 0; i < 3; i++)
			cout << scanner.nextLine() << endl;

		for (int i = 0; i < nodes; i++)
		{
			int storeID = scanner.nextDouble();
			storeIDS.push_back(storeID);
		}

		//Matrix of Distances
		for (int i = 0; i < 3; i++)
			cout << scanner.nextLine() << endl;

		dist = new Matrix<double> (nodes);

		int nullDist = 0;
		for (int i = 0; i < nodes; i++)
		{
			for (int j = 0; j < nodes; j++)
			{
				//cout<<"i= "<<i<<"j= "<<j<<endl;
				double nodeDist = scanner.nextDouble();
				//cout<<"nodeDist = "<<nodeDist<<endl;
				//getchar();
				if (nodeDist == 999)
				{
					nodeDist = 1000000;
					nullDist++;
				}
				(*dist)(i, j) = nodeDist;
			}
		}

		//cout<<"nullDist = "<<nullDist<<endl;
		//getchar();

		cout << "dist: " << (*dist);

		//Demands
		double sumDemand = 0;
		for (int i = 0; i < 3; i++)
			cout << scanner.nextLine() << endl;

		int nullDemands = 0;
		for (int c = 0; c < nodes; c++)
		{
			double demand = scanner.nextDouble();
			demands.push_back(demand);
			if (demands[c] == 0)
				nullDemands++;
			sumDemand += demand;
		}

		cout << demands << endl;
		cout << "active demands =" << demands.size() - nullDemands << endl;

		//Comp
		for (int i = 0; i < 3; i++)
			cout << scanner.nextLine() << endl;

		comp = new Matrix<double> (typesVehicles, nodes);

		for (int c = 0; c < nodes; c++)
		{
			int nComp = 0;
			for (int t = 0; t < typesVehicles; t++)
			{
				//cout<<"i= "<<i<<"j= "<<j<<endl;
				double compTruck = scanner.nextDouble();
				//cout<<"nodeDist = "<<nodeDist<<endl;
				//getchar();
				(*comp)(t, c) = compTruck;
				if (compTruck == true)
					nComp++;
			}
			nStoreComp.push_back(nComp);
		}

		cout << "comp: " << (*comp);

		//creat list of vehicles cap and costs
		for (int v = 0; v < nVehicles; v++)
		{
			int vType = getVehicleType(v);
			vehiclesCap.push_back(getVehicleTypeCap(vType));
			vehiclesFixedCost.push_back(getVehicleTypeFixedCost(vType));
			vehiclesDistCost.push_back(getVehicleTypeDistCost(vType));
		}

		/*
		 for (int v = 0; v < nVehicles; v++)
		 {
		 cout << v << "\t" << vehiclesCap[v] << "\t" << vehiclesFixedCost[v] << "\t"
		 << vehiclesDistCost[v] << endl;
		 }*/

		cout << "Sum Cap = " << sumCap << "\tSum Demand = " << sumDemand << endl;
		if (sumCap < sumDemand)
			cout << "MULTI-TRIP INSTANCE" << endl;
		cout << "#########################################################" << endl;
		cout << "############## Problem Instance Read ####################" << endl;
		cout << "#########################################################" << endl;
	}

	virtual ~ProblemInstance()
	{
	}

	int getVehicleType(int v)
	{
		int type = 0;
		int sumTypes = vehiclesTypeNumber[type];
		while (sumTypes <= v)
		{
			type++;
			sumTypes += vehiclesTypeNumber[type];
		}

		return type;
	}

	int getVehicleID(int type)
	{
		int id = 0;
		int sumTypes = 0;
		while (sumTypes != type)
		{
			id += vehiclesTypeNumber[sumTypes];
			sumTypes++;
		}

		return id;
	}

	double getVehicleCap(int v)
	{
		return vehiclesCap[v];
	}

	double getVehicleDistCost(int v)
	{
		return vehiclesDistCost[v];
	}

	double getVehicleFixedCost(int v)
	{
		return vehiclesFixedCost[v];
	}

	double getVehicleTypeCap(int v)
	{
		return vehiclesTypeCap[v];
	}

	double getVehicleTypeDistCost(int v)
	{
		return vehiclesTypeDistCost[v];
	}

	double getVehicleTypeFixedCost(int v)
	{
		return vehiclesTypeFixedCost[v];
	}

	double getVehicleNumber(int v)
	{
		return vehiclesTypeNumber[v];
	}

	double getDist(int i, int j)
	{
		return (*dist)(i, j);
	}

	int getStoreID(int s)
	{
		return storeIDS[s];
	}

	bool getComp(int v, int c)
	{
		return (*comp)(v, c);
	}

	double nodes;
	double nVehicles;
	double typesVehicles;
	// Matrizes
	Matrix<double>* dist;
	Matrix<double>* comp;
	vector<int> nStoreComp;
	//vectors
	vector<int> storeIDS;
	vector<double> demands;
	vector<int> vehiclesTypeNumber;
	vector<int> vehiclesTypeCap;
	vector<double> vehiclesTypeFixedCost;
	vector<double> vehiclesTypeFixedBoxCost2;
	vector<double> vehiclesTypeFixedCost1;
	vector<double> vehiclesTypeDistCost;
	vector<int> vehiclesCap;
	vector<double> vehiclesFixedCost;
	vector<double> vehiclesDistCost;
	int theoricalCap;

};

}

#endif /*HFMVRP_PROBLEMINSTANCE_HPP_*/

