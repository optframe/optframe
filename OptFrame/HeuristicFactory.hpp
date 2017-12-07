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


#include "./Scanner++/Scanner.h"

#include "OptFrameList.hpp"


#include "RandGen.hpp"

#include "Component.hpp"

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
#include "Heuristics/LocalSearches/RVND.hpp"

//Metaheuristics
#include "Heuristics/EmptySingleObjSearch.hpp"
#include "Heuristics/ILS/IteratedLocalSearch.hpp"
#include "Heuristics/ILS/BasicIteratedLocalSearch.hpp"
#include "Heuristics/ILS/IteratedLocalSearchLevels.hpp"
#include "Heuristics/ILS/IntensifiedIteratedLocalSearchLevels.hpp"
#include "Heuristics/ILS/Intensification.hpp"
#include "Heuristics/GRASP/BasicGRASP.hpp"
#include "Heuristics/TS/BasicTabuSearch.hpp"
#include "Heuristics/EvolutionaryAlgorithms/BasicGeneticAlgorithm.hpp"
#include "Heuristics/SA/BasicSimulatedAnnealing.hpp"

#include "Heuristics/EmptyMultiObjSearch.hpp"
#include "Heuristics/VNS/MOVNS.hpp"

#include "ComponentBuilder.h"


using namespace std;
using namespace optframe;

