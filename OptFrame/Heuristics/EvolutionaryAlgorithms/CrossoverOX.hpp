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

#ifndef OPTFRAME_CROSSOVER_OX_HPP_
#define OPTFRAME_CROSSOVER_OX_HPP_

#include "../../Solution.hpp"
#include "../../Evaluation.hpp"
#include "GeneralCrossover.hpp"

namespace optframe
{

template<class ADS = OPTFRAME_DEFAULT_ADS>
class CrossoverOX: public GeneralCrossover<vector<int>, ADS>
{
protected:
	RandGen& rg;

public:

	CrossoverOX(RandGen& _rg) :
			rg(_rg)
	{
	}

	virtual ~CrossoverOX()
	{
	}

	virtual pair<Solution<vector<int>, ADS>*, Solution<vector<int>, ADS>*> cross(const Solution<vector<int>, ADS>& s1, const Solution<vector<int>, ADS>& s2)
	{
		pair<Solution<vector<int>, ADS>*, Solution<vector<int>, ADS>*> r(NULL, NULL);

		const vector<int>& r1 = s1.getR();
		const vector<int>& r2 = s2.getR();

		// impossible to cross!
		if((r1.size() != r2.size()) || (r1.size() < 4))
			return r;

		Solution<vector<int>, ADS>& ch1 = s1.clone();
		Solution<vector<int>, ADS>& ch2 = s2.clone();

		int i = r1.size() - 1;
		while(i >= (r1.size() - 1))
			i = rg.rand(r1.size());

		int j = i;
		while(j <= i)
			j = rg.rand(r1.size());

		// Okay! i < j and (j-i) >= 1 (size of at least 2)
		doCross(r1, r2, i, j, ch1.getR());
		doCross(r2, r1, i, j, ch2.getR());

		r.first  = &ch1;
		r.second = &ch2;

		return r;
	}

	// TODO: COMPARE!!
	//void doCross(const vector<int>& A, const vector<int>& B, int i, int j, vector<int>& child)
	//vector<bool> vmap(max(vmax(A), vmax(B))+1, false);
	void doCross(const vector<int>& A, const vector<int>& B, unsigned i, unsigned j, vector<int>& child)
	{
		vector<int> middle(A.begin() + i, A.begin() + j);

		child = B;

		for(unsigned k = 0; k < child.size(); k++)
			if(in(child[k], middle))
				child[k] = -1;

		vector<int> list;
		for(unsigned k = i; k < child.size(); k++)
			if(child[k] != -1)
				list.push_back(child[k]);
		for(unsigned k = 0; k < i; k++)
			if(child[k] != -1)
				list.push_back(child[k]);

		child.clear();
		child.insert(child.end(), list.begin(), list.begin() + i);
		child.insert(child.end(), middle.begin(), middle.end());
		child.insert(child.end(), list.begin() + i, list.end());
	}

	bool in(int k, const vector<int>& v)
	{
		for(unsigned i = 0; i < v.size(); i++)
			if(v[i] == k)
				return true;
		return false;
	}

	int vmax(const vector<int>& v)
	{
		int m = v.at(0);
		for(unsigned i = 1; i < v.size(); i++)
			if(v[i] > m)
				m = v[i];
		return m;
	}

	void classicTest()
	{
		cout << "========== OX test begins ==========" << endl;
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

		vector<int> r;
		doCross(v1, v2, 3, 6, r);

		cout << "v1=" << v1 << endl;
		cout << "v2=" << v2 << endl;
		cout << "r=" << r << endl;
		cout << "========= OX test finished =========" << endl;
	}

};

}

#endif /*OPTFRAME_CROSSOVER_OX_HPP_*/
