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

#ifndef CREATE_NUMERIC_LIST_MODULE_HPP_
#define CREATE_NUMERIC_LIST_MODULE_HPP_

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CreateNumericListCommand: public Command<R, ADS, DS>
{
public:

	virtual ~CreateNumericListCommand()
	{
	}

	string id()
	{
		return "create_numeric_list";
	}
	string usage()
	{
		return "create_numeric_list begin end list_name";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		int begin    = scanner.nextInt();
		int end      = scanner.nextInt();
		string lname = scanner.next();

		stringstream ss;

		ss << lname << " [";
		for(int i=begin; i<end; i++)
			ss << i << ",";
		if((end-begin)>=0)
			ss << end;
		ss << " ]";

		return Command<R, ADS, DS>::run_module("list.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* CREATE_NUMERIC_LIST_MODULE_HPP_ */
