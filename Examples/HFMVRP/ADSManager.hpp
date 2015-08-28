#ifndef OPTFRAME_ADSMANAGERHFMVRP_HPP_
#define OPTFRAME_ADSMANAGERHFMVRP_HPP_

using namespace std;

namespace HFMVRP
{

class HFMVRPADSManager: public ADSManager<RepHFMVRP, AdsHFMVRP>
{
	typedef ADSManager<RepHFMVRP, AdsHFMVRP> super;
	ProblemInstance& hfmvrp;
public:
	HFMVRPADSManager(ProblemInstance& _hfmvrp) :
			hfmvrp(_hfmvrp)
	{
	}

	virtual ~HFMVRPADSManager()
	{

	}

	//initialize ADS as you wish
	virtual void initializeADSNeighStructure(const RepHFMVRP& rep, AdsHFMVRP& ads)
	{
		/*
		 cout << "Entrou!" << endl;
		 getchar();
		 for (map<string, vector<bool> >::iterator iter = newAds.neighborhoodStatus.begin(); iter != newAds.neighborhoodStatus.end(); iter++)
		 {
		 cout << "iter->first = " << iter->first << endl;
		 cout << "iter->second = " << iter->second << endl;
		 }

		 getchar();
		 */

		for (int r = 0; r < rep.size(); r++)
		{
			(ads.neighborhoodStatus[DeltaMoveVRP2Opt::idComponent()]).push_back(false);
			(ads.neighborhoodStatus[DeltaMoveVRPOrOpt1::idComponent()]).push_back(false);
			(ads.neighborhoodStatus[DeltaMoveVRPOrOpt2::idComponent()]).push_back(false);
			(ads.neighborhoodStatus[DeltaMoveVRPExchange::idComponent()]).push_back(false);
			(ads.neighborhoodStatus[DeltaMoveVRPSwap1_1::idComponent()]).push_back(false);
			(ads.neighborhoodStatus[DeltaMoveVRPShift10::idComponent()]).push_back(false);
		}
		/*

		 for (map<string, vector<bool> >::iterator iter = ads.neighborhoodStatus.begin(); iter != ads.neighborhoodStatus.end(); iter++)
		 {
		 cout << "iter->first = " << iter->first << endl;
		 cout << "iter->second = " << iter->second << endl;
		 }
		 cout << "Finalizou!" << endl;

		 getchar();
		 */
	}

	//update ADS as you wish
	virtual void setNeighLocalOptimum(const RepHFMVRP& rep, AdsHFMVRP& ads, string moveID)
	{
		//cout << "moveID= " << moveID << endl;
		//getchar();
		for (int r = 0; r < rep.size(); r++)
			ads.neighborhoodStatus[moveID][r] = true;
	}

	void initializeADS(const RepHFMVRP& newRep, AdsHFMVRP& newAds)
	{
		int nRoutes = newRep.size(); //nVehicles plus multi-trip Vehicles
		newAds.sumDemand.clear();
		newAds.minDemand.clear();
		newAds.maxDemand.clear();
		newAds.minPairDemand.clear();
		newAds.maxPairDemand.clear();
		newAds.cumulativeDemand.clear();

		newAds.sumDemand.resize(nRoutes);
		newAds.minDemand.resize(nRoutes);
		newAds.maxDemand.resize(nRoutes);
		newAds.minPairDemand.resize(nRoutes);
		newAds.maxPairDemand.resize(nRoutes);
		newAds.cumulativeDemand.resize(nRoutes);

		for (int r = 0; r < nRoutes; r++)
		{
			vector<int> route = newRep[r];
			int routeSize = route.size();
			if (routeSize >= 3) //two depots plus one client
			{
				vector<double> routeDemands(routeSize);

				routeDemands[0] = 0; //depot demand
				routeDemands[routeSize - 1] = 0; //depot demand
				vector<double> cDemand;
				cDemand.push_back(0);

				for (int s = 1; s < int(routeSize - 1); s++)
				{
					routeDemands[s] = hfmvrp.demands[route[s]];
					newAds.sumDemand[r] += routeDemands[s]; // increment the total demand
					cDemand.push_back(routeDemands[s] + cDemand[s - 1]); // calculate the cumulative demand for it s of the route
				}

				cDemand.push_back(0);

				newAds.cumulativeDemand[r] = cDemand; //set  the cumulative demand to the vehicle

				//minPairDemand and maxPairDemand Calculus
				double minPairDemand = 100000000;
				double maxPairDemand = 0;
				for (int i = 1; i < int(routeSize - 1); i++)
				{
					if (i != int(routeSize - 2))
					{
						double pairDemand = routeDemands[i] + routeDemands[i + 1];
						if (pairDemand < minPairDemand)
							minPairDemand = pairDemand;
						if (pairDemand > maxPairDemand)
							maxPairDemand = pairDemand;
					}
				}
				if (maxPairDemand == 0)
				{
					minPairDemand = 0;
					maxPairDemand = 0;
				}

				newAds.maxPairDemand[r] = maxPairDemand;
				newAds.minPairDemand[r] = minPairDemand;

				//minDemand and maxDemand Calculus
				vector<pair<int, double> > rankDemands;
				for (int i = 1; i < int(routeSize - 1); i++)
					rankDemands.push_back(make_pair(i, routeDemands[i]));

				//cout << rankDemands << endl;

				sort(rankDemands.begin(), rankDemands.end(), compare);

				newAds.maxDemand[r] = rankDemands[0].second;
				newAds.minDemand[r] = rankDemands[rankDemands.size() - 1].second;

			}

		}

		/*
		 cout << "Initialized ADS" << endl;
		 cout << "newAds.cumulativeDemand" << newAds.cumulativeDemand << endl;
		 cout << "newAds.maxDemand" << newAds.maxDemand << endl;
		 cout << "newAds.minDemand" << newAds.minDemand << endl;
		 cout << "newAds.maxPairDemand" << newAds.maxPairDemand << endl;
		 cout << "newAds.minPairDemand" << newAds.minPairDemand << endl;
		 cout << "newAds.sumDemand" << newAds.sumDemand << endl;

		 getchar();
		 */

	}

