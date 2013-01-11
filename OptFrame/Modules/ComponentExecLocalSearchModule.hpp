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

#include "../OptFrameModule.hpp"
#include "../LocalSearch.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ComponentExecLocalSearchModule: public OptFrameModule<R, ADS, M>
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

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
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

		pair<LocalSearch<R, ADS, M>*, string> method = factory.createLocalSearch(scanner.rest());
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
			if(!OptFrameModule<R, ADS, M>::defineText(new_name, s_new_id, dictionary))
				return false;
		}

		if (scanner.hasNext())
		{
			string var_time = scanner.next();
			stringstream sstime;
			sstime << time;
			if(!OptFrameModule<R, ADS, M>::defineText(var_time, sstime.str(), dictionary))
				return false;
		}

		return true;
	}

};

#endif /* COMPONENT_EXEC_LOCAL_SEARCH_MODULE_HPP_ */
