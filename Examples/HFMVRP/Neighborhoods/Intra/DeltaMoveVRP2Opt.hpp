// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef DELTA_MOVEVRP2OPT_HPP_
#define DELTA_MOVEVRP2OPT_HPP_

#include "../../../../OptFrame/Util/NeighborhoodStructures/VRP/Intra/NSSeqVRP2Opt.hpp"

#include <cmath>

using namespace std;

namespace HFMVRP
{

class DeltaMoveVRP2Opt: public MoveVRP2Opt<int, AdsHFMVRP>
{
	typedef MoveVRP2Opt<int, AdsHFMVRP> super;

private:
	ProblemInstance* hfmvrp;

public:

	DeltaMoveVRP2Opt(int _r, int _p1, int _p2, ProblemInstance* _hfmvrp) :
		super(_r, _p1, _p2), hfmvrp(_hfmvrp)
	{
		if (!_hfmvrp)
		{
			cout << "Error: hfmvrp problem is NULL!" << endl;
			print();
			exit(1);
		}
	}

	virtual ~DeltaMoveVRP2Opt()
	{
	}

	int myabs(int x)
	{
		return std::abs(x);
	}

	virtual bool canBeApplied(const RepHFMVRP& rep, const AdsHFMVRP& ads) //TODO voltar ADS para const
	{
		if (r >= 0)
		{
			bool all_positive = (p1 >= 1) && p1 < (rep[r].size() - 1) && (p2 >= 1) && p2 < (rep[r].size() - 1);
			bool minSize = (rep[r].size() >= 3);

			//cout << "VRP 2 OPT canBeApplied 1 " << endl;
			//cout << "idComponent() = " << idComponent() << endl;
			//cout << "VRP 2 OPT upNeigh 1" << endl;

			//cout << "ads.neighborhoodStatus.size() = " << ads.neighborhoodStatus.size() << endl;
			////cout << "r = " << r << endl;
			//cout << "ads.neighborhoodStatus.find(idComponent())->second)[r] = " << (ads.neighborhoodStatus.find(idComponent())->second)[r] << endl;
			//cout << "VRP 2 OPT upNeigh 2" << endl;

			//getchar();
			//cout << "VRP 2 OPT canBeApplied 2 " << endl;
			//getchar();
			return all_positive && minSize;
		}
		else
		{
			return 0;
		}

	}

	void updateNeighStatus(AdsHFMVRP& ads)
	{
		for (map<string, vector<bool> >::iterator iter = ads.neighborhoodStatus.begin(); iter != ads.neighborhoodStatus.end(); iter++)
		{
			ads.neighborhoodStatus[iter->first][r] = false;
		}

	}

	void updateModifiedRoutes(RepHFMVRP& rep, AdsHFMVRP& ads)
	{
		vector<int> modRoutes; //modified routes
		modRoutes.push_back(r);

		for (int iter = 0; iter < modRoutes.size(); iter++)
		{
			int r = modRoutes[iter];
			vector<int> route = rep[r];
			int routeSize = route.size();

			vector<double> routeDemands(routeSize);

			routeDemands[0] = 0; //depot demand
			routeDemands[routeSize - 1] = 0; //depot demand

			ads.cumulativeDemand[r].clear(); //TODO leak memoria
			ads.cumulativeDemand[r].resize(routeSize);

			ads.cumulativeDemand[r][0] = 0; //depot cumulative demand
			ads.cumulativeDemand[r][routeSize - 1] = 0; //depot cumulative demand

			for (int c = 1; c < int(routeSize - 1); c++)
			{
				routeDemands[c] = hfmvrp->demands[route[c]];
				ads.cumulativeDemand[r][c] = routeDemands[c] + ads.cumulativeDemand[r][c - 1]; // calculate the cumulative demand for it s of the route
			}

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
				minPairDemand = routeDemands[1];
				maxPairDemand = routeDemands[1];
			}

			ads.maxPairDemand[r] = maxPairDemand;
			ads.minPairDemand[r] = minPairDemand;

		}


	}

	Move<RepHFMVRP, AdsHFMVRP>* apply(RepHFMVRP& rep, AdsHFMVRP& ads)
	{
		int small, bigger;
		if (p1 <= p2)
		{
			small = p1;
			bigger = p2;
		}
		else
		{
			small = p2;
			bigger = p1;
		}

		reverse(rep.at(r).begin() + small, rep.at(r).begin() + bigger);

		//Update ADS
		updateNeighStatus(ads);

		//Update minDemand,maxDemand, minPairDemand, maxPairDemand, cumulative and sum
		updateModifiedRoutes(rep, ads);

		return new DeltaMoveVRP2Opt(r, p1, p2, hfmvrp);
	}

	MoveCost* cost(const Evaluation&, const RepHFMVRP& rep, const AdsHFMVRP& ads)
	{
		vector<int> route = rep[r];
		int vType = -1;
		if (r >= hfmvrp->nVehicles)
			vType = 2;//MultiTripVehicle
		else
			vType = hfmvrp->getVehicleType(r);

		double vTDC = hfmvrp->getVehicleTypeDistCost(vType);

		unsigned k1 = p1;
		unsigned k2 = p2;

		// before k1 and k2
		unsigned bk1 = k1 - 1;
		unsigned bk2 = k2 - 1;

		if (k1 == 0)
			bk1 = route.size() - 1;
		if (k2 == 0)
			bk2 = route.size() - 1;

		double f = 0;

		if (myabs(k1 - k2) >= 2)
		{
			f -= hfmvrp->getDist(route[bk1], route[k1]) * vTDC;
			//cout << "-d(" << rep[bk1] << "," << rep[k1] << ") ";
			f -= hfmvrp->getDist(route[bk2], route[k2]) * vTDC;
			//cout << "-d(" << rep[bk2] << "," << rep[k2] << ") ";

			f += hfmvrp->getDist(route[bk1], route[bk2]) * vTDC;
			//cout << "+d(" << rep[bk1] << "," << rep[bk2] << ") ";
			f += hfmvrp->getDist(route[k1], route[k2]) * vTDC;
			//cout << "+d(" << rep[k1] << "," << rep[k2] << ") ";
		}
		//cout << endl;

		return new MoveCost (f, 0);
	}

	static string idComponent()
	{
		string idComp = super::idComponent();
		idComp.append(":DeltaMoveVRP2Opt");
		return idComp;
	}

	virtual bool operator==(const Move<RepHFMVRP, AdsHFMVRP>& _m) const
	{
		const DeltaMoveVRP2Opt& m1 = (const DeltaMoveVRP2Opt&) _m;
		return ((m1.p1 == p1) && (m1.p2 == p2));
	}

	virtual void print() const
	{
		cout << "DeltaMoveVRP2Opt( route: " << r << " : ";
		cout << " edge " << p1 << " <=>  edge " << p2 << " )";
		cout << endl;
	}
};

}

#endif /*DeltaMoveVRP2Opt*/
