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

#ifndef OPTFRAME_FRIEDMAN_TEST_NOBLOCK_FUNCTION_HPP_
#define OPTFRAME_FRIEDMAN_TEST_NOBLOCK_FUNCTION_HPP_

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
class StatisticsFriedmanNoBlockTestFunction : public PreprocessFunction<R,ADS,DS>
{
public:

	virtual ~StatisticsFriedmanNoBlockTestFunction()
	{
	}

	virtual string id()
	{
		return "statistics.friedman_noblock_test";
	}

	virtual string usage()
	{
		return "statistics.friedman_noblock_test( list_of_list ) : return p-value\nnull hypothesis: values are from same distribution, if p-value < alpha reject null hypothesis.";
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
        {
            cout << id() << " function: list of values is invalid!" << endl;
			return nullptr;
        }

		if(list.size()==0)
        {
            cout << id() << " function: list of values is empty!" << endl;
			return nullptr;
        }

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
            {
                cout << id() << " function: internal list of values couldn't be read!" << endl;
				return nullptr;
            }

			list2.push_back(list1);
		}

		//cout << list2 << endl;

		unsigned nrows = list2.at(0).size();
		unsigned ncols = list2.size();

        unsigned LIMIT = 1000;

		stringstream scommand;
        FILE* aux = nullptr;

        if(nrows*ncols > LIMIT) // TOO BIG! USING FILE TO TRANSPORT DATA
        {
            cout << id() << " warning: too many data (" << (nrows*ncols) << " > " << LIMIT << ")! Will use 'optframe-friedman.aux' file to transport data." << endl;
            aux = fopen("optframe-friedman.aux", "w");
            fprintf(aux, "x <- matrix(c(");
        }
        else
       		scommand << "echo \"x <- matrix(c(";

		for(unsigned j=0; j<nrows; j++)
			for(unsigned i=0; i<ncols; i++)
			{
                string v = list2.at(i).at(j);
                if(nrows*ncols > LIMIT)
                    fprintf(aux, "%s", v.c_str());
                else
                    scommand << v;

				if(!( (j == nrows-1) && (i == ncols-1)) )
                {
                    if(nrows*ncols > LIMIT)
                        fprintf(aux, ",");
                    else
					    scommand << ",";
                }
			}

        if(nrows*ncols > LIMIT)
        {
		    fprintf(aux, "), nrow=%d, byrow=TRUE, ", nrows);
		    fprintf(aux, "dimnames = list(1 : %d, c(", nrows);
        }
        else
        {
		    scommand << "), nrow=" << nrows << ", byrow=TRUE, ";
		    scommand << "dimnames = list(1 : "<< nrows << ", c(";
        }

		for(unsigned i=0; i<ncols; i++)
		{
            if(nrows*ncols > LIMIT)
                fprintf(aux, "\'T%d\'",i);
            else
			    scommand << '\'' << 'T' << i << '\'';

			if(i != ncols-1)
            {
                if(nrows*ncols > LIMIT)
                    fprintf(aux, ",");
                else
				    scommand << ',';
            }
		}

        if(nrows*ncols > LIMIT)
        {
            fprintf(aux, ")))\nfriedman.test(x)\n");
            fclose(aux);

            scommand << "R --slave --no-save -f optframe-friedman.aux | grep p-value";
        }
        else
        {
		    scommand << ")))\n";
    		scommand << "friedman.test(x)\" | R --slave --no-save | grep p-value";
        }

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

        FILE* log = fopen("friedman.log","a");
        fprintf(log, "%s\n", scommand.str().c_str());
        fclose(log);

		//cout << "friedman_test function: OUTPUT '" << output << "'" << endl;
		if(output=="") // POSSIBLE ERROR: 'sh: R: not found'
        {
            cout << id() << " function: possible error, no R!" << endl;
			return nullptr;
        }

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

        log = fopen("friedman.log","a");
        fprintf(log, "pvalue=%.4f\n", pvalue);
        fclose(log);

		return new string(formatNumber(pvalue));
	}
};

}

#endif /* OPTFRAME_FRIEDMAN_TEST_NOBLOCK_FUNCTION_HPP_ */
