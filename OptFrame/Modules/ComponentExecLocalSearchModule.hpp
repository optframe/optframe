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

#ifndef COMPONENT_EXEC_LOCAL_SEARCH_MODULE_HPP_
#define COMPONENT_EXEC_LOCAL_SEARCH_MODULE_HPP_

#include "../Module.hpp"
#include "../LocalSearch.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentExecLocalSearchModule: public Module<R, ADS, DS>
{
public:

	virtual ~ComponentExecLocalSearchModule()
	{
	}

	string id()
	{
		return "component.exec_local_search";
	}

	string usage()
	{
		string u = id();
		u.append(" target_fo timelimit (Solution: | Constructive:) LocalSearch: output_solution_name [spent_time]");
		return u;
	}

	bool run(vector<Module<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "exec_local_search: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		double target_fo = scanner.nextDouble();
		double timelimit = scanner.nextDouble();

		OptFrameComponent* comp = factory.getNextComponent(scanner);
		if(!comp)
		{
			cout << id() << " error: " << "couldn't read Solution: or Constructive: from input (NULL Component)." << endl;
			return NULL;
		}

		pair<LocalSearch<R, ADS, DS>*, string> method = factory.createLocalSearch(scanner.rest());
		if(!method.first)
		{
			cout << id() << " error: " << "couldn't read LocalSearch: from input." << endl;
			return NULL;
		}

		scanner = Scanner(method.second);

		// ---

		Solution<R, ADS>* s;

		Timer t;

		if(comp->id() == Solution<R, ADS>::idComponent())
			s = (Solution<R, ADS>*) comp;
		else if(comp->id() == Constructive<R, ADS>::idComponent())
			s = &((Constructive<R, ADS>*) comp)->generateSolution();
		else
		{
			cout << id() << " error: not a solution or constructive!!" << endl;
			return false;
		}

		Solution<R, ADS>& sFinal = method.first->search(*s, timelimit, target_fo);

		double time = t.inMilliSecs();

		int new_id = factory.addComponent(sFinal);

		stringstream str;
		str << Solution<R, ADS>::idComponent() << " " << new_id;
		string s_new_id = str.str();

		//cout << "'" << s_new_id << "' added." << endl;

		if(comp->id() == Constructive<R, ADS>::idComponent())
			delete s;

		if (scanner.hasNext())
		{
			string new_name = scanner.next();
			if(!Module<R, ADS, DS>::defineText(new_name, s_new_id, dictionary))
				return false;
		}

		if (scanner.hasNext())
		{
			string var_time = scanner.next();
			stringstream sstime;
			sstime.precision(Module<R, ADS, DS>::precision);
			sstime << fixed;
			sstime << time;
			if(!Module<R, ADS, DS>::defineText(var_time, sstime.str(), dictionary))
				return false;
		}

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Module<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}

};

}

#endif /* COMPONENT_EXEC_LOCAL_SEARCH_MODULE_HPP_ */
