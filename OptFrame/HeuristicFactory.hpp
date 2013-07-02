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
#include "Heuristics/GRASP/GRASP.hpp"
#include "Heuristics/TS/TabuSearch.hpp"
#include "Heuristics/EvolutionaryAlgorithms/GeneticAlgorithm.hpp"
#include "Heuristics/SA/BasicSimulatedAnnealing.hpp"

#include "Heuristics/EmptyMultiObjSearch.hpp"
#include "Heuristics/VNS/MOVNS.hpp"

#include "ComponentBuilder.h"


using namespace std;
using namespace optframe;

// design pattern: Factory


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class HeuristicFactory
{
private:
	RandGen& rg;

public:

	map<string, vector<Component*> > components;
	vector<ComponentBuilder<R, ADS, DS>* > builders;
	vector<Action<R, ADS, DS>* > actions;
	map<string, vector<vector<Component*> > > componentLists;

	ComponentBuilder<R, ADS, DS>* getBuilder(string id)
	{
		for(unsigned i=0; i<builders.size(); i++)
			if(builders[i]->id() == id)
				return builders[i];
		return NULL;
	}

	/*
	optframe::Action<R, ADS, DS>* getAction() // TODO: STUB!
	{
		return actions[0];
	}
	*/

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

	Component* getNextComponent(Scanner& scanner)
	{
		std::string id = "";
		unsigned number = 0;

		if(scanner.hasNext())
			id = scanner.next();

		// safety for unsigned!
		bool validNumber = true;

		if(scanner.hasNext())
		{
			string snum = scanner.next();
			for(unsigned i=0; i<snum.size(); i++)
				if( (snum.at(i)!='.') && ((snum.at(i)<'0') || (snum.at(i)>'9')) )
				{
					// not a number
					validNumber = false;
					break;
				}

			if(validNumber)
				number = Scanner::parseInt(snum);
		}
		else
			validNumber = false;

		if((id=="") || (!validNumber))
			return NULL;

		Component* component = NULL;

		if(components.count(id) > 0)
		{
			vector<Component*> v = components[id];

			if(number < v.size())
				component = v[number];
		}
		else
			cout << "HeuristicFactory warning: component '" << id << " " << number << "' not found!" << endl;

		return component;
	}

	template< class T > void assign(T*& component, unsigned number, string id)
	{
		if(!Component::compareBase(T::idComponent(), id))
		{
			cout << "HeuristicFactory: incompatible assign '" << T::idComponent() << "' <- '" << id << "'" << endl;
			component = NULL;
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
		component = NULL;
	}

	template< class T > void assignList(vector<T*>& cList, unsigned number, string _listId)
	{
		// type checking for safety!
		string noList = Component::typeOfList(_listId);
		string listId = noList;
		listId += "[]";

		if(!Component::compareBase(T::idComponent(), noList))
		{
			cout << "HeuristicFactory: incompatible list assign '[" << T::idComponent() << "]' <- '[" << noList << "]'" << endl;
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
			cout << "Error: expected '" << T::idComponent() << "' and found '" << tmp << "'." << endl;
			component = NULL;

			return;
		}

		unsigned int number = scanner.nextInt();

		component = NULL;

		assign(component, number, tmp);
	}

	int addComponentList(vector<Component*>& cList, string _listId)
	{
		// type checking for safety!
		string noList = Component::typeOfList(_listId);
		string listId = noList;
		listId += "[]";

		for(unsigned i=0; i<cList.size(); i++)
			if((cList[i]==NULL) || (!cList[i]->compatible(noList)))
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
		if((cList.size()>0) && (cList[0] != NULL))
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


#ifdef MaPI
	MyMaPISerializer<R, ADS, DS> * serializer;
	MaPI_MapReduce<R, RankAndStop, int, pair<R, double> , R> * mapReduce;
	MyMaPIMapper<R, ADS, DS> * mapper;
	MyMaPIReducer<R, ADS, DS> * reducer;
	int argc;
	char **argv;

	void setMapReduce(MyMaPISerializer<R, ADS, DS> * serializer, MaPI_MapReduce<R, RankAndStop, int, pair<R, double> , R> * mapReduce,
			MyMaPIMapper<R, ADS, DS> * mapper,MyMaPIReducer<R, ADS, DS> * reducer,int argc, char **argv)
	{
		this->serializer = serializer;
		this->mapReduce = mapReduce;
		this->mapper = mapper;
		this->reducer = reducer;
		this->argc = argc;
		this->argv = argv;
	}
#endif/**/


	int read_np(Scanner& scanner)
	{
		std::string tmp = scanner.next();

		if (tmp != "np")
		{
			cout << "Error: expected 'np' and found '" << tmp << "'." << endl;
			return -1;
		}

		int np = scanner.nextInt();

		if (np <= 1)
		{
			cout << "Error: number or processes " << np << " must be greater than or equal 2" << endl;
			return -1;
		}

		return np;
	}

	/*
	vector<NS<R, ADS, DS>*> read_ns_list(Scanner& scanner)
	{
		vector<string>* plist = OptFrameList::readList(scanner);
		vector<string>  list;
		if(plist)
		{
			list = vector<string>(*plist);
			delete plist;
		}
		//else
		//	return NULL;

		Scanner* aux;
		std::string tmp;

		vector<NS<R, ADS, DS>*> v_ns;

		for (unsigned int i = 0; i < list.size(); i++)
		{
			aux = new Scanner(list.at(i));

			NS<R, ADS, DS>* ns;
			readComponent(ns, *aux);

			if(!ns)
				return vector<NS<R, ADS, DS>*>();

			v_ns.push_back(ns);

			delete aux;
		}

		if (v_ns.size() == 0)
		{
			cout << "Error: empty ns list." << endl;
			return vector<NS<R, ADS, DS>*>();
		}

		return v_ns;
	}

	vector<Evaluator<R, ADS, DS> *> read_ev_list(Scanner& scanner)
	{
		vector < std::string >& list = OptFrameList::readList(scanner);

		Scanner* aux;
		std::string tmp;

		vector<Evaluator<R, ADS, DS> *> v_ev;

		for (unsigned int i = 0; i < list.size(); i++)
		{
			aux = new Scanner(list.at(i));

			Evaluator<R, ADS, DS> * ev;
			readComponent(ev, aux);

			if(!ev)
				return vector<Evaluator<R, ADS, DS> *>();

			v_ev.push_back(ev);
		}

		if (v_ev.size() == 0)
		{
			cout << "Error: empty ev list." << endl;
			return vector<Evaluator<R, ADS, DS> *>();
		}

		delete &list;
		return v_ev;
	}

	vector<LocalSearch<R, ADS, DS>*> read_local_search_list(Scanner& scanner)
	{
		vector < std::string >& list = OptFrameList::readList(scanner);
		vector<LocalSearch<R, ADS, DS>*> v_heuristics;

		pair<LocalSearch<R, ADS, DS>*, std::string> method;

		for (unsigned int i = 0; i < list.size(); i++)
		{
			method = createLocalSearch(list.at(i));
			v_heuristics.push_back(method.first);
		}

		if (v_heuristics.size() == 0)
		{
			cout << "Error: empty heuristic list." << endl;
			return v_heuristics;
		}

		delete &list;
		return v_heuristics;
	}
	*/

	Evaluator<R, ADS, DS>* read_ev(Scanner& scanner)
	{
		Evaluator<R, ADS, DS>* ev = NULL;
		readComponent(ev, scanner);

		return ev;
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
		drop_all();

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
				if(v[id] != NULL) // else return false?
				{
					delete v[id];
					v[id] = NULL;
					components[type] = v;

					return true;
				}
			}
		}

		return false;
	}

	void drop_all()
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


	pair<LocalSearch<R, ADS, DS>*, std::string> createLocalSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
			return make_pair(new EmptyLocalSearch<R, ADS, DS> , "");

		string h = scanner.next();

		if (h == LocalSearch<R, ADS, DS>::idComponent())
		{
			unsigned int id = scanner.nextInt();

			LocalSearch<R, ADS, DS>* mtd = NULL;

			assign(mtd, id, LocalSearch<R, ADS, DS>::idComponent());

			if(!mtd)
				return make_pair(new EmptyLocalSearch<R, ADS, DS> , scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == EmptyLocalSearch<R, ADS, DS>::idComponent())
			return make_pair(new EmptyLocalSearch<R, ADS, DS> , scanner.rest());

		for(unsigned i=0; i<builders.size(); i++)
		{
			// build local search directly by builder name
			if(builders[i]->id()==h)
			{
				LocalSearch<R, ADS, DS>* ls = ((LocalSearchBuilder<R,ADS,DS>*)(builders[i]))->build(scanner, *this);
				return make_pair(ls, scanner.rest());
			}

			// locate builder by local search name
			if(builders[i]->canBuild(h))
			{
				LocalSearch<R, ADS, DS>* ls = ((LocalSearchBuilder<R,ADS,DS>*)(builders[i]))->build(scanner, *this);
				return make_pair(ls, scanner.rest());
			}
		}

		cout << "Warning: no LocalSearch '" << h << "' found! ignoring..." << endl;

		return make_pair(new EmptyLocalSearch<R, ADS, DS> , scanner.rest());
	}


	pair<SingleObjSearch<R, ADS, DS>*, std::string> createSingleObjSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
		{
			SingleObjSearch<R, ADS, DS>* sios = new EmptySingleObjSearch<R, ADS, DS>;
			return make_pair(sios , "");
		}

		string h = scanner.next();

		if (h == SingleObjSearch<R, ADS, DS>::idComponent())
		{
			unsigned int id = scanner.nextInt();

			SingleObjSearch<R, ADS, DS>* mtd = NULL;

			assign(mtd, id, SingleObjSearch<R, ADS, DS>::idComponent());

			if(!mtd)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == EmptySingleObjSearch<R, ADS, DS>::idComponent())
			return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

		for(unsigned i=0; i<builders.size(); i++)
		{
			// build local search directly by builder name
			if(builders[i]->id()==h)
			{
				SingleObjSearch<R, ADS, DS>* sios = ((SingleObjSearchBuilder<R,ADS,DS>*)(builders[i]))->build(scanner, *this);
				return make_pair(sios, scanner.rest());
			}

			// locate builder by local search name
			if(builders[i]->canBuild(h))
			{
				SingleObjSearch<R, ADS, DS>* sios = ((SingleObjSearchBuilder<R,ADS,DS>*)(builders[i]))->build(scanner, *this);
				return make_pair(sios, scanner.rest());
			}
		}



		if (h == GRASP<R, ADS, DS>::idComponent())
		{
			cout << "Heuristic: GRASP" << endl;

			Evaluator<R, ADS, DS>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			string rest = scanner.rest();

			pair<LocalSearch<R, ADS, DS>*, string> method;
			method = createLocalSearch(rest);

			LocalSearch<R, ADS, DS>* ls = method.first;

			scanner = Scanner(method.second);

			int iter = scanner.nextInt();

			return make_pair(new GRASP<R, ADS, DS> (*evaluator, *initsol, *ls, iter), scanner.rest());
		}

		if (h == TabuSearch<R, ADS, DS>::idComponent())
		{
			cout << "Heuristic: Tabu Search" << endl;

			Evaluator<R, ADS, DS>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			NSSeq<R, ADS, DS>* ns_seq = (NSSeq<R, ADS, DS>*) getNextComponent(scanner);

			if(!ns_seq)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			int tamT = scanner.nextInt();
			int BTmax = scanner.nextInt();

			return make_pair(new TabuSearch<R, ADS, DS> (*evaluator, *initsol, *ns_seq, tamT, BTmax), scanner.rest());
		}

		if (h == BasicIteratedLocalSearch<R, ADS, DS>::idComponent())
		{
			cout << "Heuristic: Basic Iterated Local Search" << endl;

			Evaluator<R, ADS, DS>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());


			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<LocalSearch<R, ADS, DS>*, string> method;
			method = createLocalSearch(rest);

			LocalSearch<R, ADS, DS>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			BasicILSPerturbation<R, ADS, DS>* ils_pert = NULL;
			readComponent(ils_pert, scanner);

			if(!ils_pert)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			int iterMax = scanner.nextInt();

			return make_pair(new BasicIteratedLocalSearch<R, ADS, DS> (*evaluator, *initsol, *localSearch, *ils_pert, iterMax), scanner.rest());
		}

		/*
		if (h == BasicSimulatedAnnealing<R, ADS, DS>::idComponent())
		{
			cout << "Heuristic: Basic Simulated Annealing" << endl;

			Evaluator<R, ADS, DS>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			vector<NS<R, ADS, DS>* > ns_list = read_ns_list(scanner);
			if(ns_list.size()==0)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			double alpha = scanner.nextDouble();
			int SAmax = scanner.nextInt();
			double Ti = scanner.nextDouble();

			return make_pair(new BasicSimulatedAnnealing<R, ADS, DS> (*evaluator, *initsol, ns_list, alpha, SAmax, Ti, rg), scanner.rest());
		}
		*/

		if (h == IteratedLocalSearchLevels<R, ADS, DS>::idComponent())
		{
			cout << "Heuristic: Iterated Local Search (Levels)" << endl;

			Evaluator<R, ADS, DS>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());


			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<LocalSearch<R, ADS, DS>*, string> method;
			method = createLocalSearch(rest);

			LocalSearch<R, ADS, DS>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			ILSLPerturbation<R, ADS, DS>* ilsl_pert = NULL;
			readComponent(ilsl_pert, scanner);

			if(!ilsl_pert)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			int iterMax = scanner.nextInt();
			int levelMax = scanner.nextInt();

			return make_pair(new IteratedLocalSearchLevels<R, ADS, DS> (*evaluator, *initsol, *localSearch, *ilsl_pert, iterMax, levelMax), scanner.rest());
		}

		if (h == IntensifiedIteratedLocalSearchLevels<R, ADS, DS>::idComponent())
		{
			cout << "Heuristic: Intensified Iterated Local Search (Levels)" << endl;

			Evaluator<R, ADS, DS>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());


			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<LocalSearch<R, ADS, DS>*, string> method;
			method = createLocalSearch(rest);

			LocalSearch<R, ADS, DS>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			/*rest = scanner.rest();

				 pair<HTrajectory<R, ADS, DS>*, string> method2;
				 method2 = createHeuristic(rest);

				 HTrajectory<R, ADS, DS>* localSearch2 = method2.first;

				 scanner = Scanner(method2.second);*/

			Intensification<R, ADS, DS> * intensification = NULL;
			readComponent(intensification, scanner);

			if(!intensification)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			// ====================

			ILSLPerturbation<R, ADS, DS>* ilsl_pert = NULL;
			readComponent(ilsl_pert, scanner);

			if(!ilsl_pert)
				return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());

			int iterMax = scanner.nextInt();
			int levelMax = scanner.nextInt();

			//return make_pair(new IntensifiedIteratedLocalSearchLevels<R, ADS, DS> (*evaluator, *localSearch, *localSearch2, *ilsl_pert, iterMax, levelMax),scanner.rest());
			return make_pair(new IntensifiedIteratedLocalSearchLevels<R, ADS, DS> (*evaluator, *initsol, *localSearch, *intensification, *ilsl_pert, iterMax, levelMax), scanner.rest());
		}