	virtual bool compareADS(const AdsHFMVRP& _ads1, const AdsHFMVRP& _ads2)
	{
		bool compare = true;
		if (_ads1.maxDemand != _ads2.maxDemand)
		{
			cout << "Error on maxDemand vector!" << endl;
			cout << _ads1.maxDemand << endl;
			cout << _ads2.maxDemand << endl;
			for (int r = 0; r < _ads1.maxDemand.size(); r++)
			{
				if (_ads1.maxDemand[r] != _ads2.maxDemand[r])
				{
					cout << "r = " << r << endl;
					cout << _ads1.maxDemand[r] << endl;
					cout << _ads2.maxDemand[r] << endl;
					getchar();
				}
			}

			return false;
		}
		if (_ads1.minDemand != _ads2.minDemand)
		{
			cout << "Error on minDemand vector!" << endl;
			cout << _ads1.minDemand << endl;
			cout << _ads2.minDemand << endl;
			for (int r = 0; r < _ads1.minDemand.size(); r++)
			{
				if (_ads1.minDemand[r] != _ads2.minDemand[r])
				{
					cout << "r = " << r << endl;
					cout << _ads1.minDemand[r] << endl;
					cout << _ads2.minDemand[r] << endl;
					getchar();
				}
			}
			return false;
		}
		if (_ads1.maxPairDemand != _ads2.maxPairDemand)
		{
			cout << "Error on maxPairDemand vector!" << endl;
			cout << _ads1.maxPairDemand << endl;
			cout << _ads2.maxPairDemand << endl;
			for (int r = 0; r < _ads1.maxPairDemand.size(); r++)
			{
				if (_ads1.maxPairDemand[r] != _ads2.maxPairDemand[r])
				{
					cout << "r = " << r << endl;
					cout << _ads1.maxPairDemand[r] << endl;
					cout << _ads2.maxPairDemand[r] << endl;
					getchar();
				}
			}
			return false;
		}
		if (_ads1.minPairDemand != _ads2.minPairDemand)
		{
			cout << "Error on minPairDemand vector!" << endl;
			cout << _ads1.minPairDemand << endl;
			cout << _ads2.minPairDemand << endl;
			for (int r = 0; r < _ads1.minPairDemand.size(); r++)
			{
				if (_ads1.minPairDemand[r] != _ads2.minPairDemand[r])
				{
					cout << "r = " << r << endl;
					cout << _ads1.minPairDemand[r] << endl;
					cout << _ads2.minPairDemand[r] << endl;
					getchar();
				}
			}
			return false;
		}
		if (_ads1.sumDemand != _ads2.sumDemand)
		{
			cout << "Error on sumDemand vector!" << endl;
			return false;
		}

		if (_ads1.cumulativeDemand != _ads2.cumulativeDemand)
		{
			cout << _ads1.cumulativeDemand << endl;
			cout << _ads2.cumulativeDemand << endl;
			cout << "Error on CumulativeDemand vector!" << endl;
			for (int r = 0; r < _ads1.cumulativeDemand.size(); r++)
			{
				for (int c = 0; c < _ads1.cumulativeDemand[r].size(); c++)
				{
					if (_ads1.cumulativeDemand[r][c] != _ads2.cumulativeDemand[r][c])
					{
						cout << "r = " << r << "\tc = " << c << endl;
						cout << _ads1.cumulativeDemand[r][c] << endl;
						cout << _ads2.cumulativeDemand[r][c] << endl;
						getchar();
					}
				}
			}
			return false;
		}
		return compare;
	}

	virtual void printADS(const AdsHFMVRP& ads)
	{
		cout << "ads.cumulativeDemand" << ads.cumulativeDemand << endl;
		cout << "ads.maxDemand" << ads.maxDemand << endl;
		cout << "ads.minDemand" << ads.minDemand << endl;
		cout << "ads.maxPairDemand" << ads.maxPairDemand << endl;
		cout << "ads.minPairDemand" << ads.minPairDemand << endl;
		cout << "ads.sumDemand" << ads.sumDemand << endl;
	}

};
}
#endif /* OPTFRAME_UPDATEADSHFMVRP_HPP_ */
