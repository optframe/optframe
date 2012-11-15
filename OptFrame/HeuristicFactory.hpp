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


#include "RandGen.hpp"

#include "OptFrameComponent.hpp"

#include "OptFrameContainer.hpp"

#include "SingleObjSearch.hpp"
#include "MultiObjSearch.hpp"
#include "LocalSearch.hpp"

#include "Heuristics/Empty.hpp"

//Heuristics
#include "Heuristics/RandomDescentMethod.hpp"
#include "Heuristics/HillClimbing.hpp"
#include "Heuristics/BestImprovement.hpp"
#include "Heuristics/FirstImprovement.hpp"
#include "Heuristics/CircularSearch.hpp"
#include "Heuristics/VariableNeighborhoodDescent.hpp"
#include "Heuristics/RVND.hpp"

//Metaheuristics
#include "Heuristics/EmptySingleObjSearch.hpp"
#include "Heuristics/IteratedLocalSearch.hpp"
#include "Heuristics/BasicIteratedLocalSearch.hpp"
#include "Heuristics/IteratedLocalSearchLevels.hpp"
#include "Heuristics/IntensifiedIteratedLocalSearchLevels.hpp"
#include "Heuristics/Intensification.hpp"
#include "Heuristics/GRASP.hpp"
#include "Heuristics/TabuSearch.hpp"
#include "Heuristics/EvolutionaryAlgorithms/GeneticAlgorithm.hpp"
#include "Heuristics/BasicSimulatedAnnealing.hpp"

#include "Heuristics/EmptyMultiObjSearch.hpp"
#include "Heuristics/MOVNS.hpp"

#include "ComponentBuilder.h"


using namespace std;

