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

#ifndef OPTFRAME_CROSSOVER_PMX_HPP_
#define OPTFRAME_CROSSOVER_PMX_HPP_

#include "../../Solution.hpp"
#include "../../Evaluation.hpp"
#include "GeneralCrossover.hpp"

namespace optframe
{

template<class ADS = OPTFRAME_DEFAULT_ADS>
class CrossoverPMX: public GeneralCrossover<vector<int>, ADS>
{
protected:
	RandGen& rg;

public:

	CrossoverPMX(RandGen& _rg) :
			rg(_rg)
	{
	}

	virtual ~CrossoverPMX()
	{
	}

	virtual pair<Solution<vector<int>, ADS>*, Solution<vector<int>, ADS>*> cross(const Solution<vector<int>, ADS>& s1, const Solution<vector<int>, ADS>& s2)
	{
		pair<Solution<vector<int>, ADS>*, Solution<vector<int>, ADS>*> r(NULL, NULL);

		const vector<int>& r1 = s1.getR();
		const vector<int>& r2 = s2.getR();

		// impossible to cross!
		if((r1.size() != r2.size()) || (r1.size() < 3))
			return r;

		Solution<vector<int>, ADS>& ch1 = s1.clone();
		Solution<vector<int>, ADS>& ch2 = s2.clone();

		int i = r1.size() - 1;
		int j = i;
		while((j <= i) || (j - i) == (r1.size() - 1))
			j = rg.rand(r1.size() + 1); // including end of vector

		// Okay! i < j and (j-i) >= 1 (size of at least 2)
		ch1.getR() = doCross(r1, r2, i, j);
		ch2.getR() = doCross(r2, r1, i, j);

		r.first = &ch1;
		r.second = &ch2;

		return r;
	}

	vector<int> doCross(const vector<int>& A, const vector<int>& B, unsigned i, unsigned j)
	{
		vector<int> middle(A.begin() + i, A.begin() + j);
		vector<int> middleB(B.begin() + i, B.begin() + j);

		//cout << "MIDDLE=" << middle << endl;
		//cout << "MIDDLEB=" << middleB << endl;

		// mark repeated elements with -1
		for(unsigned k = 0; k < middle.size(); k++)
			for(unsigned z = 0; z < middleB.size(); z++)
				if(middle[k] == middleB[z])
				{
					middle[k] = -1;
					middleB[z] = -1;
					break; // break first loop
				}

		// create a map of updates
		map<int, int> vmap_a;
		unsigned b1 = 0;
		for(unsigned k = 0; k < middle.size(); k++)
			if(middle[k] != -1)
				for(unsigned z = b1; z < middleB.size(); z++)
					if(middleB[z] != -1)
					{
						vmap_a[middle[k]] = middleB[z];
						b1 = z + 1;
						break;
					}

		// create child with duplications
		vector<int> child(B);
		for(unsigned k = i; k < j; k++)
			child[k] = A[k];

		//cout << "Bug child (B+middle)=" << child << endl;

		// fix using the map
		for(int k = ((int) child.size()) - 1; k >= 0; k--)
		{
			map<int, int>::iterator it = vmap_a.find(child[k]);
			if(it != vmap_a.end())
			{
				child[k] = it->second;
				vmap_a.erase(it);
			}
		}

		return child;
	}

	void classicTest()
	{
		cout << "========== PMX test begins ==========" << endl;
		vector<int> v1;
		v1.push_back(9);
		v1.push_back(8);
		v1.push_back(4);
		v1.push_back(5);
		v1.push_back(6);
		v1.push_back(7);
		v1.push_back(1);
		v1.push_back(3);
		v1.push_back(2);
		v1.push_back(0);
		vector<int> v2;
		v2.push_back(8);
		v2.push_back(7);
		v2.push_back(1);
		v2.push_back(2);
		v2.push_back(3);
		v2.push_back(0);
		v2.push_back(9);
		v2.push_back(5);
		v2.push_back(4);
		v2.push_back(6);

		vector<int> r = doCross(v1, v2, 3, 8);

		cout << "v1=" << v1 << endl;
		cout << "v2=" << v2 << endl;
		cout << "r=" << r << endl;

		vector<int> r2 = doCross(v2, v1, 3, 8);
		cout << "r2=" << r2 << endl;

		cout << "========= PMX test finished =========" << endl;
	}

};

}

#endif /*OPTFRAME_CROSSOVER_PMX_HPP_*/
