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

#ifndef DELTA_MOVEVRPEXCHANGE_HPP_
#define DELTA_MOVEVRPEXCHANGE_HPP_

#include "../../../../OptFrame/Util/NeighborhoodStructures/VRP/Intra/NSSeqVRPExchange.hpp"

#include <cmath>

using namespace std;

namespace HFMVRP
{

class DeltaMoveVRPExchange: public MoveVRPExchange<int, AdsHFMVRP>
{
	typedef MoveVRPExchange<int, AdsHFMVRP> super;

private:
	ProblemInstance* hfmvrp;

public:

	DeltaMoveVRPExchange(int _r, int _c1, int _c2, ProblemInstance* _hfmvrp) :
		super(_r, _c1, _c2), hfmvrp(_hfmvrp)
	{
		if (!_hfmvrp)
		{
			cout << "Error: hfmvrp problem is NULL!" << endl;
			print();
			exit(1);
		}
	}

	virtual ~DeltaMoveVRPExchange()
	{
	}

	int myabs(int x)
	{
		return std::abs(x);
	}

	virtual bool canBeApplied(const RepHFMVRP& rep, const AdsHFMVRP& ads)
	{
		if (r >= 0)
		{
			bool all_positive = (c1 >= 1) && (c1 < (rep[r].size() - 1)) && (c2 >= 1) && (c2 < (rep[r].size() - 1)) && (r >= 0);

			return all_positive && (rep.at(r).size() >= 5) ; //asymmetric or symmetric todo If asymmetric =>rep.at(r).size() >=4
		}
		else
		{
			return 0;
		}
	}

	virtual void updateNeighStatus(AdsHFMVRP& ads)
	{
		for (map<string, vector<bool> >::iterator iter = ads.neighborhoodStatus.begin(); iter != ads.neighborhoodStatus.end(); iter++)
		{
			ads.neighborhoodStatus[iter->first][r] = false;
		}

	}

	Move<RepHFMVRP, AdsHFMVRP>* apply(RepHFMVRP& rep, AdsHFMVRP& ads)
	{

		int small, bigger;
		if (c1 <= c2)
		{
			small = c1;
			bigger = c2;
		}
		else
		{
			small = c2;
			bigger = c1;
		}

		int aux = rep.at(r).at(c1);
		rep.at(r).at(c1) = rep.at(r).at(c2);
		rep.at(r).at(c2) = aux;

		//Update ADS
		updateNeighStatus(ads);

		for (int s = small; s < bigger; s++)
		{
			if ((s - 1) >= 0)
				ads.cumulativeDemand[r][s] = ads.cumulativeDemand[r][s - 1] + hfmvrp->demands[rep[r][s]];
		}

		//minPairDemand and maxPairDemand Calculus
		double minPairDemand = 100000000;
		double maxPairDemand = 0;
		for (int i = 1; i < int(rep[r].size() - 1); i++)
		{
			if (i != int(rep[r].size() - 2))
			{
				double pairDemand = hfmvrp->demands[rep[r][i]] + hfmvrp->demands[rep[r][i + 1]];
				if (pairDemand < minPairDemand)
					minPairDemand = pairDemand;
				if (pairDemand > maxPairDemand)
					maxPairDemand = pairDemand;
			}
		}
		if (maxPairDemand == 0)
		{
			minPairDemand = rep[r][1];
			maxPairDemand = rep[r][1];
		}

		ads.maxPairDemand[r] = maxPairDemand;
		ads.minPairDemand[r] = minPairDemand;

		return new DeltaMoveVRPExchange(r, c1, c2, hfmvrp);
	}

