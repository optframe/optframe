// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef BINARY_SELECTION_HPP_
#define BINARY_SELECTION_HPP_

#include <assert.h>

#include "Selection.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class BinarySelection: public Selection<R, ADS>
{
private:
	RandGen& rg;

public:

	BinarySelection(RandGen& _rg) :
			rg(_rg)
	{
	}

	virtual ~BinarySelection()
	{
	}

	pair<unsigned, unsigned> select(const MultiSolution<R, ADS>& p, const MultiEvaluation& mev, const vector<double>& fv)
	{
		assert(p.size() > 1);
		//cout << "SELECT: " << p.size() << endl;
		//cout << fv << " = " << Selection<R, ADS>::getSum(fv) << endl;

		unsigned s1 = 0;

		float x = rg.rand01();
		//cout << "x=" << x << endl;
		for (unsigned i = 0; i < fv.size(); i++)
		{
			x -= fv[i];
			if (x <= 0)
			{
				s1 = i;
				break;
			}
		}

		unsigned s2 = s1;
		//cout << "s1 = " << s1 << endl;
		int trye = 0;
		while (s2 == s1)
		{
			trye++;
			if(trye >= 100*p.size())
			{
				cout << "LOOP IN BINARYSELECTION??" << endl;
				cout << fv << " = " << Selection<R, ADS>::getSum(fv) << endl;
				exit(1);
			}
			x = rg.rand01();
			//cout << "x2=" << x << endl;
			for (unsigned i = 0; i < fv.size(); i++)
			{
				x -= fv[i];
				if (x <= 0)
				{
					s2 = i;
					break;
				}
			}
		}

		return make_pair(s1, s2);
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Selection<R, ADS>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Selection<R, ADS>::idComponent() << ":BinarySelection";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* BINARY_SELECTION_HPP_ */
