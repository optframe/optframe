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

#ifndef HELPMODULE_HPP_
#define HELPMODULE_HPP_

#include "../Command.hpp"

using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemHelpCommand : public Command<R, ADS, DS>
{
public:

	virtual ~SystemHelpCommand()
	{
	}

	string id()
	{
		return "system.help";
	}
	string usage()
	{
		return "system.help";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& all_functions, HeuristicFactory<R, ADS, DS>&, map<string,string>&, map< string,vector<string> >&, string)
	{
		cout << "Available modules are:" << endl;
		for(unsigned int i=0;i<all_modules.size();i++)
		{
			cout << all_modules[i]->id();
			if(all_modules[i]->handles.size() > 0)
				cout << " =>";
			for(unsigned h=0; h<all_modules[i]->handles.size(); h++)
				cout << "\t" << all_modules[i]->handles[h] << " ";
			cout << endl;
		}

		cout << "Available functions are:" << endl;
		for(unsigned int i=0;i<all_functions.size();i++)
		{
			cout << all_functions[i]->id();
			if(all_functions[i]->handles.size() > 0)
				cout << " =>";
			for(unsigned h=0; h<all_functions[i]->handles.size(); h++)
				cout << "\t" << all_functions[i]->handles[h];
			cout << endl;
		}

		cout << "Type 'system.usage module_name' to learn how to use the command." << endl;

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* HELPMODULE_HPP_ */
