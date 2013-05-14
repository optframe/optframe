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

#ifndef OPTFRAME_COMPONENT_BETTER_THAN_HPP_
#define OPTFRAME_COMPONENT_BETTER_THAN_HPP_

#include "../Module.hpp"

#include "SystemSilentDefineModule.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentBetterThanModule: public Module<R, ADS, DS>
{
public:

	virtual ~ComponentBetterThanModule()
	{
	}

	string id()
	{
		return "component.better_than";
	}

	string usage()
	{
		string u = "component.better_than OptFrame:Evaluator id value_1 value_2 store_variable";
		return u;
	}

	string formatBool(bool b)
	{
		if(b)
			return "true";
		else
			return "false";
	}

	bool run(vector<Module<R, ADS, DS>*>& allModules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "component.better_than: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		Evaluator<R, ADS, DS>* eval = factory.read_ev(scanner);

		if(!eval)
		{
			cout << "module " << id() << " error: no evaluator!" << endl;
			return false;
		}

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		double value1 = scanner.nextDouble();

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		double value2 = scanner.nextDouble();

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string variable = scanner.next();

		string result = formatBool(eval->betterThan(value1, value2));

		return Module<R, ADS, DS>::defineText(variable, result, dictionary);
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Module<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* OPTFRAME_COMPONENT_BETTER_THAN_HPP_ */
