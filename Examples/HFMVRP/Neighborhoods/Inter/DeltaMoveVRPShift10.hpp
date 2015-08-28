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

#ifndef DELTA_MOVEVRPSHIFT1_0_HPP_
#define DELTA_MOVEVRPSHIFT1_0_HPP_

#include "../../../../OptFrame/Util/NeighborhoodStructures/VRP/Inter/NSSeqVRPShift10.hpp"

#include <cmath>

using namespace std;

namespace HFMVRP
{

class DeltaMoveVRPShift10: public MoveVRPShift10<int, AdsHFMVRP>
{
	typedef MoveVRPShift10<int, AdsHFMVRP> super;

private:
	ProblemInstance* hfmvrp;
	int vType1;
	int vType2;

public:

	DeltaMoveVRPShift10(int _r1, int _r2, int _cli, int _pos, ProblemInstance* _hfmvrp) :
		super(_r1, _r2, _cli, _pos), hfmvrp(_hfmvrp)
	{
		if (!_hfmvrp)
		{
			cout << "Error: hfmvrp problem is NULL!" << endl;
			print();
			exit(1);
		}

		if (r1 >= hfmvrp->nVehicles)
			vType1 = 2;//MultiTripVehicle
		else
			vType1 = hfmvrp->getVehicleType(r1);

		if (r2 >= hfmvrp->nVehicles)
			vType2 = 2;//MultiTripVehicle
		else
			vType2 = hfmvrp->getVehicleType(r2);

	}

	virtual ~DeltaMoveVRPShift10()
	{
	}

	int myabs(int x)
	{
		return std::abs(x);
	}
	virtual bool canBeApplied(const RepHFMVRP& rep, const AdsHFMVRP& ads)
	{
		if (r1 >= 0)
		{
			bool all_positive = (r1 >= 0) && (r2 >= 0) && (cli >= 1) && (cli < (rep[r1].size() - 1)) && (pos >= 1) && (pos < (rep[r2].size() - 1));

			//Compatibility
			bool notEmpty = (rep[r1].size() >= 3); //The minimum is two depots more one client
			bool is_Comp = hfmvrp->getComp(vType2, rep[r1][cli]);

			/*
			 cout << "newAds.cumulativeDemand" << ads.cumulativeDemand << endl;
			 cout << "newAds.maxDemand" << ads.maxDemand << endl;
			 cout << "newAds.minDemand" << ads.minDemand << endl;
			 cout << "newAds.maxPairDemand" << ads.maxPairDemand << endl;
			 cout << "newAds.minPairDemand" << ads.minPairDemand << endl;
			 cout << "newAds.sumDemand" << ads.sumDemand << endl;


			 if (hfmvrp->demands[rep[r1][cli]] < ads.minDemand[r1])
			 {
			 cout << "BUGGGGGG!!" << endl;
			 cout << "r1 = " << r1 << endl;
			 cout << "cli = " << cli << endl;
			 cout<<"rep[r1][cli] = "<<rep[r1][cli]<<endl;
			 cout<<"hfmvrp->demands[rep[r1][cli]] = "<<hfmvrp->demands[rep[r1][cli]]<<endl;
			 cout<<"ads.minDemand[r1] = "<<ads.minDemand[r1]<<endl;
			 cout<<"ads.maxDemand[r1] = "<<ads.maxDemand[r1]<<endl;
			 cout<<"ads.cumulativeDemand[r1] = "<<ads.cumulativeDemand[r1]<<endl;
			 getchar();
			 }			 */
			bool capR2 = ((hfmvrp->demands[rep[r1][cli]] + ads.sumDemand[r2]) <= hfmvrp->getVehicleTypeCap(vType2));

			//If one of the routes is out of optimum (value 0), the variable localOptimum has its value equal to 1
//			bool localOptimum = !((ads.neighborhoodStatus.find(idComponent())->second)[r1] && (ads.neighborhoodStatus.find(idComponent())->second)[r2]);

			return all_positive && is_Comp && capR2 && (int(rep.size()) >= 2) && notEmpty;
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
			ads.neighborhoodStatus[iter->first][r1] = false;
			ads.neighborhoodStatus[iter->first][r2] = false;
		}
	}