#ifdef MaPI
		if (h == "MapReduce")
		{
			cout << "Heuristic: MapReduce" << endl;

			Evaluator<R, ADS, DS>* evaluator = read_ev(&scanner);

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<HTrajectory<R, ADS, DS>*, string> method;
			method = createHeuristic(rest);

			HTrajectory<R, ADS, DS>* hmap = method.first;

			scanner = Scanner(method.second);

			// ===================
			// Read next heuristic
			// ===================

			rest = scanner.rest();

			method = createHeuristic(rest);

			HTrajectory<R, ADS, DS>* hreduce = method.first;

			scanner = Scanner(method.second);

			// ===================
			mapper->setHeuristic(hmap);
			reducer->setHeuristic(hreduce); // reduz usando a heurística hreduce

			return make_pair(new OptFrameMapReduce<R, ADS, DS> (*serializer,*mapReduce,*mapper,*reducer,*evaluator), scanner.rest());
		}

		if (h == "Map")
		{
			cout << "Heuristic: Map" << endl;

			Evaluator<R, ADS, DS>* evaluator = read_ev(&scanner);

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<HTrajectory<R, ADS, DS>*, string> method;
			method = createHeuristic(rest);

			HTrajectory<R, ADS, DS>* hmap = method.first;

			scanner = Scanner(method.second);

			// ===================
			mapper->setHeuristic(hmap);
			// A não especificação da heurística de redução implina na
			// redução à melhor solução produzida nos mapeamentos

			return make_pair(new OptFrameMapReduce<R, ADS, DS> (*serializer,*mapReduce,*mapper,*reducer,*evaluator), scanner.rest());
		}
