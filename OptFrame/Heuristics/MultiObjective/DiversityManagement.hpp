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

#ifndef OPTFRAME_DIVERSITY_MANAGEMENT_HPP_
#define OPTFRAME_DIVERSITY_MANAGEMENT_HPP_

#include "MOSIndividual.hpp"
#include "../../Component.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class DiversityManagement: Component
{
public:

	DiversityManagement()
	{
	}

	virtual ~DiversityManagement()
	{
	}

	virtual void assignDiversity(vector<MOSIndividual<R, ADS, DS>*>& p) = 0;

	virtual void print() const
	{
		cout << "DiversityManagement" << endl;
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CrowdingDistance: public DiversityManagement<R, ADS, DS>
{
public:

	vector<Direction<DS>*> vDir;

	CrowdingDistance(vector<Direction<DS>*>& _vDir) :
			vDir(_vDir)
	{
	}

	virtual ~CrowdingDistance()
	{
	}

	static bool compare(const pair<double, int>& p1, const pair<double, int>& p2)
	{
		return p1.first < p2.first;
	}

	virtual void assignDiversity(vector<MOSIndividual<R, ADS, DS>*>& I)
	{
		int max_rank = 0;
		for(unsigned i = 0; i < I.size(); i++)
			if(I[i]->fitness > max_rank)
				max_rank = I[i]->fitness;
		unsigned nranks = max_rank + 1; // include 0

		int l = I.size();
		if(l == 0)
			return;

		// for each 'i', ...
		for(unsigned i = 0; i < I.size(); i++)
		{
			// ... set I[i].distance = 0
			I[i]->diversity = 0;
		}

		// for each objective 'm'
		for(unsigned m = 0; m < vDir.size(); m++)
		{
			for(unsigned r = 0; r < nranks; r++)
			{
				// I = sort(I, m)
				vector<pair<double, int> > fitness;  // (fitness, id)

				for(int i = 0; i < I.size(); i++)
					if(I[i]->fitness == r)
					{
						double fit = I[i]->mev.at(m).evaluation();
						fitness.push_back(make_pair(fit, i));
					}

				sort(fitness.begin(), fitness.end(), compare);

				// I[1] dist = I[l] dist = 'infinity'
				// ADAPTATION
				/*
				 r[fitness[0].second]->diversity = numeric_limits<double>::infinity();
				 r[fitness[l - 1].second]->diversity = numeric_limits<double>::infinity();
				 */

				////cout << "ORDER: (rank=" << r << ") " << fitness << endl;
				// for i=2 to l-1
				// ADAPTATION WITH ANOTHER LOOP
				for(int k = 0; k < fitness.size(); k++)
				{
					int i = fitness[k].second;

					int idx_before = -1;
					for(int e = k - 1; e >= 0; e--)
						if(&I[i]->s != &I[fitness[e].second]->s)
						{
							idx_before = e;
							break;
						}

					if(idx_before == -1)
					{
						I[i]->diversity += numeric_limits<double>::infinity();
						continue;
					}

					int idx_after = -1;
					for(int e = k + 1; e < fitness.size(); e++)
						if(&I[i]->s != &I[fitness[e].second]->s)
						{
							idx_after = e;
							break;
						}

					if(idx_after == -1)
					{
						I[i]->diversity += numeric_limits<double>::infinity();
						continue;
					}

					// I[i] dist += (I[i+1].m - I[i-1].m)/(fmax_m - fmin_m)
					// ADAPTATION
					I[i]->diversity += abs(fitness[idx_after].first - fitness[idx_before].first) / abs(vDir[m]->max() - vDir[m]->min());
				}

			} // for each front
		} // for each objective

		//MOSIndividual<R, ADS, DS>::updateDistances(I); // update parents' ranks
	} // end function

};

}

#endif /*OPTFRAME_DIVERSITY_MANAGEMENT_HPP_*/
