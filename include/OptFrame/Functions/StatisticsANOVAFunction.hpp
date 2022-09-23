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

#ifndef OPTFRAME_ANOVA_FUNCTION_HPP_
#define OPTFRAME_ANOVA_FUNCTION_HPP_

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
class StatisticsANOVAFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~StatisticsANOVAFunction()
	{
	}

	virtual string id()
	{
		return "statistics.anova";
	}

	virtual string usage()
	{
		return "statistics.anova( list_of_list ) : return p-value\nnull hypothesis: no difference between values, if p-value < alpha reject null hypothesis.";
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

		stringstream scommand;
		scommand << "echo \"";

		stringstream ssx;
		ssx << "xv <- c(";

		unsigned nrows = list2.at(0).size();
		unsigned ncols = list2.size();

		for(unsigned i=0; i<ncols; i++)
			for(unsigned j=0; j<nrows; j++)
			{
				ssx << list2.at(i).at(j);

				if(!( (j == nrows-1) && (i == ncols-1)) )
					ssx << ",";
			}

		ssx << ")";

		//cout << "SSX: " << ssx.str() << endl;

		stringstream ssf;
		ssf << "fv <- factor(c(";

		for(unsigned i=0; i<ncols; i++)
			for(unsigned j=0; j<nrows; j++)
			{
				ssf << "'T"<<i<<"'";
				if(!( (j == nrows-1) && (i == ncols-1)) )
					ssf << ",";
			}

		ssf << "))";

		//cout << "SSF: " << ssf.str() << endl;

		scommand << ssx.str() << '\n' << ssf.str() << '\n';

		scommand << "summary(aov(xv ~ fv))\" | R --no-save | grep fv";

		//cout << "COMMAND: '" << scommand.str() << "'" << endl;

		FILE* pPipe = popen(scommand.str().c_str(), "r");
		if (pPipe == nullptr)
		{
		    cout << "anova function: PIPE NOT OPEN!" << endl;
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

		//cout << "anova function: OUTPUT '" << output << "'" << endl;
		if(output=="") // POSSIBLE ERROR: 'sh: R: not found'
			return nullptr;

		Scanner scan_out(output);
		//example: '> fv <- ... all values
		// > summary(aov(xv ~ fv))
		//fv           1  31088   31088   0.199  0.671
		//'
		scan_out.nextLine(); // drop '> fv <- ... all values'
		scan_out.nextLine(); // drop '> summary(aov(xv ~ fv))'
		scan_out.next(); // drop 'fv'
		scan_out.next(); // drop '1'
		scan_out.next(); // drop '31088,'
		scan_out.next(); // drop '31088'
		scan_out.next(); // drop '0.199'
		// WARNING: p-value can be 'NA' (NOT SURE, BUT FOR SAFETY...)
		string spvalue = scan_out.next();
		double pvalue;
		if(spvalue == "NA")
		{
			cout << "anova function warning: returning 'NA' result! p-value = 1.0" << endl;
			pvalue = 1;
		}
		else
			pvalue = Scanner::parseDouble(spvalue);

		return new string(formatNumber(pvalue));
	}
};

}

#endif /* OPTFRAME_ANOVA_FUNCTION_HPP_ */
