#ifndef MODM_ADSMANAGER_H_
#define MODM_ADSMANAGER_H_

#include "../../OptFrame/ADSManager.hpp"

#include "Representation.h"
#include "ADS.h"
#include "ProblemInstance.h"

using namespace std;
using namespace optframe;

namespace MODM
{

class MODMADSManager: public ADSManager<RepMODM, AdsMODM>
{
private:
	typedef ADSManager<RepMODM, AdsMODM> super;
	ProblemInstance& pMODM;
	int c, n;

public:
	MODMADSManager(ProblemInstance& _pMODM) :
			pMODM(_pMODM)
	{
		c = pMODM.getNumberOfClients();
		n = pMODM.getNumberOfProducts();
		Component::verbose = false;
	}

	virtual ~MODMADSManager()
	{

	}

	//initialize ADS as you wish
	virtual void initializeADSNeighStructure(const RepMODM& rep, AdsMODM& ads)
	{

	}

	//update ADS as you wish
	virtual void setNeighLocalOptimum(const RepMODM& rep, AdsMODM& ads, string moveID)
	{

		for (int r = 0; r < rep.size(); r++)
			ads.neighborhoodStatus[moveID][r] = true;
	}

	void initializeADS(const RepMODM& newRep, AdsMODM& newAds)
	{
		newAds.clientOffers.clear();
		newAds.productOffers.clear();
		newAds.totalCost.clear();
		newAds.totalRevenue.clear();

		newAds.clientOffers.resize(c);
		newAds.productOffers.resize(n);
		newAds.totalCost.resize(n);
		newAds.totalRevenue.resize(n);

		vector<int> _clientOffers(c);
		vector<int> _productOffers(n);
		vector<int> _totalCost(n);
		vector<int> _totalRevenue(n);

		for (int product = 0; product < n; product++)
			for (int client = 0; client < c; client++)
			{
				_totalCost[product] += (newRep[client][product] * pMODM.getCost(client, product));
				_totalRevenue[product] += (newRep[client][product] * pMODM.getRevenue(client, product));
				_productOffers[product] += newRep[client][product];
				_clientOffers[client] += newRep[client][product];

			}

		newAds.clientOffers = _clientOffers;
		newAds.productOffers = _productOffers;
		newAds.totalCost = _totalCost;
		newAds.totalRevenue = _totalRevenue;

		if (Component::verbose)
		{
			cout << "Prints Initialize ADS" << endl;
			cout << "newAds.clientOffers:";
			cout << newAds.clientOffers << endl;
			cout << "newAds.productOffers:";
			cout << newAds.productOffers << endl;
			cout << "newAds.totalCost:";
			cout << newAds.totalCost << endl;
			cout << "newAds.totalRevenue:";
			cout << newAds.totalRevenue << endl << endl;
		}

	}

	virtual bool compareADS(const AdsMODM& _ads1, const AdsMODM& _ads2)
	{
		if (_ads1.clientOffers != _ads1.clientOffers)
		{
			cout << "Error on Client Offers Vector!" << endl;
			//getchar();
			return false;
		}

		if (_ads1.productOffers != _ads1.productOffers)
		{
			cout << "Error on productOffers Vector!" << endl;
			//getchar();
			return false;
		}

		if (_ads1.totalCost != _ads1.totalCost)
		{
			cout << "Error on totalCost Vector!" << endl;
			//getchar();
			return false;
		}

		if (_ads1.totalRevenue != _ads1.totalRevenue)
		{
			cout << "Error on totalRevenue Vector!" << endl;
			//getchar();
			return false;
		}
		return true;
	}

	virtual void printADS(const AdsMODM& ads)
	{
		cout << "---------------------------------" << endl;
		cout << "ADS print:" << endl;
		cout << ads.clientOffers << endl;
		cout << ads.productOffers << endl;
		cout << ads.totalCost << endl;
		cout << ads.totalRevenue << endl;
		cout << "FINISH ADS print" << endl;
		cout << "---------------------------------" << endl;
	}

};

}
#endif /* MODM_ADSMANAGER_H_ */