#endif

		cout << "Warning: no SingleObjSearch '" << h << "' found! ignoring..." << endl;

		return make_pair(new EmptySingleObjSearch<R, ADS, DS> , scanner.rest());
	}

	pair<MultiObjSearch<R, ADS, DS>*, std::string> createMultiObjSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
		{
			MultiObjSearch<R, ADS, DS>* mios = new EmptyMultiObjSearch<R, ADS,	DS >;
			return make_pair(mios, "");
		}

		string h = scanner.next();

		if (h == MultiObjSearch<R, ADS, DS>::idComponent())
		{
			unsigned int id = scanner.nextInt();

			MultiObjSearch<R, ADS, DS>* mtd = NULL;

			assign(mtd, id, MultiObjSearch<R, ADS, DS>::idComponent());

			if (!mtd)
				return make_pair(new EmptyMultiObjSearch<R, ADS, DS>, scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == EmptyMultiObjSearch<R, ADS, DS>::idComponent())
			return make_pair(new EmptyMultiObjSearch<R, ADS, DS>, scanner.rest());

		cout << "Warning: no MultiObjSearch '" << h << "' found! ignoring..." << endl;

		return make_pair(new EmptyMultiObjSearch<R, ADS, DS>, scanner.rest());
	}

};


#endif /* HEURISTICFACTORY_HPP_ */
