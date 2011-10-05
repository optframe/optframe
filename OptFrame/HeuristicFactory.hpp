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
	vector<InitialSolution<R, ADS>*> initsol;
	vector<NS<R, ADS, M>*> ns;
	vector<Evaluator<R, ADS, M>*> ev;
	vector<ILSLPerturbation<R, ADS, M>*> ilsl_pert;
	vector<BasicILSPerturbation<R, ADS, M>*> ils_pert;
	vector<Intensification<R, ADS, M>*> ils_int;

	vector<Solution<R, ADS>*> loadsol;
	vector<Heuristic<R, ADS, M>*> method;

	vector<InitialPopulation<R, ADS>*> initpop;
	vector<Selection<R, ADS, M>*> ga_sel;
	vector<Mutation<R, ADS, M>*> ga_mut;
	vector<Crossover<R, ADS, M>*> ga_cross;
	vector<Elitism<R, ADS, M>*> ga_elt;

	vector<Population<R, ADS>*> loadpop;

	RandGen& rg;

public:

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

	Solution<R, ADS>* read_loadsol(Scanner* scanner)
	{
		//cout << "DBG: " << loadsol.size() << endl;

		string tmp = scanner->next();

		if (tmp != "loadsol")
		{
			cout << "Error: expected 'loadsol' and found '" << tmp << "'." << endl;
			return NULL;
		}

		unsigned int loadsol_id = scanner->nextInt();

		if (loadsol.size() <= loadsol_id)
		{
			cout << "Error: solution number " << loadsol_id << " doesn't exist!" << endl;
			return NULL;
		}

		return loadsol[loadsol_id];
	}

	InitialSolution<R, ADS>* read_initsol(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "initsol")
			cout << "Warning: expected 'initsol' and found '" << tmp << "'." << endl;

		unsigned int initsol_id = scanner->nextInt();

		if (initsol.size() <= initsol_id)
		{
			cout << "Error: initsol number " << initsol_id << " doesn't exist!" << endl;
			exit(1);
		}

		return initsol[initsol_id];
	}

	Population<R, ADS>* read_loadpop(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "loadpop")
		{
			cout << "Error: expected 'loadpop' and found '" << tmp << "'." << endl;
			return NULL;
		}

		unsigned int loadpop_id = scanner->nextInt();

		if (loadpop.size() <= loadpop_id)
		{
			cout << "Error: solution number " << loadpop_id << " doesn't exist!" << endl;
			return NULL;
		}

		return loadpop[loadpop_id];
	}

	InitialPopulation<R, ADS>* read_initpop(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "initpop")
		{
			cout << "Error: expected 'initpop' and found '" << tmp << "'." << endl;
			return NULL;
		}

		unsigned int initpop_id = scanner->nextInt();

		if (initpop.size() <= initpop_id)
		{
			cout << "Error: initpop number " << initpop_id << " doesn't exist!" << endl;
			return NULL;
		}

		return initpop[initpop_id];
	}

	NS<R, ADS, M>* read_ns(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ns")
		{
			cout << "Warning: expected 'ns' and found '" << tmp << "'." << endl;
			return NULL;
		}

		unsigned int ns_id = scanner->nextInt();

		if (ns.size() <= ns_id)
		{
			cout << "Error: ns number " << ns_id << " doesn't exist!" << endl;
			return NULL;
		}

		return ns[ns_id];
	}

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
			tmp = aux->next();
			if (tmp != "ns")
			{
				cout << "Error: expected 'ns' and found '" << tmp << "'." << endl;
				return vector<NS<R, ADS, M>*>();
			}

			unsigned int ns_id = aux->nextInt();

			if (ns.size() <= ns_id)
			{
				cout << "Error: ns number " << ns_id << " doesn't exist!" << endl;
				return vector<NS<R, ADS, M>*>();
			}

			v_ns.push_back(ns[ns_id]);

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
			tmp = aux->next();
			if (tmp != "ev")
			{
				cout << "Error: expected 'ev' and found '" << tmp << "'." << endl;
				return vector<Evaluator<R, ADS, M> *>();
			}

			unsigned int ev_id = aux->nextInt();

			if (ev.size() <= ev_id)
			{
				cout << "Error: ev number " << ev_id << " doesn't exist!" << endl;
				return vector<Evaluator<R, ADS, M> *>();
			}

			v_ev.push_back(ev[ev_id]);

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
		string tmp = scanner->next();

		if (tmp == "moev")
		{
			vector<Evaluator<R, ADS, M> *> evs = read_ev_list(scanner);

			MultiObjectiveEvaluator<R, ADS, M>* moev = new MultiObjectiveEvaluator<R, ADS, M> (*evs[0]);

			for (unsigned int i = 1; i < evs.size(); i++)
				moev->add(*evs[i]);

			return moev;
		}
		else if (tmp != "ev")
		{
			cout << "Error: expected 'ev' and found '" << tmp << "'." << endl;
			return NULL;
		}

		unsigned int ev_id = scanner->nextInt();

		if (ev.size() <= ev_id)
		{
			cout << "Error: ev number " << ev_id << " doesn't exist!" << endl;
			return NULL;
		}

		return ev[ev_id];
	}

	ILSLPerturbation<R, ADS, M>* read_ilsl_pert(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ilsl_pert")
		{
			cout << "Error: expected 'ilsl_pert' and found '" << tmp << "'." << endl;
			return NULL;
		}

		unsigned int ilsl_pert_id = scanner->nextInt();

		if (ilsl_pert.size() <= ilsl_pert_id)
		{
			cout << "Error: 'perturbation levels' number " << ilsl_pert_id << " doesn't exist!" << endl;
			return NULL;
		}

		return ilsl_pert[ilsl_pert_id];
	}

	BasicILSPerturbation<R, ADS, M>* read_ils_pert(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ils_pert")
		{
			cout << "Error: expected 'ils_pert' and found '" << tmp << "'." << endl;
			return NULL;
		}

		unsigned int ils_pert_id = scanner->nextInt();

		if (ils_pert.size() <= ils_pert_id)
		{
			cout << "Error: 'perturbation levels' number " << ils_pert_id << " doesn't exist!" << endl;
			return NULL;
		}

		return ils_pert[ils_pert_id];
	}

	Intensification<R, ADS, M> * read_ils_int(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ils_int")
		{
			cout << "Error: expected 'ils_int' and found '" << tmp << "'." << endl;
			return NULL;
		}

		unsigned int ils_int_id = scanner->nextInt();

		if (ils_int.size() <= ils_int_id)
		{
			cout << "Error: 'intensification' number " << ils_int_id << " doesn't exist!" << endl;
			return NULL;
		}

		return ils_int[ils_int_id];
	}

	Selection<R, ADS, M>* read_ga_sel(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ga_sel")
		{
			cout << "Error: expected 'ga_sel' and found '" << tmp << "'." << endl;
			return NULL;
		}

		int ga_sel_id = scanner->nextInt();

		if (ga_sel.size() <= ga_sel_id)
		{
			cout << "Error: 'genetic selection' number " << ga_sel_id << " doesn't exist!" << endl;
			return NULL;
		}

		return ga_sel[ga_sel_id];
	}

	Mutation<R, ADS, M>* read_ga_mut(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ga_mut")
		{
			cout << "Error: expected 'ga_mut' and found '" << tmp << "'." << endl;
			return NULL;
		}

		int ga_mut_id = scanner->nextInt();

		if (ga_mut.size() <= ga_mut_id)
		{
			cout << "Error: 'genetic mutation' number " << ga_mut_id << " doesn't exist!" << endl;
			return NULL;
		}

		return ga_mut[ga_mut_id];
	}

	Elitism<R, ADS, M>* read_ga_elt(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ga_elt")
		{
			cout << "Error: expected 'ga_elt' and found '" << tmp << "'." << endl;
			return NULL;
		}

		int ga_elt_id = scanner->nextInt();

		if (ga_elt.size() <= ga_elt_id)
		{
			cout << "Error: 'genetic elitism' number " << ga_elt_id << " doesn't exist!" << endl;
			return NULL;
		}

		return ga_elt[ga_elt_id];
	}

	Crossover<R, ADS, M>* read_ga_cross(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ga_cross")
		{
			cout << "Error: expected 'ga_cross' and found '" << tmp << "'." << endl;
			return NULL;
		}

		int ga_cross_id = scanner->nextInt();

		if (ga_cross.size() <= ga_cross_id)
		{
			cout << "Error: 'genetic crossover' number " << ga_cross_id << " doesn't exist!" << endl;
			return NULL;
		}

		return ga_cross[ga_cross_id];
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
		for (unsigned int i = 0; i < ns.size(); i++)
			delete ns[i];
                ns.clear();

		for (unsigned int i = 0; i < ev.size(); i++)
			delete ev[i];
                ev.clear();

		for (unsigned int i = 0; i < initsol.size(); i++)
			delete initsol[i];
                initsol.clear();

		for (unsigned int i = 0; i < loadsol.size(); i++)
			delete loadsol[i];
                loadsol.clear();

		for (unsigned int i = 0; i < method.size(); i++)
			delete method[i];
                method.clear();

		for (unsigned int i = 0; i < initpop.size(); i++)
			delete initpop[i];
                initpop.clear();

		for (unsigned int i = 0; i < loadpop.size(); i++)
			delete loadpop[i];
                loadpop.clear();

		for (unsigned int i = 0; i < ilsl_pert.size(); i++)
			delete ilsl_pert[i];
                ilsl_pert.clear();

		for (unsigned int i = 0; i < ils_pert.size(); i++)
			delete ils_pert[i];
                ils_pert.clear();

		for (unsigned int i = 0; i < ils_int.size(); i++)
			delete ils_int[i];
                ils_int.clear();

		for (unsigned int i = 0; i < ga_sel.size(); i++)
			delete ga_sel[i];
                ga_sel.clear();

		for (unsigned int i = 0; i < ga_mut.size(); i++)
			delete ga_mut[i];
                ga_mut.clear();

		for (unsigned int i = 0; i < ga_cross.size(); i++)
			delete ga_cross[i];
                ga_cross.clear();

		for (unsigned int i = 0; i < ga_elt.size(); i++)
			delete ga_elt[i];
                ga_elt.clear();
   }

	int add_method(Heuristic<R, ADS, M>* _method)
	{
		method.push_back(_method);
		return method.size() - 1;
	}

	Heuristic<R, ADS, M>* get_method(int index)
	{
		return method[index];
	}

	int add_loadsol(Solution<R, ADS>* _loadsol)
	{
		loadsol.push_back(_loadsol);
		return loadsol.size() - 1;
	}

	Solution<R, ADS>* get_loadsol(int index)
	{
		return loadsol[index];
	}

	int add_initsol(InitialSolution<R, ADS>* _initsol)
	{
		initsol.push_back(_initsol);
		return initsol.size() - 1;
	}

	int add_loadpop(Population<R, ADS>* _loadpop)
	{
		loadpop.push_back(_loadpop);
		return loadpop.size() - 1;
	}

	Population<R, ADS>* get_loadpop(int index)
	{
		return loadpop[index];
	}

	int add_initpop(InitialPopulation<R, ADS>* _initpop)
	{
		initpop.push_back(_initpop);
		return initpop.size() - 1;
	}

	InitialSolution<R, ADS>* get_initsol(int index)
	{
		return initsol[index];
	}

	Population<R, ADS>* get_initpop(int index)
	{
		return initpop[index];
	}

	int add_ns(NS<R, ADS, M>* _ns)
	{
		ns.push_back(_ns);
		return ns.size() - 1;
	}

	NS<R, ADS, M>* get_ns(int index)
	{
		return ns[index];
	}

	int add_ev(Evaluator<R, ADS, M>* _ev)
	{
		ev.push_back(_ev);
		return ev.size() - 1;
	}

	Evaluator<R, ADS, M>* get_ev(int index)
	{
		return ev[index];
	}

	int add_ilsl_pert(ILSLPerturbation<R, ADS, M>* _ilsl_pert)
	{
		ilsl_pert.push_back(_ilsl_pert);
		return ilsl_pert.size() - 1;
	}

	int add_ils_pert(BasicILSPerturbation<R, ADS, M>* _ils_pert)
	{
		ils_pert.push_back(_ils_pert);
		return ils_pert.size() - 1;
	}

	void add_ils_int(Intensification<R, ADS, M>* _ils_int)
	{
		ils_int.push_back(_ils_int);
	}

	ILSLPerturbation<R, ADS, M>* get_ilsl_pert(int index)
	{
		return ilsl_pert[index];
	}

	BasicILSPerturbation<R, ADS, M>* get_ils_pert(int index)
	{
		return ils_pert[index];
	}

	int add_ga_mut(Mutation<R, ADS, M>* _ga_mut)
	{
		ga_mut.push_back(_ga_mut);
		return ga_mut.size() - 1;
	}

	Mutation<R, ADS, M>* get_ga_mut(int index)
	{
		return ga_mut[index];
	}

	int add_ga_sel(Selection<R, ADS, M>* _ga_sel)
	{
		ga_sel.push_back(_ga_sel);
		return ga_sel.size() - 1;
	}

	int add_ga_elt(Elitism<R, ADS, M>* _ga_elt)
	{
		ga_elt.push_back(_ga_elt);
		return ga_elt.size() - 1;
	}

	Selection<R, ADS, M>* get_ga_sel(int index)
	{
		return ga_sel[index];
	}

	Elitism<R, ADS, M>* get_ga_elt(int index)
	{
		return ga_elt[index];
	}

	int add_ga_cross(Crossover<R, ADS, M>* _ga_cross)
	{
		ga_cross.push_back(_ga_cross);
		return ga_cross.size() - 1;
	}

	Crossover<R, ADS, M>* get_ga_cross(int index)
	{
		return ga_cross[index];
	}

	int initsol_size()
	{
		return initsol.size();
	}

	int initpop_size()
	{
		return initpop.size();
	}

	int ns_size()
	{
		return ns.size();
	}

	int ev_size()
	{
		return ev.size();
	}

	int loadsol_size()
	{
		return loadsol.size();
	}

	int loadpop_size()
	{
		return loadpop.size();
	}

	int method_size()
	{
		return method.size();
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

		if (h == "method")
		{
			unsigned int id = scanner.nextInt();

			if(id >= method.size())
			{
	         cout << "Error: method number " << id << " doesn't exist!" << endl;
	         return make_pair(new Empty<R, ADS, M> , scanner.rest());
			}

			return make_pair(method[id], scanner.rest());
		}

		if (h == "Empty")
			return make_pair(new Empty<R, ADS, M> , scanner.rest());

		if (h == "BI")
		{
			cout << "Heuristic: Best Improvement" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			NSSeq<R, ADS, M>* ns_seq = (NSSeq<R, ADS, M>*) read_ns(&scanner);
			if(!ns_seq)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			return make_pair(new BestImprovement<R, ADS, M> (*evaluator, *ns_seq), scanner.rest());
		}

		if (h == "FI")
		{
			cout << "Heuristic: First Improvement" << endl;

			Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
			if(!evaluator)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			NSSeq<R, ADS, M>* ns_seq = (NSSeq<R, ADS, M>*) read_ns(&scanner);
			if(!ns_seq)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			return make_pair(new FirstImprovement<R, ADS, M> (*evaluator, *ns_seq), scanner.rest());
		}

	   if(h == "CS")
      {
         cout << "Heuristic: Circular Search" << endl;

         Evaluator<R, ADS, M>* evaluator = read_ev(&scanner);
         NSEnum<R, ADS, M>* ns_enum = (NSEnum<R, ADS, M>*) read_ns(&scanner);

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

			NS<R, ADS, M>* ns = (NS<R, ADS, M>*) read_ns(&scanner);
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

			InitialSolution<R, ADS>* initsol = read_initsol(&scanner);
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

			NSSeq<R, ADS, M>* ns = (NSSeq<R, ADS, M>*) read_ns(&scanner);
			if(!ns)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			int tamT = scanner.nextInt();
			int BTmax = scanner.nextInt();

			return make_pair(new TabuSearch<R, ADS, M> (*evaluator, *ns, tamT, BTmax), scanner.rest());
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

			BasicILSPerturbation<R, ADS, M>* ils_pert = read_ils_pert(&scanner);
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

			ILSLPerturbation<R, ADS, M>* ilsl_pert = read_ilsl_pert(&scanner);
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

			Intensification<R, ADS, M> * intensification = read_ils_int(&scanner);
			if(!intensification)
			   return make_pair(new Empty<R, ADS, M> , scanner.rest());

			// ====================

			ILSLPerturbation<R, ADS, M>* ilsl_pert = read_ilsl_pert(&scanner);
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

			ILSLPerturbation<R, ADS, M>* ilsl_pert = read_ilsl_pert(&scanner);
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
