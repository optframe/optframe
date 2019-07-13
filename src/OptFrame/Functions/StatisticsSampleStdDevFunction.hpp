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

#ifndef OPTFRAME_SAMPLE_STANDARD_DEVIATION_FUNCTION_HPP_
#define OPTFRAME_SAMPLE_STANDARD_DEVIATION_FUNCTION_HPP_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "../Scanner++/Scanner.h"

#include "../PreprocessFunction.hpp"

#include "../OptFrameList.hpp"

#include <algorithm>

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class StatisticsSampleStdDevFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~StatisticsSampleStdDevFunction()
	{
	}

	virtual string id()
	{
		return "statistics.sample_stddev";
	}

	virtual string usage()
	{
		return "statistics.sample_stddev( list ) : return list sample standard deviation (with N-1 degrees of freedom)";
	}

	virtual string* run(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scanner(body);

		vector<string>* plist = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list;
		if(plist)
		{
			list = vector<string>(*plist);
			delete plist;
		}
		else
			return nullptr;

		if(list.size()==0)
			return nullptr;

		int len = list.size();

		double sum = 0;
		for(unsigned i=0; i<list.size(); i++)
			sum += Scanner::parseDouble(list[i]);

		double avg = sum/len;

		sum = 0;
		for(unsigned i=0; i<list.size(); i++)
		{
			double diff = Scanner::parseDouble(list[i]) - avg;
			sum += (diff*diff);
		}

		stringstream ss;
		ss << sqrt(sum/(len-1)); // N-1 degrees of freedom

		string sample_stddev = ss.str();

		return new string(sample_stddev);
	}
};

}

#endif /* OPTFRAME_SAMPLE_STANDARD_DEVIATION_FUNCTION_HPP_ */
