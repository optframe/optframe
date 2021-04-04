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

#ifndef HEURISTICFACTORY_HPP_
#define HEURISTICFACTORY_HPP_

#include <iostream>
#include <string>

#include "Solution.hpp"
#include "MultiSolution.hpp"
#include "MultiESolution.hpp"

#include "./Scanner++/Scanner.hpp"

#include "OptFrameList.hpp"


#include "RandGen.hpp"

#include "Component.hpp"
#include "ComponentHelper.hpp"

#include "Action.hpp"

#include "SingleObjSearch.hpp"
#include "MultiObjSearch.hpp"
#include "LocalSearch.hpp"

#include "Heuristics/Empty.hpp"

//Heuristics
#include "Heuristics/LocalSearches/RandomDescentMethod.hpp"
#include "Heuristics/LocalSearches/HillClimbing.hpp"
#include "Heuristics/LocalSearches/BestImprovement.hpp"
#include "Heuristics/LocalSearches/FirstImprovement.hpp"
#include "Heuristics/LocalSearches/CircularSearch.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescentUpdateADS.hpp"
//#include "Heuristics/LocalSearches/RVND.hpp"

//Metaheuristics
#include "Heuristics/EmptySingleObjSearch.hpp"
#include "Heuristics/ILS/IteratedLocalSearch.hpp"
#include "Heuristics/ILS/BasicIteratedLocalSearch.hpp"
#include "Heuristics/ILS/IteratedLocalSearchLevels.hpp"
//TODO ERROR on IntensifiedIteratedLocalSearchLevels
//#include "Heuristics/ILS/IntensifiedIteratedLocalSearchLevels.hpp"
#include "Heuristics/ILS/Intensification.hpp"
#include "Heuristics/GRASP/BasicGRASP.hpp"
#include "Heuristics/TS/BasicTabuSearch.hpp"
//TODO ERROR on BasicGeneticAlgorithm
//#include "Heuristics/EvolutionaryAlgorithms/BasicGeneticAlgorithm.hpp"
#include "Heuristics/SA/BasicSimulatedAnnealing.hpp"

#include "Heuristics/EmptyMultiObjSearch.hpp"
//#include "Heuristics/VNS/MOVNS.hpp"

#include "ComponentBuilder.h"

#include "Solutions/CopySolution.hpp"


//using namespace std;
//using namespace optframe; (?????????????????) Don't use namespace declarations in headers

// design pattern: Factory

