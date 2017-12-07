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

#ifndef OPTFRAME_WILCOXON_TEST_FUNCTION_HPP_
#define OPTFRAME_WILCOXON_TEST_FUNCTION_HPP_

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
class StatisticsWilcoxonTestFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~StatisticsWilcoxonTestFunction()
	{
	}

	virtual string id()
	{
		return "statistics.wilcoxon_test";
	}

	virtual string usage()
	{
		return "statistics.wilcoxon_test( list1 list2 ) : return p-value\ndependent 2-group Wilcoxon Signed Rank Test  => requires: data sampled in pairs\nnull hypothesis: data come from same distributions (after treatment), if p-value < alpha reject null hypothesis.";
		//return "wilcoxon_test( list1 signal list2 ) : return p-value\ndependent 2-group Wilcoxon Signed Rank Test  => requires: data sampled in pairs\nnull hypothesis: data come from same distributions (after treatment), if p-value < alpha reject null hypothesis.\n'signal' can be '<', '>' or '=='";
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

		//string signal = scanner.next();

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

		if(list1.size() != list2.size())
		{
			cout << "wilcoxon_test function: lists should have same size!" << endl;
			return nullptr;
		}

		// CARE WITH CONSTANT LISTS IN A PAIRED TEST!
		bool jitter = false;

		for(unsigned i=0; i<list1.size(); i++)
			if(Scanner::parseDouble(list1[i]) == Scanner::parseDouble(list2[i]))
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
		scommand << ",";

		scommand << "paired=TRUE";

		/*
		scommand << ",alternative=";

		if(signal=="<")
			scommand << "'l'"; // less
		else if(signal==">")
			scommand << "'g'"; // greater
		else if(signal=="==")
			scommand << "'t'"; // two.sided
		else
		{
			cout << "wilcoxon function: unknown signal '" << signal << "'" << endl;
			return nullptr;
		}
		*/

		scommand << ")\" | R --no-save | grep p-value";

		//cout << scommand.str() << endl;
		//if(jitter)
		//	cout << "NEEDED JITTER!" << endl;

		FILE* pPipe = popen(scommand.str().c_str(), "r");
		if (pPipe == nullptr)
		{
		    cout << "wilcoxon_test function: PIPE NOT OPEN!" << endl;
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

		//cout << "wilcoxon_test function: OUTPUT '" << output << "'" << endl;
		if(output=="") // POSSIBLE ERROR: 'sh: R: not found'
			return nullptr;

		Scanner scan_out(output); //example: 'V = 0, p-value = 0.01992'
		scan_out.next(); // drop 'V'
		scan_out.next(); // drop '='
		scan_out.next(); // drop '0,'
		scan_out.next(); // drop 'p-value'
		scan_out.next(); // drop '='
		// WARNING: p-value can be 'NA'
		string spvalue = scan_out.next();
		double pvalue;
		if(spvalue == "NA")
		{
			cout << "wilcoxon function warning: returning 'NA' result! p-value = 1.0" << endl;
			pvalue = 1;
		}
		else
			pvalue = Scanner::parseDouble(spvalue);

		return new string(formatNumber(pvalue));
	}
};

}

#endif /* OPTFRAME_WILCOXON_TEST_FUNCTION_HPP_ */
