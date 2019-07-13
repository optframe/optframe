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

#ifndef OPTFRAME_PLOT_2AXIS_MODULE_HPP_
#define OPTFRAME_PLOT_2AXIS_MODULE_HPP_

#include<string>

#include<algorithm>

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Plot2AxisCommand: public Command<R, ADS, DS>
{
public:

	virtual ~Plot2AxisCommand()
	{
	}

	string id()
	{
		return "plot.2axis";
	}

	string usage()
	{
		return "plot.2axis 1.list with: title xtitle ytitle 2.list of lists with: title, list of xvalues, list of yvalues 3.output file";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "plot.2axis module input: '" << input << "'" << endl;

		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "plot.2aix command: no such list!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string>  list1;
		vector<string>* p_list1 = OptFrameList::readList(ldictionary, scanner);
		if(p_list1)
		{
			list1 = vector<string>(*p_list1);
			delete p_list1;
		}
		else
			return false;

		if (list1.size() != 3)
		{
			cout << "plot.2axis command: expected 3 parameters (title, xtitle, ytitle) in first list and found " << list1.size() << endl;
			return false;
		}

		string title  = list1.at(0);
		string xtitle = list1.at(1);
		string ytitle = list1.at(2);

		if (!scanner.hasNext())
		{
			cout << "plot.2aix command: no such list!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string>  list2;
		vector<string>* p_list2 = OptFrameList::readList(ldictionary, scanner);
		if(p_list2)
		{
			list2 = vector<string>(*p_list2);
			delete p_list2;
		}
		else
			return false;

		string filename = scanner.rest();

		vector<string> lines;
		vector<pair<string, vector<vector<double> > > > files;

		for(unsigned i=0; i<list2.size(); i++)
		{
			Scanner scan(list2.at(i));

			vector<string>  list;
			vector<string>* p_list = OptFrameList::readList(ldictionary, scan);
			if(p_list)
			{
				list = vector<string>(*p_list);
				delete p_list;
			}
			else
				return false;

			if(list.size() != 3)
			{
				cout << "plot.2aix command: internal list size different from 3! expected name, xvalues and yvalues." << endl;
				cout << "Usage: " << usage() << endl;
				return false;
			}


			Scanner scan_x(list.at(1));
			vector<string>  list_x;
			vector<string>* p_list_x = OptFrameList::readList(ldictionary, scan_x);
			if(p_list_x)
			{
				list_x = vector<string>(*p_list_x);
				delete p_list_x;
			}
			else
				return false;


			Scanner scan_y(list.at(2));
			vector<string>  list_y;
			vector<string>* p_list_y = OptFrameList::readList(ldictionary, scan_y);
			if(p_list_y)
			{
				list_y = vector<string>(*p_list_y);
				delete p_list_y;
			}
			else
				return false;


			if(list_x.size() != list_y.size())
			{
				cout << "plot.2aix command: list_x size different from list_y size! (" << list_x.size() << " != " << list_y.size() << ")" << endl;
				cout << "Usage: " << usage() << endl;
				return false;
			}

			lines.push_back(list.at(0));
			stringstream ss;
			ss << "_gnuplot_dat_file_" << i << ".dat";
			pair<string, vector<vector<double> > > p(make_pair(ss.str(), vector<vector<double> >(list_x.size())));

			for(unsigned l=0; l<list_x.size(); l++)
			{
				try
				{
					p.second.at(l).push_back(Scanner::parseDouble(list_x.at(l)));
					p.second.at(l).push_back(Scanner::parseDouble(list_y.at(l)));
				}
				catch(ConversionError& e)
				{
					cout << "plot.2axis command: error parsing x or y value!" << endl;
					return false;
				}
			}

			FILE* f = fopen(p.first.c_str(), "w");
			if(f)
			{
				for(unsigned j=0; j<p.second.size(); j++)
					fprintf(f, "%.2f\t%.2f\n", p.second.at(j).at(0), p.second.at(j).at(1));
			}
			else
			{
				cout << "plot.2axis command: couldn't open file: '" << p.first << "'" << endl;
				return false;
			}

			fclose(f);

			files.push_back(p);
		}


		FILE* f = fopen("_gnuplot_script.pg", "w");

		fprintf(f, "reset\n");
		fprintf(f, "set terminal png\n");
		fprintf(f, "set xlabel \"%s\"\n", xtitle.c_str());
		fprintf(f, "set ylabel \"%s\"\n", ytitle.c_str());
		fprintf(f, "set title \"%s\"\n",  title.c_str() );
		fprintf(f, "set key reverse Left outside\n");
		fprintf(f, "set grid\n");
		fprintf(f, "set style data linespoints\n");

		fprintf(f, "plot ");

		for(unsigned i=0; i<lines.size(); i++)
		{
			fprintf(f,"\"_gnuplot_dat_file_%u.dat\" using 1:2 title \"%s\"", i, lines[i].c_str());
			if(i != (lines.size()-1))
				fprintf(f, ",");
		}

		fprintf(f, "\n");

		fclose(f);

		stringstream sexec;
		sexec << "gnuplot _gnuplot_script.pg > " << filename;

		//cout << "sexec: '" << sexec.str() << "'" << endl;

		int c = system(sexec.str().c_str());

		if(c < 0)
		{
			cout << "plot.2axis error: return is less than zero '" << c << "'" << endl;
			return false;
		}

		for(unsigned i=0; i<files.size(); i++)
		{
			stringstream ss;
			ss << "_gnuplot_dat_file_" << i << ".dat";
			remove(ss.str().c_str());
		}

		remove("_gnuplot_script.pg");

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* OPTFRAME_PLOT_2AXIS_MODULE_HPP_ */
