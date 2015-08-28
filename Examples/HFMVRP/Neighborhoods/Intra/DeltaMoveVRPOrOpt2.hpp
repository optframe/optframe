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

#ifndef DELTA_MOVEVRPOR2OPT_HPP_
#define DELTA_MOVEVRPOR2OPT_HPP_

#include "../../../../OptFrame/Util/NeighborhoodStructures/VRP/Intra/NSSeqVRPOrOpt2.hpp"

#include <cmath>

using namespace std;

namespace HFMVRP
{

class DeltaMoveVRPOrOpt2: public MoveVRPOrOpt2<int, AdsHFMVRP>
{
	typedef MoveVRPOrOpt2<int, AdsHFMVRP> super;

private:
	ProblemInstance* hfmvrp;
	int k;

public:

	DeltaMoveVRPOrOpt2(int _r, int _c, int _pos, ProblemInstance* _hfmvrp) :
		super(_r, _c, _pos), hfmvrp(_hfmvrp)
	{
		k = 2; //OrOpt1
		if (!_hfmvrp)
		{
			cout << "Error: hfmvrp problem is NULL!" << endl;
			print();
			exit(1);
		}
	}

	virtual ~DeltaMoveVRPOrOpt2()
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
			bool all_positive = (r >= 0) && (c >= 1) && (c < (((int) rep[r].size()) - 2)) && (pos >= 1) && (pos < (((int) rep[r].size()) - 2));
			bool minSize = (((int) rep[r].size()) >= 3);


			return all_positive && minSize && (c != pos) && (c + 1 != pos) && ((c - pos) >= k);
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
		//Update NeighStatus
		updateNeighStatus(ads);

		int aux;
		if (c < pos)
		{
			for (int i = c + 1; i < (pos - 1); i++)
			{
				aux = rep.at(r).at(i);
				rep.at(r).at(i) = rep.at(r).at(i + 1);
				rep.at(r).at(i + 1) = aux;
			}
			for (int i = c; i < (pos - 2); i++)
			{
				aux = rep.at(r).at(i);
				rep.at(r).at(i) = rep.at(r).at(i + 1);
				rep.at(r).at(i + 1) = aux;
			}

			//Update ADS
			//Update minDemand,maxDemand, minPairDemand, maxPairDemand, cumulative and sum
			updateModifiedRoutes(rep, ads);

			return new DeltaMoveVRPOrOpt2(r, pos - 2, c, hfmvrp);
		}
		else
		{
			for (int i = c; i > pos; i--)
			{
				aux = rep.at(r).at(i);
				rep.at(r).at(i) = rep.at(r).at(i - 1);
				rep.at(r).at(i - 1) = aux;
			}
			for (int i = c + 1; i > pos + 1; i--)
			{
				aux = rep.at(r).at(i);
				rep.at(r).at(i) = rep.at(r).at(i - 1);
				rep.at(r).at(i - 1) = aux;
			}

			//Update ADS
			//Update minDemand,maxDemand, minPairDemand, maxPairDemand, cumulative and sum
			updateModifiedRoutes(rep, ads);

			return new DeltaMoveVRPOrOpt2(r, pos, c + 2, hfmvrp);

		}

		return new DeltaMoveVRPOrOpt2(-1, -1, -1, hfmvrp);

	}

