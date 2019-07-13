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

#ifndef OPTFRAME_MANN_U_TEST_FUNCTION_HPP_
#define OPTFRAME_MANN_U_TEST_FUNCTION_HPP_

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
class StatisticsMannUTestFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~StatisticsMannUTestFunction()
	{
	}

	virtual string id()
	{
		return "statistics.mann_u_test";
	}

	virtual string usage()
	{
		return "statistics.mann_u_test( list1 list2 ) : return p-value\nindependent 2-group Mann-Whitney U Test => requires: data independently sampled\nnull hypothesis: data come from same distribution; if p-value < alpha reject null hypothesis.";
	}

	virtual string formatNumber(double v)
	{
		stringstream ss;
		ss << v;
		return ss.str();
	}

	virtual string* run(vector<PreprocessFunction<R,ADS,DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, const map< string, string >& dictionary, const map< string,vector<string> >& ldictionary, string body)
	{
		Scanner scanner(body);

		vector<string>* plist1 = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list1;
		if(plist1)
		{
			list1 = vector<string>(*plist1);
			delete plist1;
		}
		else
			return nullptr;

		if(list1.size()==0)
			return nullptr;

		vector<string>* plist2 = OptFrameList::readList(ldictionary, scanner);
		vector<string>  list2;
		if(plist2)
		{
			list2 = vector<string>(*plist2);
			delete plist2;
		}
		else
			return nullptr;

		if(list2.size()==0)
			return nullptr;

		// CARE WITH TIES!
		bool jitter = false;

		for(unsigned i=0; i<list1.size(); i++)
			if((i<list2.size()) && (Scanner::parseDouble(list1[i]) == Scanner::parseDouble(list2[i])))
			{
				jitter = true;
				break;
			}

		stringstream scommand;
		scommand << "echo \"wilcox.test( x=c(";

		for(unsigned i=0; i<list1.size(); i++)
		{
			scommand << list1.at(i);
			if(i != list1.size()-1)
				scommand << ",";
		}

		scommand << "), y=";
		if(jitter)
			scommand << "jitter(";
		scommand << "c(";

		for(unsigned i=0; i<list2.size(); i++)
		{
			scommand << list2.at(i);
			if(i != list2.size()-1)
				scommand << ",";
		}

		scommand << ")";
		if(jitter)
			scommand << ")";

		scommand << " )\" | R --no-save | grep p-value";

		//cout << scommand.str() << endl;
		//if(jitter)
		//	cout << "NEEDED JITTER!" << endl;

		FILE* pPipe = popen(scommand.str().c_str(), "r");
		if (pPipe == nullptr)
		{
		    cout << "mann_u_test function: PIPE NOT OPEN!" << endl;
		    return nullptr;
		}

		char line[100];

		string output = "";

		while(fgets(line, 100, pPipe) != nullptr)
		{
			string sline = line;
			output.append(sline);
		}

		pclose(pPipe);

		//cout << "mann_u_test function: OUTPUT '" << output << "'" << endl;
		if(output=="") // POSSIBLE ERROR: 'sh: R: not found'
			return nullptr;

		Scanner scan_out(output); //example: 'W = 450, p-value = 1'
		scan_out.next(); // drop 'W'
		scan_out.next(); // drop '='
		scan_out.next(); // drop '450,'
		scan_out.next(); // drop 'p-value'
		scan_out.next(); // drop '='
		// WARNING: p-value can be 'NA'
		string spvalue = scan_out.next();
		double pvalue;
		if(spvalue == "NA")
		{
			cout << "mann_u_test function warning: returning 'NA' result! p-value = 1.0" << endl;
			pvalue = 1;
		}
		else
			pvalue = Scanner::parseDouble(spvalue);

		return new string(formatNumber(pvalue));
	}
};

}

#endif /* OPTFRAME_MANN_U_TEST_FUNCTION_HPP_ */