// design pattern: Factory

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class HeuristicFactory
{
private:
	RandGen& rg;

public:

	map<string, vector<OptFrameComponent*> > components;
	vector<ComponentBuilder<R, ADS, M>* > builders;
	map<string, vector<vector<OptFrameComponent*> > > componentLists;

	OptFrameComponent* getNextComponent(Scanner& scanner)
	{
		std::string id = scanner.next();
		unsigned number = scanner.nextInt();

		OptFrameComponent* component = NULL;

		if(components.count(id) > 0)
		{
			vector<OptFrameComponent*> v = components[id];

			if(number < v.size())
				component = v[number];
		}
		else
			cout << "HeuristicFactory warning: component '" << id << " " << number << "' not found!" << endl;

		return component;
	}

	template< class T > void assign(T*& component, unsigned number, string id)
	{
		if(!compareBase(T::idComponent(), id))
		{
			cout << "HeuristicFactory: incompatible assign '" << T::idComponent() << "' <- '" << id << "'" << endl;
			component = NULL;
			return;
		}

		if(components.count(id) > 0)
		{
			vector<OptFrameComponent*>& v = components[id];
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
		string noList = typeOfList(_listId);
		string listId = "[]";
		listId += noList;

		if(!compareBase(T::idComponent(), noList))
		{
			cout << "HeuristicFactory: incompatible list assign '[" << T::idComponent() << "]' <- '[" << noList << "]'" << endl;
			return;
		}

		if(componentLists.count(listId) > 0)
		{
			vector<vector<OptFrameComponent*> >& vv = componentLists[listId];
			if(number < vv.size())
				for(unsigned i=0; i<vv[number].size(); i++)
					cList.push_back((T*)vv[number][i]);
		}
		else
			cout << "'" << listId << " " << number << "' not found!" << endl;
	}

	int addComponent(OptFrameComponent& component, string id)
	{
		if(!component.compatible(id))
		{
			cout << "Warning: incompatible components '";
			cout << component.id() << "' and '" << id << "'!" << endl;

			return -1;
		}

		vector<OptFrameComponent*>& v = components[id];
		v.push_back(&component);

		int idx = components[id].size() - 1;

		//cout << "HeuristicFactory: added component '" << id << " " << idx << "'" << endl;

		return idx;
	}

	int addComponent(OptFrameComponent& component)
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

	int addComponentList(vector<OptFrameComponent*>& cList, string _listId)
	{
		// type checking for safety!
		string noList = typeOfList(_listId);
		string listId = "[]";
		listId += noList;

		for(unsigned i=0; i<cList.size(); i++)
			if((cList[i]==NULL) || (!cList[i]->compatible(noList)))
			{
				cout << "Warning: incompatible components '";
				cout << cList[i]->id() << "' and '" << typeOfList(listId) << "'!" << endl;

				return -1;
			}

		vector<vector<OptFrameComponent*> >& v = componentLists[listId];
		v.push_back(cList);

		int idx = componentLists[listId].size() - 1;

		return idx;
	}

	int addComponentList(vector<OptFrameComponent*>& cList)
	{
		if((cList.size()>0) && (cList[0] != NULL))
		{
			string listId = "[]";
			listId += cList[0]->id();

			return addComponentList(cList, listId);
		}
		else
			return -1;
	}

	//! \english compareBase is an auxiliar method to compare a pattern to a component id. Check if 'component' inherits from 'base'. \endenglish \portuguese compareBase eh um metodo auxiliar para comparar um padrao a um id de componente. Testa se 'component' herda de 'base'. \endportuguese
	/*!
		 \sa compareBase(string, string)
	 */

	// Check if 'base' inherits from 'component'
	bool compareBase(string _base, string _component)
	{
		if((_base.length()<3) || (_component.length()<3))
		{
			cout << "HeuristicFactory::compareBase warning: comparing less than 3 characters! with base='" << _base << "' component='" << _component << "'" << endl;
			return false;
		}

		bool baseIsList      = (_base.at(_base.length()-2) == '['          ) && (_base.at(_base.length()-1) == ']'          );
		bool componentIsList = (_component.at(_component.length()-2) == '[') && (_component.at(_component.length()-1) == ']');

		if(baseIsList != componentIsList)
			return false;

		// remove list (if exists)
		string base      = typeOfList(_base);
		string component = typeOfList(_component);

		bool sameBase = true;

		if(base.length() <= component.length())
		{
			for(unsigned i=0; i<base.length(); i++)
				if(base.at(i) != component.at(i))
					sameBase = false;
		}
		else
			sameBase = false;

		if(sameBase)
			return true;

		// ------------------
		// check last family
		// ------------------

		Scanner scanner(base);
		scanner.useSeparators(":");

		string family = scanner.next();
		while(scanner.hasNext())
			family = scanner.next();

		Scanner scanComponent(component);
		scanComponent.useSeparators(":");
		string part;
		while(scanComponent.hasNext())
		{
			part = scanComponent.next();
			if(part == family)
				sameBase = true;
		}

		return sameBase;
	}

	string typeOfList(string listId)
	{
		Scanner scanner(listId);
		scanner.useSeparators(" \t\n[]");

		return scanner.next();
	}


	//! \english listComponents lists all available components that match a given pattern. \endenglish \portuguese listComponents lista todos componentes disponiveis que coincidem com um padrao dado. \endportuguese
	/*!
		 \sa listComponents(string)
	 */
	vector<string> listComponents(string pattern)
	{
		vector<string> list;

		map<std::string, vector<OptFrameComponent*> >::iterator iter;

		for(iter = components.begin(); iter != components.end(); iter++)
		{
			vector<OptFrameComponent*> v = iter->second;

			for (unsigned int i = 0; i < v.size(); i++)
				if (compareBase(pattern, v[i]->id()))
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

		map<std::string, vector<vector<OptFrameComponent*> > >::iterator iter;

		for(iter = componentLists.begin(); iter != componentLists.end(); iter++)
		{
			vector<vector<OptFrameComponent*> > vl = iter->second;

			for (unsigned int i = 0; i < vl.size(); i++)
				if (compareBase(pattern, iter->first))
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
			if (compareBase(pattern, builders[i]->id()))
				list.push_back(make_pair(builders[i]->id(), builders[i]->parameters()));

		return list;
	}


#ifdef MaPI
	MyMaPISerializer<R, ADS, M> * serializer;
	MaPI_MapReduce<R, RankAndStop, int, pair<R, double> , R> * mapReduce;
	MyMaPIMapper<R, ADS, M> * mapper;
	MyMaPIReducer<R, ADS, M> * reducer;
	int argc;
	char **argv;

	void setMapReduce(MyMaPISerializer<R, ADS, M> * serializer, MaPI_MapReduce<R, RankAndStop, int, pair<R, double> , R> * mapReduce,
			MyMaPIMapper<R, ADS, M> * mapper,MyMaPIReducer<R, ADS, M> * reducer,int argc, char **argv)
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

	vector<NS<R, ADS, M>*> read_ns_list(Scanner& scanner)
	{
		vector < std::string >& list = readList(scanner);

		Scanner* aux;
		std::string tmp;

		vector<NS<R, ADS, M>*> v_ns;

		for (unsigned int i = 0; i < list.size(); i++)
		{
			aux = new Scanner(list.at(i));

			NS<R, ADS, M>* ns;
			readComponent(ns, *aux);

			if(!ns)
				return vector<NS<R, ADS, M>*>();

			v_ns.push_back(ns);

			delete aux;
		}

		if (v_ns.size() == 0)
		{
			cout << "Error: empty ns list." << endl;
			return vector<NS<R, ADS, M>*>();
		}

		delete &list;
		return v_ns;
	}

	vector<Evaluator<R, ADS, M> *> read_ev_list(Scanner& scanner)
	{
		vector < std::string >& list = readList(scanner);

		Scanner* aux;
		std::string tmp;

		vector<Evaluator<R, ADS, M> *> v_ev;

		for (unsigned int i = 0; i < list.size(); i++)
		{
			aux = new Scanner(list.at(i));

			Evaluator<R, ADS, M> * ev;
			readComponent(ev, aux);

			if(!ev)
				return vector<Evaluator<R, ADS, M> *>();

			v_ev.push_back(ev);
		}

		if (v_ev.size() == 0)
		{
			cout << "Error: empty ev list." << endl;
			return vector<Evaluator<R, ADS, M> *>();
		}

		delete &list;
		return v_ev;
	}

	vector<LocalSearch<R, ADS, M>*> read_local_search_list(Scanner& scanner)
	{
		vector < std::string >& list = readList(scanner);
		vector<LocalSearch<R, ADS, M>*> v_heuristics;

		pair<LocalSearch<R, ADS, M>*, std::string> method;

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

	Evaluator<R, ADS, M>* read_ev(Scanner& scanner)
	{
		Evaluator<R, ADS, M>* ev = NULL;
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
	}

	void drop_all()
	{
		map<std::string, vector<OptFrameComponent*> >::iterator iter;

		for(iter = components.begin(); iter != components.end(); iter++)
		{
			vector<OptFrameComponent*> v = iter->second;

			for (unsigned int i = 0; i < v.size(); i++)
				delete v[i];

			iter->second.clear();
		}
	}


	RandGen& getRandGen()
	{
		return rg;
	}

	static vector<std::string>& readList(Scanner& scanner)
	{
		vector < std::string > *list = new vector<std::string> ;
		std::string word;
		char character = scanner.nextChar();
		int numberOfBrackets;

		while (character == ' ')
		{
			character = scanner.nextChar();
		}

		if(character != '[')
		{
			if(( character >= '0' ) && ( character <= '9' )) // syntax 1..n
			{
				std::string number = "";
				number += character;
				character = scanner.nextChar();

				while( ( ( character >= '0' ) && ( character <= '9' ) ) )
				{
					number += character;
					character = scanner.nextChar();
				}

				Scanner toInt(number);
				int firstInt = toInt.nextInt();

				int dots = 0;

				while(( character == ' ' ) || ( character == '.' ))
				{
					if(character == '.')
						dots++;

					character = scanner.nextChar();
				}

				if(dots != 2)
				{
					cout << "Error: expected two dots (..) and found " << dots << " dots!" << endl;
					return *list;
				}

				stringstream rest;
				rest << character << scanner.rest();
				scanner = Scanner(rest.str());

				int secondInt = scanner.nextInt();

				if(firstInt < secondInt)
				{
					for(int i = firstInt; i <= secondInt; i++)
					{
						stringstream toStr;
						toStr << i;
						list->push_back(toStr.str());
					}
				}
				else
					for(int i = firstInt; i >= secondInt; i--)
					{
						stringstream toStr;
						toStr << i;
						list->push_back(toStr.str());
					}

				return *list;
			}
			else
			{
				cout << "Error:! expected '[' and found '" << character << "'!" << endl;
				return *list;
			}
		}


		numberOfBrackets = 0;

		character = scanner.nextChar();

		word = "";
		while ((character != ']') || ((character == ']') && numberOfBrackets > 0))
		{
			if (character == '[')
				numberOfBrackets++;
			if (character == ']')
				numberOfBrackets--;

			if ((character == ',') && (numberOfBrackets == 0))
			{
				list->push_back(word);
				word = "";
			}
			else
			{
				word += character;
			}

			character = scanner.nextChar();
		}
		list->push_back(word);

		for (unsigned int i = 0; i < list->size(); i++)
		{
			list->at(i) = scanner.trim(list->at(i));
		}

		// error in code, dont know why but list should be empty!
		if((list->size()==1) && (list->at(0)==""))
			list->pop_back();

		return *list;
	}


	pair<LocalSearch<R, ADS, M>*, std::string> createLocalSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
			return make_pair(new Empty<R, ADS, M> , "");

		string h = scanner.next();

		if (h == LocalSearch<R, ADS, M>::idComponent())
		{
			unsigned int id = scanner.nextInt();

			LocalSearch<R, ADS, M>* mtd = NULL;

			assign(mtd, id, LocalSearch<R, ADS, M>::idComponent());

			if(!mtd)
				return make_pair(new Empty<R, ADS, M> , scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == Empty<R, ADS, M>::idComponent())
			return make_pair(new Empty<R, ADS, M> , scanner.rest());

		for(unsigned i=0; i<builders.size(); i++)
		{
			// build local search directly by builder name
			if(builders[i]->id()==h)
			{
				LocalSearch<R, ADS, M>* ls = ((LocalSearchBuilder<R,ADS,M>*)(builders[i]))->build(scanner, *this);
				return make_pair(ls, scanner.rest());
			}

			// locate builder by local search name
			if(builders[i]->canBuild(h))
			{
				LocalSearch<R, ADS, M>* ls = ((LocalSearchBuilder<R,ADS,M>*)(builders[i]))->build(scanner, *this);
				return make_pair(ls, scanner.rest());
			}
		}

		cout << "Warning: no LocalSearch '" << h << "' found! ignoring..." << endl;

		return make_pair(new Empty<R, ADS, M> , scanner.rest());
	}


	pair<SingleObjSearch<R, ADS, M>*, std::string> createSingleObjSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
		{
			SingleObjSearch<R, ADS, M>* sios = new EmptySingleObjSearch<R, ADS, M>;
			return make_pair(sios , "");
		}

		string h = scanner.next();

		if (h == SingleObjSearch<R, ADS, M>::idComponent())
		{
			unsigned int id = scanner.nextInt();

			SingleObjSearch<R, ADS, M>* mtd = NULL;

			assign(mtd, id, SingleObjSearch<R, ADS, M>::idComponent());

			if(!mtd)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == EmptySingleObjSearch<R, ADS, M>::idComponent())
			return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());


		if (h == GRASP<R, ADS, M>::idComponent())
		{
			cout << "Heuristic: GRASP" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			string rest = scanner.rest();

			pair<LocalSearch<R, ADS, M>*, string> method;
			method = createLocalSearch(rest);

			LocalSearch<R, ADS, M>* ls = method.first;

			scanner = Scanner(method.second);

			int iter = scanner.nextInt();

			return make_pair(new GRASP<R, ADS, M> (*evaluator, *initsol, *ls, iter), scanner.rest());
		}

		if (h == TabuSearch<R, ADS, M>::idComponent())
		{
			cout << "Heuristic: Tabu Search" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			NSSeq<R, ADS, M>* ns_seq = (NSSeq<R, ADS, M>*) getNextComponent(scanner);

			if(!ns_seq)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			int tamT = scanner.nextInt();
			int BTmax = scanner.nextInt();

			return make_pair(new TabuSearch<R, ADS, M> (*evaluator, *initsol, *ns_seq, tamT, BTmax), scanner.rest());
		}

		if (h == BasicIteratedLocalSearch<R, ADS, M>::idComponent())
		{
			cout << "Heuristic: Basic Iterated Local Search" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());


			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<LocalSearch<R, ADS, M>*, string> method;
			method = createLocalSearch(rest);

			LocalSearch<R, ADS, M>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			BasicILSPerturbation<R, ADS, M>* ils_pert = NULL;
			readComponent(ils_pert, scanner);

			if(!ils_pert)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			int iterMax = scanner.nextInt();

			return make_pair(new BasicIteratedLocalSearch<R, ADS, M> (*evaluator, *initsol, *localSearch, *ils_pert, iterMax), scanner.rest());
		}

		if (h == BasicSimulatedAnnealing<R, ADS, M>::idComponent())
		{
			cout << "Heuristic: Basic Simulated Annealing" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			vector<NS<R, ADS, M>* > ns_list = read_ns_list(scanner);
			if(ns_list.size()==0)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			double alpha = scanner.nextDouble();
			int SAmax = scanner.nextInt();
			double Ti = scanner.nextDouble();

			return make_pair(new BasicSimulatedAnnealing<R, ADS, M> (*evaluator, *initsol, ns_list, alpha, SAmax, Ti, rg), scanner.rest());
		}

		if (h == IteratedLocalSearchLevels<R, ADS, M>::idComponent())
		{
			cout << "Heuristic: Iterated Local Search (Levels)" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());


			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<LocalSearch<R, ADS, M>*, string> method;
			method = createLocalSearch(rest);

			LocalSearch<R, ADS, M>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			ILSLPerturbation<R, ADS, M>* ilsl_pert = NULL;
			readComponent(ilsl_pert, scanner);

			if(!ilsl_pert)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			int iterMax = scanner.nextInt();
			int levelMax = scanner.nextInt();

			return make_pair(new IteratedLocalSearchLevels<R, ADS, M> (*evaluator, *initsol, *localSearch, *ilsl_pert, iterMax, levelMax), scanner.rest());
		}

		if (h == IntensifiedIteratedLocalSearchLevels<R, ADS, M>::idComponent())
		{
			cout << "Heuristic: Intensified Iterated Local Search (Levels)" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(scanner);
			if(!evaluator)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			Constructive<R, ADS>* initsol = NULL;
			readComponent(initsol, scanner);
			if(!initsol)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());


			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<LocalSearch<R, ADS, M>*, string> method;
			method = createLocalSearch(rest);

			LocalSearch<R, ADS, M>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			/*rest = scanner.rest();

				 pair<HTrajectory<R, ADS, M>*, string> method2;
				 method2 = createHeuristic(rest);

				 HTrajectory<R, ADS, M>* localSearch2 = method2.first;

				 scanner = Scanner(method2.second);*/

			Intensification<R, ADS, M> * intensification = NULL;
			readComponent(intensification, scanner);

			if(!intensification)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			// ====================

			ILSLPerturbation<R, ADS, M>* ilsl_pert = NULL;
			readComponent(ilsl_pert, scanner);

			if(!ilsl_pert)
				return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());

			int iterMax = scanner.nextInt();
			int levelMax = scanner.nextInt();

			//return make_pair(new IntensifiedIteratedLocalSearchLevels<R, ADS, M> (*evaluator, *localSearch, *localSearch2, *ilsl_pert, iterMax, levelMax),scanner.rest());
			return make_pair(new IntensifiedIteratedLocalSearchLevels<R, ADS, M> (*evaluator, *initsol, *localSearch, *intensification, *ilsl_pert, iterMax, levelMax), scanner.rest());
		}


