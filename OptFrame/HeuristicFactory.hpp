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

using namespace std;

#include "./Scanner++/Scanner.h"

#include "RandGen.hpp"

#include "OptFrameContainer.hpp"

#include "Heuristic.hpp"

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
#include "Heuristics/IteratedLocalSearch.hpp"
#include "Heuristics/BasicIteratedLocalSearch.hpp"
#include "Heuristics/IteratedLocalSearchLevels.hpp"
#include "Heuristics/IntensifiedIteratedLocalSearchLevels.hpp"
#include "Heuristics/Intensification.hpp"
#include "Heuristics/MultiHeuristic.hpp"
#include "Heuristics/GRASP.hpp"
#include "Heuristics/TabuSearch.hpp"
#include "Heuristics/EvolutionaryAlgorithms/GeneticAlgorithm.hpp"
#include "Heuristics/BasicSimulatedAnnealing.hpp"

// design pattern: Factory

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class HeuristicFactory
{
private:
	RandGen& rg;

public:

	map<string, vector<OptFrameComponent*> > components;

	OptFrameComponent* getNextComponent(Scanner* scanner)
   {
	   string id = scanner->next();
	   unsigned number = scanner->nextInt();

      OptFrameComponent* component = NULL;

      if(components.count(id) > 0)
      {
         vector<OptFrameComponent*> v = components[id];

         if(number < v.size())
            component = v[number];
      }
      else
         cout << "'" << id << "' not found!" << endl;

      return component;
   }

	template< class T > void assign(T*& component, string id, unsigned number)
   {
      if(components.count(id) > 0)
      {
         vector<OptFrameComponent*> v = components[id];
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

	int addComponent(OptFrameComponent& component, string id)
   {
      if(!component.compatible(id))
      {
         cout << "Warning: incompatible components '";
         cout << component.id() << "' and '" << id << "'!" << endl;

         return -1;
      }

      vector<OptFrameComponent*> v = components[id];
      v.push_back(&component);
      components[id] = v;

      int idx = components[id].size() - 1;

      return idx;
   }

	int addComponent(OptFrameComponent& component)
   {
      return addComponent(component, component.id());
   }

	template< class T > void readComponent(T*& component, Scanner* scanner)
   {
      string tmp = scanner->next();

      if(tmp != T::idComponent())
      {
         cout << "Error: expected '" << T::idComponent() << "' and found '" << tmp << "'." << endl;
         component = NULL;

         return;
      }

      unsigned int number = scanner->nextInt();

      component = NULL;

      assign(component, tmp, number);
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


	int read_np(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "np")
		{
			cout << "Error: expected 'np' and found '" << tmp << "'." << endl;
			return -1;
		}

		int np = scanner->nextInt();

		if (np <= 1)
		{
			cout << "Error: number or processes " << np << " must be greater than or equal 2" << endl;
			return -1;
		}

		return np;
	}

	vector<NS<R, ADS, M>*> read_ns_list(Scanner* scanner)
	{
		vector < string > list = readList(*scanner);

		Scanner* aux;
		string tmp;

		vector<NS<R, ADS, M>*> v_ns;

		for (unsigned int i = 0; i < list.size(); i++)
		{
			aux = new Scanner(list.at(i));

         NS<R, ADS, M>* ns;
         readComponent(ns, aux);

         if(!ns)
            return vector<NS<R, ADS, M>*>();

			v_ns.push_back(ns);
		}

		if (v_ns.size() == 0)
		{
			cout << "Error: empty ns list." << endl;
			return vector<NS<R, ADS, M>*>();
		}

		return v_ns;
	}

	vector<Evaluator<R, ADS, M> *> read_ev_list(Scanner* scanner)
	{
		vector < string > list = readList(*scanner);

		Scanner* aux;
		string tmp;

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

		return v_ev;
	}

	vector<Heuristic<R, ADS, M>*> read_heuristic_list(Scanner* scanner)
	{
		vector < string > list = readList(*scanner);
		vector<Heuristic<R, ADS, M>*> v_heuristics;

		pair<Heuristic<R, ADS, M>*, string> method;

		for (unsigned int i = 0; i < list.size(); i++)
		{
			method = createHeuristic(list.at(i));
			v_heuristics.push_back(method.first);
		}

		if (v_heuristics.size() == 0)
		{
			cout << "Error: empty heuristic list." << endl;
			return v_heuristics;
		}

		return v_heuristics;
	}

	Evaluator<R, ADS, M>* read_ev(Scanner* scanner)
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
      map<string, vector<OptFrameComponent*> >::iterator iter;

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

	static vector<string>& readList(Scanner& scanner)
	{
		vector < string > *list = new vector<string> ;
		string word;
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
            string number = "";
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

		return *list;
	}

	pair<Heuristic<R, ADS, M>*, string> createHeuristic(string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
			return make_pair(new Empty<R, ADS, M> , "");

		string h = scanner.next();

		if (h == "OptFrame:method")
		{
			unsigned int id = scanner.nextInt();

			Heuristic<R, ADS, M>* mtd = NULL;

			assign(mtd, "OptFrame:method", id);

			if(!mtd)
	         return make_pair(new Empty<R, ADS, M> , scanner.rest());

			return make_pair(mtd, scanner.rest());
		}

		if (h == "Empty")
			return make_pair(new Empty<R, ADS, M> , scanner.rest());

		if (h == "OptFrame:bi")
		{
			cout << "Heuristic: Best Improvement" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			NSSeq<R, ADS, M>* ns_seq = (NSSeq<R, ADS, M>*) getNextComponent(&scanner);

			if(!ns_seq)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			cout << "ok" << endl;

			return make_pair(new BestImprovement<R, ADS, M> (*evaluator, *ns_seq), scanner.rest());
		}

		if (h == "FI")
		{
			cout << "Heuristic: First Improvement" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

         NSSeq<R, ADS, M>* ns_seq = (NSSeq<R, ADS, M>*) getNextComponent(&scanner);

			if(!ns_seq)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			return make_pair(new FirstImprovement<R, ADS, M> (*evaluator, *ns_seq), scanner.rest());
		}

	   if(h == "CS")
      {
         cout << "Heuristic: Circular Search" << endl;

         Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
         NSEnum<R, ADS, M>* ns_enum = NULL;
         readComponent(ns_enum, &scanner);

         return make_pair(new CircularSearch<R, ADS, M> (*evaluator, *ns_enum), scanner.rest());
      }

		if (h == "HC")
		{
			cout << "Heuristic: Hill Climbing" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);

			string rest = scanner.rest();

			pair<Heuristic<R, ADS, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, ADS, M>* h = method.first;

			scanner = Scanner(method.second);

			return make_pair(new HillClimbing<R, ADS, M> (*evaluator, *h), scanner.rest());
		}

		if (h == "RDM")
		{
			cout << "Heuristic: Random Descent Method" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

         NS<R, ADS, M>* ns = (NS<R, ADS, M>*) getNextComponent(&scanner);

			if(!ns)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			int iter = scanner.nextInt();

			return make_pair(new RandomDescentMethod<R, ADS, M> (*evaluator, *ns, iter), scanner.rest());
		}

		if (h == "GRASP")
		{
			cout << "Heuristic: GRASP" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			InitialSolution<R, ADS>* initsol = NULL;
			readComponent(initsol, &scanner);

			if(!initsol)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			string rest = scanner.rest();

			pair<Heuristic<R, ADS, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, ADS, M>* h = method.first;

			scanner = Scanner(method.second);

			int iter = scanner.nextInt();

			return make_pair(new GRASP<R, ADS, M> (*evaluator, *initsol, *h, iter), scanner.rest());
		}

		if (h == "TS")
		{
			cout << "Heuristic: Tabu Search" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

         NSSeq<R, ADS, M>* ns_seq = (NSSeq<R, ADS, M>*) getNextComponent(&scanner);

			if(!ns_seq)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			int tamT = scanner.nextInt();
			int BTmax = scanner.nextInt();

			return make_pair(new TabuSearch<R, ADS, M> (*evaluator, *ns_seq, tamT, BTmax), scanner.rest());
		}

		if (h == "ILS")
		{
			cout << "Heuristic: Basic Iterated Local Search" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<Heuristic<R, ADS, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, ADS, M>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			BasicILSPerturbation<R, ADS, M>* ils_pert = NULL;
			readComponent(ils_pert, &scanner);

			if(!ils_pert)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			int iterMax = scanner.nextInt();

			return make_pair(new BasicIteratedLocalSearch<R, ADS, M> (*evaluator, *localSearch, *ils_pert, iterMax), scanner.rest());
		}

		if (h == "SA")
		{
			cout << "Heuristic: Basic Simulated Annealing" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			vector<NS<R, ADS, M>* > ns_list = read_ns_list(&scanner);
			if(ns_list.size()==0)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			double alpha = scanner.nextDouble();
			int SAmax = scanner.nextInt();
			double Ti = scanner.nextDouble();

			return make_pair(new BasicSimulatedAnnealing<R, ADS, M> (*evaluator, ns_list, alpha, SAmax, Ti, rg), scanner.rest());
		}

		if (h == "ILSL")
		{
			cout << "Heuristic: Iterated Local Search (Levels)" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<Heuristic<R, ADS, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, ADS, M>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			ILSLPerturbation<R, ADS, M>* ilsl_pert = NULL;
         readComponent(ilsl_pert, &scanner);

			if(!ilsl_pert)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			int iterMax = scanner.nextInt();
			int levelMax = scanner.nextInt();

			return make_pair(new IteratedLocalSearchLevels<R, ADS, M> (*evaluator, *localSearch, *ilsl_pert, iterMax, levelMax), scanner.rest());
		}

		if (h == "IILSL")
		{
			cout << "Heuristic: Intensified Iterated Local Search (Levels)" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<Heuristic<R, ADS, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, ADS, M>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			/*rest = scanner.rest();

			 pair<Heuristic<R, ADS, M>*, string> method2;
			 method2 = createHeuristic(rest);

			 Heuristic<R, ADS, M>* localSearch2 = method2.first;

			 scanner = Scanner(method2.second);*/

			Intensification<R, ADS, M> * intensification = NULL;
         readComponent(intensification, &scanner);

			if(!intensification)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			// ====================

			ILSLPerturbation<R, ADS, M>* ilsl_pert = NULL;
         readComponent(ilsl_pert, &scanner);

			if(!ilsl_pert)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			int iterMax = scanner.nextInt();
			int levelMax = scanner.nextInt();

			//return make_pair(new IntensifiedIteratedLocalSearchLevels<R, ADS, M> (*evaluator, *localSearch, *localSearch2, *ilsl_pert, iterMax, levelMax),scanner.rest());
			return make_pair(new IntensifiedIteratedLocalSearchLevels<R, ADS, M> (*evaluator, *localSearch, *intensification, *ilsl_pert, iterMax, levelMax), scanner.rest());
		}

		if (h == "VND")
		{
			cout << "Heuristic: Variable Neighborhood Descent" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			vector<Heuristic<R, ADS, M>*> hlist = read_heuristic_list(&scanner);
			if(hlist.size()==0)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			return make_pair(new VariableNeighborhoodDescent<R, ADS, M> (*evaluator, hlist), scanner.rest());

		}

		if (h == "RVND")
		{
			cout << "Heuristic: RVND" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			vector<Heuristic<R, ADS, M>*> hlist = read_heuristic_list(&scanner);
			if(hlist.size()==0)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			return make_pair(new RVND<R, ADS, M> (*evaluator, hlist, rg), scanner.rest());
		}

		if (h == "VNS")
		{
			cout << "Heuristic: Variable Neighborhood Search" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<Heuristic<R, ADS, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, ADS, M>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			ILSLPerturbation<R, ADS, M>* ilsl_pert = NULL;
         readComponent(ilsl_pert, &scanner);

			if(!ilsl_pert)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			int iterMax = scanner.nextInt();
			int levelMax = scanner.nextInt();

			return make_pair(new IteratedLocalSearchLevels<R, ADS, M> (*evaluator, *localSearch, *ilsl_pert, iterMax, levelMax), scanner.rest());
		}

		if (h == "MH")
		{
			cout << "Heuristic: MultiHeuristic" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			vector<Heuristic<R, ADS, M>*> hlist = read_heuristic_list(&scanner);
			if(hlist.size()==0)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			return make_pair(new MultiHeuristic<R, ADS, M> (*evaluator, hlist), scanner.rest());
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

			pair<Heuristic<R, ADS, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, ADS, M>* hmap = method.first;

			scanner = Scanner(method.second);

			// ===================
			// Read next heuristic
			// ===================

			rest = scanner.rest();

			method = createHeuristic(rest);

			Heuristic<R, ADS, M>* hreduce = method.first;

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

			pair<Heuristic<R, ADS, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, ADS, M>* hmap = method.first;

			scanner = Scanner(method.second);

			// ===================
			mapper->setHeuristic(hmap);
			// A não especificação da heurística de redução implina na
			// redução à melhor solução produzida nos mapeamentos

			return make_pair(new OptFrameMapReduce<R, ADS, M> (*serializer,*mapReduce,*mapper,*reducer,*evaluator), scanner.rest());
		}
#endif

		cout << "Warning: no heuristic '" << h << "' found! ignoring..." << endl;

		return make_pair(new Empty<R, ADS, M> , scanner.rest());
	}

};

#endif /* HEURISTICFACTORY_HPP_ */