// design pattern: Factory


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class HeuristicFactory
{
private:
	RandGen& rg;

public:

	map<string, vector<Component*> > components;
	vector<ComponentBuilder<R, ADS>* > builders;
	vector<Action<R, ADS>* > actions;
	map<string, vector<vector<Component*> > > componentLists;

	ComponentBuilder<R, ADS>* getBuilder(string id)
	{
		for(unsigned i=0; i<builders.size(); i++)
			if(builders[i]->id() == id)
				return builders[i];
		return nullptr;
	}


	bool inComponents(Component* c)
	{
		map<std::string, vector<Component*> >::iterator iter;
		for(iter = components.begin(); iter != components.end(); iter++)
		{
			vector<Component*> v = iter->second;

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

		int inumber = scanner.nextInt();

		if(inumber < 0)
			return nullptr;

		unsigned number = inumber;

		Component* component = nullptr;

		if(id[0] == ':')
		{
			// COMPONENT SHORTCUT!
			// look for pattern
			map<std::string, vector<Component*> >::iterator iter;
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

	template< class T > void assign(T*& component, unsigned number, string id)
	{
		// check prefix "OptFrame:"
		if (id[0] != 'O')
		{
			string id2 = id;
			id = "OptFrame:";
			id.append(id2);
		}

		if(!Component::compareBase(T::idComponent(), id))
		{
			cout << "HeuristicFactory: incompatible assign '" << T::idComponent() << "' <- '" << id << "'";
			cout << endl;
			component = nullptr;
			return;
		}

		if(components.count(id) > 0)
		{
			vector<Component*>& v = components[id];
			if(number < v.size())
			{
				component = ((T*) v[number]) ;
				return;
			}
		}
		else
			cout << "'" << id << "' not found!" << endl;

		// not found!
		component = nullptr;
	}

	template< class T > void assignList(vector<T*>& cList, unsigned number, string _listId)
	{
		// type checking for safety!
		string noList = Component::typeOfList(_listId);
		string listId = noList;
		listId += "[]";

		if(!Component::compareBase(T::idComponent(), noList))
		{
			cout << "HeuristicFactory: incompatible list assign '[" << T::idComponent() << "]' <- '[" << noList << "]'";
			cout << endl;
			return;
		}

		if(componentLists.count(listId) > 0)
		{
			vector<vector<Component*> >& vv = componentLists[listId];
			if(number < vv.size())
				for(unsigned i=0; i<vv[number].size(); i++)
					cList.push_back((T*)vv[number][i]);
		}
		else
			cout << "'" << listId << " " << number << "' not found!" << endl;
	}

	int addComponent(Component& component, string id)
	{
		if(inComponents(&component))
		{
			cout << "HeuristicFactory addComponent: component '" << component.id()  << "' already registered!" << endl;

			return -1;
		}

		if(!component.compatible(id))
		{
			cout << "HeuristicFactory addComponent: incompatible components '";
			cout << component.id() << "' and '" << id << "'!" << endl;

			return -1;
		}

		vector<Component*>& v = components[id];
		v.push_back(&component);

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

		unsigned int number = scanner.nextInt();

		component = nullptr;

		assign(component, number, tmp);
	}

	int addComponentList(vector<Component*>& cList, string _listId)
	{
		// type checking for safety!
		string noList = Component::typeOfList(_listId);
		string listId = noList;
		listId += "[]";

		for(unsigned i=0; i<cList.size(); i++)
			if((cList[i]==nullptr) || (!cList[i]->compatible(noList)))
			{
				cout << "Warning: incompatible components '";
				cout << cList[i]->id() << "' and '" << Component::typeOfList(listId) << "'!" << endl;

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

		map<std::string, vector<Component*> >::iterator iter;

		for(iter = components.begin(); iter != components.end(); iter++)
		{
			vector<Component*> v = iter->second;

			for (unsigned int i = 0; i < v.size(); i++)
				if (Component::compareBase(pattern, v[i]->id()))
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

		map<std::string, vector<vector<Component*> > >::iterator iter;

		for(iter = componentLists.begin(); iter != componentLists.end(); iter++)
		{
			vector<vector<Component*> > vl = iter->second;

			for (unsigned int i = 0; i < vl.size(); i++)
				if (Component::compareBase(pattern, iter->first))
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
			if (Component::compareBase(pattern, builders[i]->id()))
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
		map<std::string, vector<Component*> >::iterator iter;

		for(iter = components.begin(); iter != components.end(); iter++)
		{
			vector<Component*> v = iter->second;

			for (unsigned int i = 0; i < v.size(); i++)
				delete v[i];

			iter->second.clear();
		}

		map<std::string, vector<vector<Component*> > >::iterator iter2;

		for(iter2 = componentLists.begin(); iter2 != componentLists.end(); iter2++)
		{
			// Should not delete the components inside lists. They're already deleted!
			iter2->second.clear();
		}
	}


	RandGen& getRandGen()
	{
		return rg;
	}


	pair<LocalSearch<R, ADS>*, std::string> createLocalSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
			return pair<LocalSearch<R, ADS>*, std::string>(nullptr, "");

		string h = scanner.next();

		if ((h == LocalSearch<R, ADS>::idComponent()) || (h == "LocalSearch"))
		{
			unsigned int id = scanner.nextInt();

			LocalSearch<R, ADS>* mtd = nullptr;

			assign(mtd, id, LocalSearch<R, ADS>::idComponent());

			if(!mtd)
				return make_pair(new EmptyLocalSearch<R, ADS> , scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == EmptyLocalSearch<R, ADS>::idComponent())
			return make_pair(new EmptyLocalSearch<R, ADS> , scanner.rest());

		for(unsigned i=0; i<builders.size(); i++)
		{
			// build local search directly by builder name
			if(builders[i]->id()==h)
			{
				LocalSearch<R, ADS>* ls = ((LocalSearchBuilder<R,ADS>*)(builders[i]))->build(scanner, *this);
				return make_pair(ls, scanner.rest());
			}

			// locate builder by local search name
			if(builders[i]->canBuild(h))
			{
				LocalSearch<R, ADS>* ls = ((LocalSearchBuilder<R,ADS>*)(builders[i]))->build(scanner, *this);
				return make_pair(ls, scanner.rest());
			}
		}

		cout << "HeuristicFactory::createLocalSearch warning: no LocalSearch '" << h << "' found! ignoring..." << endl;

		return pair<LocalSearch<R, ADS>*, std::string>(nullptr, scanner.rest());
	}


	pair<SingleObjSearch<R, ADS>*, std::string> createSingleObjSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
			return pair<SingleObjSearch<R, ADS>*, std::string>(nullptr, "");

		string h = scanner.next();

		if (h == SingleObjSearch<R, ADS>::idComponent())
		{
			unsigned int id = scanner.nextInt();

			SingleObjSearch<R, ADS>* mtd = nullptr;

			assign(mtd, id, SingleObjSearch<R, ADS>::idComponent());

			if(!mtd)
				return make_pair(new EmptySingleObjSearch<R, ADS> , scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == EmptySingleObjSearch<R, ADS>::idComponent())
			return make_pair(new EmptySingleObjSearch<R, ADS> , scanner.rest());

		for(unsigned i=0; i<builders.size(); i++)
		{
			// build local search directly by builder name
			if(builders[i]->id()==h)
			{
				SingleObjSearch<R, ADS>* sios = ((SingleObjSearchBuilder<R,ADS>*)(builders[i]))->build(scanner, *this);
				return make_pair(sios, scanner.rest());
			}

			// locate builder by local search name
			if(builders[i]->canBuild(h))
			{
				SingleObjSearch<R, ADS>* sios = ((SingleObjSearchBuilder<R,ADS>*)(builders[i]))->build(scanner, *this);
				return make_pair(sios, scanner.rest());
			}
		}


		cout << "HeuristicFactory::createSingleObjSearch warning: no SingleObjSearch '" << h << "' found! ignoring..." << endl;

		return pair<SingleObjSearch<R, ADS>*, std::string>(nullptr, scanner.rest());
	}

	pair<MultiObjSearch<R, ADS>*, std::string> createMultiObjSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
			return pair<MultiObjSearch<R, ADS>*, std::string>(nullptr, "");

		string h = scanner.next();

		if (h == MultiObjSearch<R, ADS>::idComponent())
		{
			unsigned int id = scanner.nextInt();

			MultiObjSearch<R, ADS>* mtd = nullptr;

			assign(mtd, id, MultiObjSearch<R, ADS>::idComponent());

			if (!mtd)
				return make_pair(new EmptyMultiObjSearch<R, ADS>, scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == EmptyMultiObjSearch<R, ADS>::idComponent())
			return make_pair(new EmptyMultiObjSearch<R, ADS>, scanner.rest());

		cout << "HeuristicFactory::createMultiObjSearch warning: no MultiObjSearch '" << h << "' found! ignoring..." << endl;

		return pair<MultiObjSearch<R, ADS>*, std::string>(nullptr, scanner.rest());
	}

};


#endif /* HEURISTICFACTORY_HPP_ */