#ifdef MaPI
		if (h == "MapReduce")
		{
			cout << "Heuristic: MapReduce" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<HTrajectory<R, ADS, M>*, string> method;
			method = createHeuristic(rest);

			HTrajectory<R, ADS, M>* hmap = method.first;

			scanner = Scanner(method.second);

			// ===================
			// Read next heuristic
			// ===================

			rest = scanner.rest();

			method = createHeuristic(rest);

			HTrajectory<R, ADS, M>* hreduce = method.first;

			scanner = Scanner(method.second);

			// ===================
			mapper->setHeuristic(hmap);
			reducer->setHeuristic(hreduce); // reduz usando a heurística hreduce

			return make_pair(new OptFrameMapReduce<R, ADS, M> (*serializer,*mapReduce,*mapper,*reducer,*evaluator), scanner.rest());
		}

		if (h == "Map")
		{
			cout << "Heuristic: Map" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<HTrajectory<R, ADS, M>*, string> method;
			method = createHeuristic(rest);

			HTrajectory<R, ADS, M>* hmap = method.first;

			scanner = Scanner(method.second);

			// ===================
			mapper->setHeuristic(hmap);
			// A não especificação da heurística de redução implina na
			// redução à melhor solução produzida nos mapeamentos

			return make_pair(new OptFrameMapReduce<R, ADS, M> (*serializer,*mapReduce,*mapper,*reducer,*evaluator), scanner.rest());
		}
