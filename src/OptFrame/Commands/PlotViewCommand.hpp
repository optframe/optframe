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

#ifndef OPTFRAME_PLOT_VIEW_MODULE_HPP_
#define OPTFRAME_PLOT_VIEW_MODULE_HPP_

#include<string>

#include<algorithm>

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class PlotViewCommand: public Command<R, ADS, DS>
{
public:

	virtual ~PlotViewCommand()
	{
	}

	string id()
	{
		return "plot.view";
	}

	string usage()
	{
		return "plot.view file";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "plot.view module input: '" << input << "'" << endl;

		// Software: Eye of Gnome
		string view_software = "eog";

		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "plot.view command: no filename!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string filename = scanner.rest();

		stringstream ssdisp;
		ssdisp << view_software << " " << filename << " &"; // detach application

		int c = system(ssdisp.str().c_str());

		if(c < 0)
		{
			cout << "plot.view error: return is less than zero '" << c << "'" << endl;
			return false;
		}

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* OPTFRAME_PLOT_VIEW_MODULE_HPP_ */