	void updateModifiedRoutes(RepHFMVRP& rep, AdsHFMVRP& ads)
	{
		vector<int> modRoutes; //modified routes
		modRoutes.push_back(r1);
		modRoutes.push_back(r2);
		//A troca ja foi efetuada!!
		double demandV = hfmvrp->demands[rep[r2][pos]];
		ads.sumDemand[r1] -= demandV;
		ads.sumDemand[r2] += demandV;

		for (int iter = 0; iter < modRoutes.size(); iter++)
		{
			int r = modRoutes[iter];
			vector<int> route = rep[r];
			int routeSize = route.size();
			if (routeSize >= 3) //two depots plus one client
			{
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
					minPairDemand = 0;
					maxPairDemand = 0;
				}

				ads.maxPairDemand[r] = maxPairDemand;
				ads.minPairDemand[r] = minPairDemand;

				//minDemand and maxDemand Calculus
				vector<pair<int, double> > rankDemands;
				for (int i = 1; i < int(routeSize - 1); i++)
					rankDemands.push_back(make_pair(i, routeDemands[i]));

				sort(rankDemands.begin(), rankDemands.end(), compare);

				//cout << rankDemands << endl;

				ads.maxDemand[r] = rankDemands[0].second;
				ads.minDemand[r] = rankDemands[rankDemands.size() - 1].second;

			}
			else
			{
				ads.cumulativeDemand[r].clear(); //TODO leak memoria
				ads.maxDemand[r] = 0;
				ads.minDemand[r] = 0;
				ads.maxPairDemand[r] = 0;
				ads.minPairDemand[r] = 0;
				//multi-trip vehicle is now non-needed
				/*
				if (r > hfmvrp->nVehicles)
				{
					cout << "DELETANDO ROTA" << endl;
					getchar();
					ads.cumulativeDemand.erase(ads.cumulativeDemand.begin() + r);
					ads.maxDemand.erase(ads.maxDemand.begin() + r);
					ads.minDemand.erase(ads.minDemand.begin() + r);
					ads.maxPairDemand.erase(ads.maxPairDemand.begin() + r);
					ads.minPairDemand.erase(ads.minPairDemand.begin() + r);
				}
				*/

			}
		}
	}
	Move<RepHFMVRP, AdsHFMVRP>* apply(RepHFMVRP& rep, AdsHFMVRP& ads)
	{
		//getting client
		int c = rep.at(r1).at(cli);

		//removendo os clientes de cada rota
		rep.at(r1).erase(rep.at(r1).begin() + cli);

		//fazendo a inserção
		rep.at(r2).insert(rep.at(r2).begin() + pos, c);

		/*
		 cout << "Route1[";
		 for (int i = 0; i < rep[r1].size(); i++)
		 cout << rep[r1][i] << ",";
		 cout << "]" << endl;
		 cout << "Route2[";
		 for (int i = 0; i < rep[r2].size(); i++)
		 cout << rep[r2][i] << ",";
		 cout << "]" << endl;
		 */
		//Update ADS
		updateNeighStatus(ads);

		//Update minDemand,maxDemand, minPairDemand, maxPairDemand, cumulative and sum
		updateModifiedRoutes(rep, ads);

		return new DeltaMoveVRPShift10(r2, r1, pos, cli, hfmvrp);
	}

	MoveCost* cost(const Evaluation&, const RepHFMVRP& rep, const AdsHFMVRP& ads)
	{
		vector<int> route1 = rep[r1];
		vector<int> route2 = rep[r2];

		int routeSize1 = route1.size();
		int routeSize2 = route2.size();

		double vTDC1 = hfmvrp->getVehicleTypeDistCost(vType1);
		double vTDC2 = hfmvrp->getVehicleTypeDistCost(vType2);

		unsigned k1 = cli;
		unsigned k2 = pos;

		// before k1 and k2
		unsigned bk1 = k1 - 1;
		unsigned bk2 = k2 - 1;

		// after k1 and k2
		unsigned ak1 = k1 + 1;
		unsigned ak2 = k2 + 1;

		double f = 0;
		/*
		 cout << "route1[" << bk1 << "[ = " << route1[bk1] << endl;
		 cout << "route1[" << k1 << "[ = " << route1[k1] << endl;
		 cout << "route1[" << ak1 << "[ = " << route1[ak1] << endl;
		 cout << "route2[" << bk2 << "[ = " << route2[bk2] << endl;
		 cout << "route2[" << k2 << "[ = " << route2[k2] << endl;
		 cout << "route2[" << ak2 << "[ = " << route2[ak2] << endl << endl;
		 cout << "getDist(0,0) : =" << hfmvrp->getDist(0, 0) << endl;
		 */
		f -= hfmvrp->getDist(route1[bk1], route1[k1]) * vTDC1;
		//cout << "-d(" << route[bk1] << "," << route[k1] << ") \t f= " << f << endl;
		f -= hfmvrp->getDist(route1[k1], route1[ak1]) * vTDC1;
		//cout << "-d(" << route[k1] << "," << route[ak1] << ") \t f= " << f << endl;
		f -= hfmvrp->getDist(route2[bk2], route2[k2]) * vTDC2;
		//cout << "-d(" << route[k2] << "," << route[ak2] << ") \t f= " << f << endl;


		f += hfmvrp->getDist(route1[bk1], route1[ak1]) * vTDC1;
		//cout << "+d(" << route[bk1] << "," << route[k2] << ") \t f= " << f << endl;
		f += hfmvrp->getDist(route2[bk2], route1[k1]) * vTDC2;
		//cout << "+d(" << route[k2] << "," << route[ak1] << ") \t f= " << f << endl;
		f += hfmvrp->getDist(route1[k1], route2[k2]) * vTDC2;
		//cout << "+d(" << route[bk2] << "," << route[k1] << ") \t f= " << f << endl;

		if (routeSize1 <= 3)
		{
			f -= hfmvrp->vehiclesTypeFixedCost[vType1];
		}
		if (routeSize2 <= 2)
			f += hfmvrp->vehiclesTypeFixedCost[vType2];

		return new MoveCost (f, 0);
	}

	static string idComponent()
	{
		string idComp = super::idComponent();
		idComp.append(":DeltaMoveVRPShift10");
		return idComp;
	}

	virtual bool operator==(const Move<RepHFMVRP, AdsHFMVRP>& _m) const
	{
		const DeltaMoveVRPShift10& m1 = (const DeltaMoveVRPShift10&) _m;
		return ((r1 == m1.r1) && (r2 == m1.r2) && (cli == m1.cli) && (pos == m1.pos));
	}

	virtual void print() const
	{
		cout << "DeltaMoveVRPShift10( ";
		cout << r1 << " , ";
		cout << r2 << " , ";
		cout << cli << " , ";
		cout << pos << " )";
	}
};

}

#endif /*DELTA_MOVEVRPSHIFT1_0_HPP_*/