#endif

		cout << "Warning: no SingleObjSearch '" << h << "' found! ignoring..." << endl;

		return make_pair(new EmptySingleObjSearch<R, ADS, M> , scanner.rest());
	}

	pair<MultiObjSearch<R, ADS, M>*, std::string> createMultiObjSearch(std::string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
		{
			MultiObjSearch<R, ADS, M>* mios = new EmptyMultiObjSearch<R, ADS,	M>;
			return make_pair(mios, "");
		}

		string h = scanner.next();

		if (h == MultiObjSearch<R, ADS, M>::idComponent())
		{
			unsigned int id = scanner.nextInt();

			MultiObjSearch<R, ADS, M>* mtd = NULL;

			assign(mtd, id, MultiObjSearch<R, ADS, M>::idComponent());

			if (!mtd)
				return make_pair(new EmptyMultiObjSearch<R, ADS, M>, scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == EmptyMultiObjSearch<R, ADS, M>::idComponent())
			return make_pair(new EmptyMultiObjSearch<R, ADS, M>, scanner.rest());

		cout << "Warning: no MultiObjSearch '" << h << "' found! ignoring..." << endl;

		return make_pair(new EmptyMultiObjSearch<R, ADS, M>, scanner.rest());
	}

};

#endif /* HEURISTICFACTORY_HPP_ */