	MoveCost* cost(const Evaluation&, const RepHFMVRP& rep, const AdsHFMVRP& ads)
	{
		vector<int> route = rep[r];
		int routeSize = route.size();
		//cout << "routeSize = " << routeSize << endl;
		//cout << route << endl;
		//cout << "c = " << c << "\tpos = " << pos << endl;
		int vType = -1;
		if (r >= hfmvrp->nVehicles)
			vType = 2;//MultiTripVehicle
		else
			vType = hfmvrp->getVehicleType(r);

		double vTDC = hfmvrp->getVehicleTypeDistCost(vType);

		// before c and pos
		int bi = c - 1;
		int bj = pos - 1;

		if (bi < 0)
		{
			//cout << "BUGGGGGG============== bi = " << bi << endl;
			//getchar();
			bi = routeSize - 2;
		}
		if (bj < 0)
		{
			//cout << "BUGGGGGG================ bj = " << bj << endl;
			//getchar();
			bj = routeSize - 2;
		}

		// after j
		unsigned aj = pos + 1;

		if (aj >= routeSize)//depot at the for each route
		{
			aj = 1;
			//cout << "BUGGGGGG==================aj = " << aj << endl;
			//getchar();
		}

		// last i+k
		unsigned lik = c + k - 1;

		// after i+k
		unsigned aik = c + k;

		if (aik >= routeSize)
		{
			aik = 1;
			//cout << "BUGGGGGG=================aik = " << aik << endl;
			//getchar();
		}

		double f = 0;

		if (((c == routeSize - 2) && (pos == 0)) || ((c == 0) && (pos == routeSize - 2))) // extreme point
		{
			f = 0;
		}
		else
		{
			if (c < pos)
			{
				f -= hfmvrp->getDist(route[bi], route[c]) * vTDC;
				//cout << "-d(" << rep[r][bi] << "," << rep[r][c] << ") \t f= " << f << endl;
				f -= hfmvrp->getDist(route[lik], route[aik]) * vTDC;
				//cout << "-d(" << rep[r][lik] << "," << rep[r][aik] << ") \t f= " << f << endl;
				f -= hfmvrp->getDist(route[pos], route[aj]) * vTDC;
				//cout << "-d(" << rep[r][pos] << "," << rep[r][aj] << ") \t f= " << f << endl;

				f += hfmvrp->getDist(route[bi], route[aik]) * vTDC;
				//cout << "+d(" << rep[r][bi] << "," << rep[r][aik] << ") \t f= " << f << endl;
				f += hfmvrp->getDist(route[pos], route[c]) * vTDC;
				//cout << "+d(" << rep[r][pos] << "," << rep[r][c] << ") \t f= " << f << endl;
				f += hfmvrp->getDist(route[lik], route[aj]) * vTDC;
				//cout << "+d(" << rep[r][lik] << "," << rep[r][aj] << ") \t f= " << f << endl;
			}
			else // c > pos
			{
				f -= hfmvrp->getDist(route[bi], route[c]) * vTDC;
				//cout << "-d(" << rep[r][bi] << "," << rep[r][c] << ") \t f= " << f << endl;
				f -= hfmvrp->getDist(route[lik], route[aik]) * vTDC;
				//cout << "-d(" << rep[r][lik] << "," << rep[r][aik] << ") \t f= " << f << endl;
				f -= hfmvrp->getDist(route[bj], route[pos]) * vTDC;
				//cout << "-d(" << rep[r][bj] << "," << rep[r][pos] << ") \t f= " << f << endl;

				f += hfmvrp->getDist(route[bi], route[aik]) * vTDC;
				//cout << "+d(" << rep[r][bi] << "," << rep[r][aik] << ") \t f= " << f << endl;
				f += hfmvrp->getDist(route[bj], route[c]) * vTDC;
				//cout << "+d(" << rep[r][bj] << "," << rep[r][c] << ") \t f= " << f << endl;
				f += hfmvrp->getDist(route[lik], route[pos]) * vTDC;
				//cout << "+d(" << rep[r][lik] << "," << rep[r][pos] << ") \t f= " << f << endl;
			}
		}

		return new MoveCost (f, 0);
	}

	static string idComponent()
	{
		string idComp = super::idComponent();
		idComp.append(":DeltaMoveVRPOrOpt2");
		return idComp;
	}

	virtual bool operator==(const Move<RepHFMVRP, AdsHFMVRP>& _m) const
	{
		const DeltaMoveVRPOrOpt2& m1 = (const DeltaMoveVRPOrOpt2&) _m;
		return (m1.r == r) && (m1.c == c) && (m1.pos == pos);
	}

	virtual void print() const
	{
		cout << "DeltaMoveVRPOrOpt2( route: " << r << " , " << c << " , " << pos << " )" << endl;
	}
};

}

#endif /*DELTA_MOVEVRPOR2OPT_HPP_*/
