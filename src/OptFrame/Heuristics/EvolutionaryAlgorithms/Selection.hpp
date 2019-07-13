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

#ifndef SELECTION_HPP_
#define SELECTION_HPP_

#include "../../MultiSolution.hpp"
#include "EA.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Selection : public Component, public EA
{
public:

	virtual ~Selection()
	{
	}

	virtual pair<unsigned, unsigned> select(const MultiSolution<R, ADS>& population, const MultiEvaluation& mev, const vector<double>& fv) = 0;


	static double getMax(const vector<double>& fv)
	{
		double lmax = -10000000;
		for (int i = 0; i < fv.size(); i++)
			if (fv[i] > lmax)
				lmax = fv[i];
		return lmax;
	}

	static double getSum(const vector<double>& fv)
	{
		double s = 0;
		for (int i = 0; i < fv.size(); i++)
			s += fv[i];
		return s;
	}

	static void normalize(vector<double>& fv)
	{
		double sum = getSum(fv);
		if(sum == 0)
			sum = 1;
		for (int i = 0; i < fv.size(); i++)
		{
			fv[i] = fv[i] / sum;
			if(fv[i] != fv[i]) // is nan
			{
				cout << "Selection::normalize()::NAN VALUE!" << endl;
				cout << fv << endl;
				exit(1);
			}
		}
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":" << EA::family() << ":Selection";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* SELECTION_HPP_ */
