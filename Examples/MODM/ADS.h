#ifndef MODM_ADS_H_
#define MODM_ADS_H_

#include <vector>
#include "../../OptFrame/Util/printable.h"

using namespace std;

// ADS Representation

namespace MODM
{

struct AdsMODM
{
	vector<int> clientOffers;
	vector<int> productOffers;
	vector<int> totalCost;
	vector<int> totalRevenue;

	map<string, vector<bool> > neighborhoodStatus; // mapping 'move.id()' to 'NeighborhoodStatus'
};

}
//#include <ostream>
//ostream & operator<< (ostream & s, RepHFMVRP){}

#endif /*MODM_ADS_H_*/