	MoveCost* cost(const Evaluation&, const RepHFMVRP& rep, const AdsHFMVRP& ads)
	{
		vector<int> route = rep[r];
		int routeSize = route.size();
		//cout << "routeSize = " << routeSize << endl;
		//cout << route << endl;
		//getchar();
		int vType = -1;
		if (r >= hfmvrp->nVehicles)
			vType = 2;//MultiTripVehicle
		else
			vType = hfmvrp->getVehicleType(r);

		double vTDC = hfmvrp->getVehicleTypeDistCost(vType);

		unsigned k1 = c1;
		unsigned k2 = c2;

		// before k1 and k2
		unsigned bk1 = k1 - 1;
		unsigned bk2 = k2 - 1;

		// after k1 and k2
		unsigned ak1 = k1 + 1;
		unsigned ak2 = k2 + 1;

		double f = 0;

		if (myabs(k1 - k2) >= 2)
		{
			f -= hfmvrp->getDist(route[bk1], route[k1]) * vTDC;
			//cout << "-d(" << route[bk1] << "," << route[k1] << ") \t f= " << f << endl;
			f -= hfmvrp->getDist(route[k1], route[ak1]) * vTDC;
			//cout << "-d(" << route[k1] << "," << route[ak1] << ") \t f= " << f << endl;
			f -= hfmvrp->getDist(route[bk2], route[k2]) * vTDC;
			//cout << "-d(" << route[bk2] << "," << route[k2] << ") \t f= " << f << endl;
			f -= hfmvrp->getDist(route[k2], route[ak2]) * vTDC;
			//cout << "-d(" << route[k2] << "," << route[ak2] << ") \t f= " << f << endl;

			f += hfmvrp->getDist(route[bk1], route[k2]) * vTDC;
			//cout << "+d(" << route[bk1] << "," << route[k2] << ") \t f= " << f << endl;
			f += hfmvrp->getDist(route[k2], route[ak1]) * vTDC;
			//cout << "+d(" << route[k2] << "," << route[ak1] << ") \t f= " << f << endl;
			f += hfmvrp->getDist(route[bk2], route[k1]) * vTDC;
			//cout << "+d(" << route[bk2] << "," << route[k1] << ") \t f= " << f << endl;
			f += hfmvrp->getDist(route[k1], route[ak2]) * vTDC;
			//cout << "+d(" << route[k1] << "," << route[ak2] << ") \t f= " << f << endl;
		}
		else
		{
			if (k1 < k2)
			{

				f -= hfmvrp->getDist(route[bk1], route[k1]) * vTDC;
				//cout << "-d(" << route[bk1] << "," << route[k1] << ") \t f= " << f << endl;

				f -= hfmvrp->getDist(route[k2], route[ak2]) * vTDC;
				//cout << "-d(" << route[k2] << "," << route[ak2] << ") \t f= " << f << endl;

				f += hfmvrp->getDist(route[bk1], route[k2]) * vTDC;
				//cout << "+d(" << route[bk1] << "," << route[k2] << ") \t f= " << f << endl;
				f += hfmvrp->getDist(route[k1], route[ak2]) * vTDC;
				//cout << "+d(" << route[k1] << "," << route[ak2] << ") \t f= " << f << endl;
			}
			else
			{
				f -= hfmvrp->getDist(route[k1], route[ak1]) * vTDC;
				//cout << "-d(" << route[k1] << "," << route[ak1] << ") \t f= " << f << endl;

				f -= hfmvrp->getDist(route[bk2], route[k2]) * vTDC;
				//cout << "-d(" << route[bk2] << "," << route[k2] << ") \t f= " << f << endl;

				f += hfmvrp->getDist(route[bk2], route[k1]) * vTDC;
				//cout << "+d(" << route[bk2] << "," << route[k1] << ") \t f= " << f << endl;
				f += hfmvrp->getDist(route[k2], route[ak1]) * vTDC;
				//cout << "+d(" << route[k2] << "," << route[ak1] << ") \t f= " << f << endl;

			}
		}
		//cout << endl;

		return new MoveCost (f, 0);
	}

	string id() const
	{
		stringstream ssIdRoute;
		ssIdRoute << super::idComponent() << ":DeltaMoveVRPExchange" << "+Route=" << r;
		return ssIdRoute.str();
	}

	static string idComponent()
	{
		string idComp = super::idComponent();
		idComp.append(":DeltaMoveVRPExchange");
		return idComp;
	}

	virtual bool operator==(const Move<RepHFMVRP, AdsHFMVRP>& _m) const
	{
		const DeltaMoveVRPExchange& m1 = (const DeltaMoveVRPExchange&) _m;
		return ((m1.c1 == c1) && (m1.c2 == c2) && (m1.r == r));
	}

	virtual void print() const
	{
		cout << "DeltaMoveVRPExchange( route: " << r << " : ";
		cout << " client " << c1 << " <=>  client" << c2 << " )";
		cout << endl;
	}
};

}

#endif /*DELTA_MOVEVRPEXCHANGE_HPP_*/