namespace optframe
{

//template<XRepresentation R, class ADS = _ADS, XBaseSolution<S, XEv> S = CopySolution<S, XEv>, XEvaluation XEv = Evaluation<>>
//template<class R, class ADS, XSolution S>
//template<XSolution S, XEvaluation XEv = Evaluation<>>
template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class HeuristicFactory
{
private:
	sref<RandGen> rg;

public:

	map<string, vector<std::shared_ptr<Component>> > components;
	vector<ComponentBuilder<S, XEv, XES, X2ES>* > builders;
	vector<Action<S, XEv, XES, X2ES>* > actions;
	map<string, vector<vector<sptr<Component>> > > componentLists;

	ComponentBuilder<S, XEv, XES, X2ES>* getBuilder(string id)
	{
		for(unsigned i=0; i<builders.size(); i++)
			if(builders[i]->id() == id)
				return builders[i];
		return nullptr;
	}


	bool inComponents(std::shared_ptr<Component> c)
	{
		map<std::string, vector<std::shared_ptr<Component>> >::iterator iter;
		for(iter = components.begin(); iter != components.end(); iter++)
		{
			vector<std::shared_ptr<Component>> v = iter->second;

			for (unsigned int i = 0; i < v.size(); i++)
				if(v[i]==c)
					return true;
		}

		return false;
	}

	Component* getNextComponent(Scanner& scanner, string* compName = nullptr, int* compNumber = nullptr)
	{
		if(!scanner.hasNext())
			return nullptr;

		std::string id = scanner.next();

		if(!scanner.hasNextInt())
			return nullptr;

		int inumber = *scanner.nextInt();

		if(inumber < 0)
			return nullptr;

		unsigned number = inumber;

		Component* component = nullptr;

		if(id[0] == ':')
		{
			// COMPONENT SHORTCUT!
			// look for pattern
			map<std::string, vector<std::shared_ptr<Component>> >::iterator iter;
			for(iter = components.begin(); iter != components.end(); iter++)
			{
				string name = iter->first;

				int p = name.find(id, 0);
				if((p > 0) && (p + id.length() == name.length())) // exact match after position 'p'
				{
					component = iter->second.at(number);
					id = name;
					break;
				}
			}
			if(!component)
				cout << "HeuristicFactory warning: pattern of component '" << id << " " << number << "' not found" << endl;
		}
		else
		{
			// look for exact
			if(components.count(id) > 0)
			{
				vector<Component*> v = components[id];

				if(number < v.size())
					component = v[number];
			}
			else
				cout << "HeuristicFactory warning: component '" << id << " " << number << "' not found!" << endl;
		}

		if(compName && compNumber)
		{
			(*compName) = id;
			(*compNumber) = number;
		}

		return component;
	}

	template< class T > void assign(std::shared_ptr<T>& component, unsigned number, string id)
	{
		// check prefix "OptFrame:"
		if (id[0] != 'O')
		{
			string id2 = id;
			id = "OptFrame:";
			id.append(id2);
		}

		if(!ComponentHelper::compareBase(T::idComponent(), id))
		{
			cout << "HeuristicFactory: incompatible assign '" << T::idComponent() << "' <- '" << id << "'";
			cout << endl;
			component = nullptr;
			return;
		}

		if(components.count(id) > 0)
		{
			vector<sptr<Component>>& v = components[id];
			if(number < v.size()) 
			{
				component = std::shared_ptr<T>((T*)v[number].get()); // need to cast to type T...
				return;
			}
		}
		else
			cout << "'" << id << "' not found!" << endl;

		// not found!
		component = nullptr;
	}

	template< class T > void assignList(vector<std::shared_ptr<T>>& cList, unsigned number, string _listId)
	{
		// type checking for safety!
		string noList = ComponentHelper::typeOfList(_listId);
		string listId = noList;
		listId += "[]";

		if(!ComponentHelper::compareBase(T::idComponent(), noList))
		{
			cout << "HeuristicFactory: incompatible list assign '[" << T::idComponent() << "]' <- '[" << noList << "]'";
			cout << endl;
			return;
		}

		if(componentLists.count(listId) > 0)
		{
			vector<vector<sptr<Component>> >& vv = componentLists[listId];
			if(number < vv.size())
				for(unsigned i=0; i<vv[number].size(); i++)
					cList.push_back(sptr<T>((T*)vv[number][i].get()));
		}
		else
			cout << "'" << listId << " " << number << "' not found!" << endl;
	}

	int addComponent(sref<Component> component, string id)
	{
		if(inComponents(component.sptr()))
		{
			cout << "HeuristicFactory addComponent: component '" << component->id()  << "' already registered!" << endl;

			return -1;
		}

		if(!component->compatible(id))
		{
			cout << "HeuristicFactory addComponent: incompatible components '";
			cout << component->id() << "' and '" << id << "'!" << endl;

			return -1;
		}

		vector<std::shared_ptr<Component>>& v = components[id];
      std::shared_ptr<Component> scomp = component.sptr();
		v.push_back(scomp);

		int idx = components[id].size() - 1;

		//cout << "HeuristicFactory: added component '" << id << " " << idx << "'" << endl;

		return idx;
	}

	int addComponent(Component& component)
	{
		return addComponent(component, component.id());
	}

	template< class T > void readComponent(T*& component, Scanner& scanner)
	{
		std::string tmp = scanner.next();

		if(tmp != T::idComponent())
		{
			cout << "Error: expected '" << T::idComponent() << "' and found '" << tmp << "'.";
			cout << endl;
			component = nullptr;

			return;
		}

		unsigned int number = *scanner.nextInt();

		component = nullptr;

		assign(component, number, tmp);
	}

	int addComponentList(vector<Component*>& cList, string _listId)
	{
		// type checking for safety!
		string noList = ComponentHelper::typeOfList(_listId);
		string listId = noList;
		listId += "[]";

		for(unsigned i=0; i<cList.size(); i++)
			if((cList[i]==nullptr) || (!cList[i]->compatible(noList)))
			{
				cout << "Warning: incompatible components '";
				cout << cList[i]->id() << "' and '" << ComponentHelper::typeOfList(listId) << "'!" << endl;

				return -1;
			}

		vector<vector<Component*> >& v = componentLists[listId];
		v.push_back(cList);

		int idx = componentLists[listId].size() - 1;

		//cout << "HeuristicFactory: added component list '" << listId << " " << idx << "'" << endl;

		return idx;
	}

	int addComponentList(vector<Component*>& cList)
	{
		if((cList.size()>0) && (cList[0] != nullptr))
		{
			string listId = cList[0]->id();
			listId += "[]";

			return addComponentList(cList, listId);
		}
		else
			return -1;
	}


	//! \english listComponents lists all available components that match a given pattern. \endenglish \portuguese listComponents lista todos componentes disponiveis que coincidem com um padrao dado. \endportuguese
	/*!
		 \sa listComponents(string)
	 */
	vector<string> listComponents(string pattern)
	{
		vector<string> list;

		map<std::string, vector<std::shared_ptr<Component>> >::iterator iter;

		for(iter = components.begin(); iter != components.end(); iter++)
		{
			vector<Component*> v = iter->second;

			for (unsigned int i = 0; i < v.size(); i++)
				if (ComponentHelper::compareBase(pattern, v[i]->id()))
				{
					stringstream ss;
					ss << iter->first << " " << i;
					list.push_back(ss.str());
				}
		}

		return list;
	}


	//! \english listAllComponents lists all available OptFrame components. \endenglish \portuguese listAllComponents lista todos os componentes do OptFrame disponiveis. \endportuguese
	/*!
		 \sa listAllComponents()
	 */

	vector<string> listAllComponents()
	{
		return listComponents("OptFrame:");
	}


	//! \english listComponents lists all available components that match a given pattern. \endenglish \portuguese listComponents lista todos componentes disponiveis que coincidem com um padrao dado. \endportuguese
	/*!
		 \sa listComponents(string)
	 */
	vector<string> listComponentLists(string pattern)
	{
		vector<string> list;

		map<std::string, vector<vector<sptr<Component>> > >::iterator iter;

		for(iter = componentLists.begin(); iter != componentLists.end(); iter++)
		{
			vector<vector<Component*> > vl = iter->second;

			for (unsigned int i = 0; i < vl.size(); i++)
				if (ComponentHelper::compareBase(pattern, iter->first))
				{
					stringstream ss;
					ss << iter->first << " " << i;
					list.push_back(ss.str());
				}
		}

		return list;
	}

	//! \english listBuilders lists all component builders that match a given pattern, with their respective parameters. \endenglish \portuguese listBuilders lista todos component builders, com seus respectivos parametros, que coincidem com um padrao dado. \endportuguese
	/*!
		 \sa listComponents(string)
	 */

	vector<pair<string, vector<pair<string,string> > > > listBuilders(string pattern)
	{
		vector<pair<string, vector<pair<string,string> > > > list;

		for(unsigned i=0; i<builders.size(); i++)
			if (ComponentHelper::compareBase(pattern, builders[i]->id()))
				list.push_back(make_pair(builders[i]->id(), builders[i]->parameters()));

		return list;
	}


	// ================================================================
	// ================================================================

	HeuristicFactory() :
		rg(*new RandGen)
	{
	}

	HeuristicFactory(RandGen _rg) :
		rg(*new RandGen(_rg))
	{
	}

	virtual ~HeuristicFactory()
	{
		clear();

		delete& rg;

		for(unsigned i=0; i<builders.size(); i++)
			delete builders.at(i);
		builders.clear();

		for(unsigned i=0; i<actions.size(); i++)
			delete actions.at(i);
		actions.clear();
	}

	bool drop(string type, int id)
	{
		if(components.count(type) > 0)
		{
			vector<Component*> v = components[type];

			if(id < ((int)v.size())) // else return false?
			{
				if(v[id] != nullptr) // else return false?
				{
					delete v[id];
					v[id] = nullptr;
					components[type] = v;

					return true;
				}
			}
		}

		return false;
	}

	// EMPTY ALL LISTS!
	// Components, Builders and Actions
	void clear()
	{
		map<std::string, vector<std::shared_ptr<Component>> >::iterator iter;

		for(iter = components.begin(); iter != components.end(); iter++)
		{
			vector<std::shared_ptr<Component>> v = iter->second;

			//for (unsigned int i = 0; i < v.size(); i++)
			//	delete v[i];

			iter->second.clear();
		}

		map<std::string, vector<vector<sptr<Component>> > >::iterator iter2;

		for(iter2 = componentLists.begin(); iter2 != componentLists.end(); iter2++)
		{
			// Should not delete the components inside lists. They're already deleted!
			iter2->second.clear();
		}
	}


	sref<RandGen> getRandGen()
	{
		return rg;
	}


	pair<sptr<LocalSearch<XES, XEv>>, std::string> createLocalSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
			return pair<sptr<LocalSearch<XES, XEv>>, std::string>(nullptr, "");

		string h = scanner.next();

		if ((h == LocalSearch<XES, XEv>::idComponent()) || (h == "LocalSearch"))
		{
			unsigned int id = *scanner.nextInt();

			sptr<LocalSearch<XES, XEv>> mtd = nullptr;

			assign(mtd, id, LocalSearch<XES, XEv>::idComponent());

			if(!mtd)
				return pair<sptr<LocalSearch<XES, XEv>>, std::string>(new EmptyLocalSearch<XES, XEv> , scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == EmptyLocalSearch<XES, XEv>::idComponent())
			return pair<sptr<LocalSearch<XES, XEv>>, std::string>(new EmptyLocalSearch<XES, XEv> , scanner.rest());

		for(unsigned i=0; i<builders.size(); i++)
		{
			// build local search directly by builder name
			if(builders[i]->id()==h)
			{
				LocalSearch<XES, XEv>* ls = ((LocalSearchBuilder<S, XEv>*)(builders[i]))->build(scanner, *this);
				return pair<sptr<LocalSearch<XES, XEv>>, std::string>(ls, scanner.rest());
			}

			// locate builder by local search name
			if(builders[i]->canBuild(h))
			{
				LocalSearch<XES, XEv>* ls = ((LocalSearchBuilder<S, XEv>*)(builders[i]))->build(scanner, *this);
				return pair<sptr<LocalSearch<XES, XEv>>, std::string>(ls, scanner.rest());
			}
		}

		cout << "HeuristicFactory::createLocalSearch warning: no LocalSearch '" << h << "' found! ignoring..." << endl;

		return pair<sptr<LocalSearch<XES, XEv>>, std::string>(nullptr, scanner.rest());
	}


	pair<sptr<SingleObjSearch<XES>>, std::string> createSingleObjSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
			return pair<sptr<SingleObjSearch<XES>>, std::string>(nullptr, "");

		string h = scanner.next();

		if (h == SingleObjSearch<XES>::idComponent())
		{
			unsigned int id = *scanner.nextInt();

			sptr<SingleObjSearch<XES>> mtd = nullptr;

			assign(mtd, id, SingleObjSearch<XES>::idComponent());

			if(!mtd)
				return pair<sptr<SingleObjSearch<XES>>, std::string>(new EmptySingleObjSearch<XES, XEv> , scanner.rest());

			return pair<sptr<SingleObjSearch<XES>>, std::string>(mtd, scanner.rest());
		}

		if (h == EmptySingleObjSearch<XES, XEv>::idComponent())
			return pair<sptr<SingleObjSearch<XES>>, std::string>(new EmptySingleObjSearch<XES, XEv> , scanner.rest());

		for(unsigned i=0; i<builders.size(); i++)
		{
			// build local search directly by builder name
			if(builders[i]->id()==h)
			{
				SingleObjSearch<XES>* sios = ((SingleObjSearchBuilder<S, XEv>*)(builders[i]))->build(scanner, *this);
				return pair<sptr<SingleObjSearch<XES>>, std::string>(sios, scanner.rest());
			}

			// locate builder by local search name
			if(builders[i]->canBuild(h))
			{
				SingleObjSearch<XES>* sios = ((SingleObjSearchBuilder<S, XEv>*)(builders[i]))->build(scanner, *this);
				return pair<sptr<SingleObjSearch<XES>>, std::string>(sios, scanner.rest());
			}
		}


		cout << "HeuristicFactory::createSingleObjSearch warning: no SingleObjSearch '" << h << "' found! ignoring..." << endl;

		return pair<sptr<SingleObjSearch<XES>>, std::string>(nullptr, scanner.rest());
	}

	pair<MultiObjSearch<XES>*, std::string> createMultiObjSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
			return pair<MultiObjSearch<XES>*, std::string>(nullptr, "");

		string h = scanner.next();

		if (h == MultiObjSearch<XES>::idComponent())
		{
			unsigned int id = *scanner.nextInt();

			MultiObjSearch<XES>* mtd = nullptr;

			assign(mtd, id, MultiObjSearch<XES>::idComponent());

			if (!mtd)
				return make_pair(new EmptyMultiObjSearch<XES>, scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == EmptyMultiObjSearch<XES>::idComponent())
			return make_pair(new EmptyMultiObjSearch<XES>, scanner.rest());

		cout << "HeuristicFactory::createMultiObjSearch warning: no MultiObjSearch '" << h << "' found! ignoring..." << endl;

		return pair<MultiObjSearch<XES>*, std::string>(nullptr, scanner.rest());
	}

};

} // namespace optframe

#endif /* HEURISTICFACTORY_HPP_ */
