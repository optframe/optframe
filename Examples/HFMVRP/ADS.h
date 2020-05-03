#ifndef HFMVRP_ADS_H_
#define HFMVRP_ADS_H_

#include <vector>
#include "../../OptFrame/Util/printable.h"

#include "../../OptFrame/Util/Matrix.hpp"

using namespace std;

// ADS Representation


struct AdsHFMVRP
{
	vector<double> sumDemand;
	vector<double> minDemand;
	vector<double> maxDemand;
	vector<double> minPairDemand;
	vector<double> maxPairDemand;
	vector<vector<double> > cumulativeDemand;

	map<string, vector<bool> > neighborhoodStatus; // mapping 'move.id()' to 'NeighborhoodStatus'
};

//#include <ostream>
//ostream & operator<< (ostream & s, RepHFMVRP){}

#endif /*HFMVRP_ADS_H_*/

