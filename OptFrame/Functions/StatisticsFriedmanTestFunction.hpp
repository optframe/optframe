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

#ifndef OPTFRAME_FRIEDMAN_TEST_FUNCTION_HPP_
#define OPTFRAME_FRIEDMAN_TEST_FUNCTION_HPP_

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
class StatisticsFriedmanTestFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~StatisticsFriedmanTestFunction()
	{
	}

	virtual string id()
	{
		return "statistics.friedman_test";
	}

	virtual string usage()
	{
		return "statistics.friedman_test( list list_of_list ) : return p-value\nnull hypothesis: values are from same distribution, if p-value < alpha reject null hypothesis.";
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

		vector<string>* plistb = OptFrameList::readList(ldictionary, scanner);
		vector<string>  listb;
		if(plistb)
		{
			listb = vector<string>(*plistb);
			delete plistb;
		}
		else
			return nullptr;

		if(listb.size()==0)
			return nullptr;

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

		vector<vector<string> > list2;
		for(unsigned i=0; i<list.size(); i++)
		{
			Scanner scan(list[i]);
			vector<string>* plist1 = OptFrameList::readList(ldictionary, scan);
			vector<string>  list1;
			if(plist1)
			{
				list1 = vector<string>(*plist1);
				delete plist1;
			}
			else
				return nullptr;

			list2.push_back(list1);
		}

		//cout << list2 << endl;

		if(list2.size()==0)
			return nullptr;

		unsigned sizes = list2.at(0).size();

		for(unsigned i=1; i<list2.size(); i++)
			if(list2.at(i).size() != sizes)
			{
				cout << "statistics.friedman_test error: different sizes!" << endl;
				return nullptr;
			}

		if(listb.size() != sizes)
		{
			cout << "statistics.friedman_test error: block with different size!" << endl;
			return nullptr;
		}

		stringstream scommand;
		scommand << "echo \"b = factor(c(";
		for(unsigned b=0; b<listb.size(); b++)
			for(unsigned i=0; i<list2.size(); i++)
			{
				scommand << "\'" << listb.at(b) << "\'";
				if(!((i == list2.size()-1) && (b == listb.size()-1)))
					scommand << ",";
			}
		scommand << "))\n";

		scommand << "g = factor(c(";
		for(unsigned g=0; g<sizes; g++)
			for(unsigned i=0; i<list2.size(); i++)
			{
				scommand << "\'T" << i << "\'";
				if(!((i == list2.size()-1) && (g == sizes-1)))
					scommand << ",";
			}
		scommand << "))\n";

		scommand << "v = c(";
		for(unsigned v=0; v<sizes; v++)
			for(unsigned i=0; i<list2.size(); i++)
			{
				scommand << list2.at(i).at(v);
				if(!((i == list2.size()-1) && (v == sizes-1)))
					scommand << ",";
			}
		scommand << ")\n";

		scommand << "friedman.test(v, g, b)\" | R --no-save | grep p-value";

		//cout << "COMMAND: '" << scommand.str() << "'" << endl;

		FILE* pPipe = popen(scommand.str().c_str(), "r");
		if (pPipe == nullptr)
		{
		    cout << "friedman_test command: PIPE NOT OPEN!" << endl;
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

		//cout << "friedman_test function: OUTPUT '" << output << "'" << endl;
		if(output=="") // POSSIBLE ERROR: 'sh: R: not found'
			return nullptr;

		Scanner scan_out(output); //example: 'Friedman chi-squared = 8, df = 2, p-value = 0.01832'
		scan_out.next(); // drop 'Friedman'
		scan_out.next(); // drop 'chi-squared'
		scan_out.next(); // drop '='
		scan_out.next(); // drop '8,'
		scan_out.next(); // drop 'df'
		scan_out.next(); // drop '='
		scan_out.next(); // drop '2,'
		scan_out.next(); // drop 'p-value'
		scan_out.next(); // drop '='
		// WARNING: p-value can be 'NA'
		string spvalue = scan_out.next();
		double pvalue;
		if(spvalue == "NA")
		{
			cout << "friedman function warning: returning 'NA' result! p-value = 1.0" << endl;
			pvalue = 1;
		}
		else
			pvalue = Scanner::parseDouble(spvalue);

		return new string(formatNumber(pvalue));
	}
};

}

#endif /* OPTFRAME_FRIEDMAN_TEST_FUNCTION_HPP_ */
