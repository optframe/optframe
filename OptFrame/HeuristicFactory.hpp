#ifndef HEURISTICFACTORY_HPP_
#define HEURISTICFACTORY_HPP_

#include <iostream>

using namespace std;

#include "./Util/Scanner++/Scanner.h"

#include "Heuristic.hpp"

#include "Heuristic/Empty.hpp"

//Heuristics
#include "Heuristic/RandomDescentMethod.hpp"
#include "Heuristic/HillClimbing.hpp"
#include "Heuristic/BestImprovement.hpp"
#include "Heuristic/FirstImprovement.hpp"
#include "Heuristic/VariableNeighborhoodDescent.hpp"
#include "Heuristic/RVND.hpp"

//Metaheuristics
#include "Heuristic/IteratedLocalSearch.hpp"
#include "Heuristic/IteratedLocalSearchLevels.hpp"
#include "Heuristic/IntensifiedIteratedLocalSearchLevels.hpp"
#include "Heuristic/Intensification.hpp"
#include "Heuristic/MultiHeuristic.hpp"
#include "Heuristic/GRASP.hpp"
#include "Heuristic/TabuSearch.hpp"
#include "Heuristic/EvolutionaryAlgorithms/GeneticAlgorithm.hpp"

//Parallel Support
//#include "Parallel/Parallel.h"

/*
 #include "Heuristic/Descida.hpp"
 #include "Heuristic/SimulatedAnnealing.hpp"
 #include "Heuristic/BuscaTabu.hpp"
 #include "Heuristic/IteratedLocalSearch/IteratedLocalSearch.hpp"
 #include "Heuristic/IteratedLocalSearch/IteratedLocalSearchLevels.hpp"
 #include "Heuristic/IteratedLocalSearch/PerturbationLPlus2.hpp"
 /////#include "Heuristic/NonLinearSimulatedAnnealing.hpp"
 #include "Heuristic/MultiStart.hpp"
 #include "Heuristic/VariableNeighborhoodDescent.hpp"
 #include "Heuristic/RVND.hpp"
 #include "Heuristic/VariableHeuristicDescent.hpp"
 #include "Heuristic/AlgoritmosGeneticos/AlgoritmosGeneticos.hpp"
 #include "Heuristic/AlgoritmosMemeticos/AlgoritmosMemeticos.hpp"
 #include "Heuristic/MultiHeuristic.hpp"
 #include "Heuristic/IteratedLocalSearch/IntensifiedIteratedLocalSearch.hpp"
 #include "Heuristic/IteratedLocalSearch/IntensifiedIteratedLocalSearchLevels.hpp"
 #include "Heuristic/IteratedLocalSearch/Intensification.hpp"
 #include "Heuristic/VNS.hpp"
 */

// design pattern: Factory

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class HeuristicFactory
{
private:
	vector<InitialSolution<R>*> initsol;
	vector<NS<R, M>*> ns;
	vector<Evaluator<R, M>*> ev;
	vector<ILSLPerturbation<R, M>*> ilsl_pert;
	vector<Intensification<R,M>*> ils_int;

	vector<Solution<R>*> loadsol;
	vector<Heuristic<R, M>*> method;

	vector<InitialPopulation<R>*> initpop;
	vector<Selection<R, M>*> ga_sel;
	vector<Mutation<R, M>*> ga_mut;
	vector<Crossover<R, M>*> ga_cross;
	vector<Elitism<R, M>*> ga_elt;

	//typedef Solution<R> chromossome;
	//typedef vector<chromossome*> Population;
	//vector<Population*> loadpop;
	vector<Population<R>*> loadpop;

public:


#ifdef MaPI
	MyMaPISerializer<R, M> * serializer;
	MaPI_MapReduce<R, RankAndStop, int, pair<R, double> , R> * mapReduce;
	MyMaPIMapper<R, M> * mapper;
	MyMaPIReducer<R, M> * reducer;
	int argc;
	char **argv;

	void setMapReduce(MyMaPISerializer<R, M> * serializer, MaPI_MapReduce<R, RankAndStop, int, pair<R, double> , R> * mapReduce,
			MyMaPIMapper<R, M> * mapper,MyMaPIReducer<R, M> * reducer,int argc, char **argv)
	{
		this->serializer = serializer;
		this->mapReduce = mapReduce;
		this->mapper = mapper;
		this->reducer = reducer;
		this->argc = argc;
		this->argv = argv;
	}
#endif/**/

	Solution<R>* read_loadsol(Scanner* scanner)
	{
		//cout << "DBG: " << loadsol.size() << endl;

		string tmp = scanner->next();

		if (tmp != "loadsol")
			cout << "Warning: expected 'loadsol' and found '" << tmp << "'." << endl;

		int loadsol_id = scanner->nextInt();

		if (loadsol.size() <= loadsol_id)
		{
			cout << "Error: solution number " << loadsol_id << " doesn't exist!" << endl;
			exit(1);
		}

		return loadsol[loadsol_id];
	}

	InitialSolution<R>* read_initsol(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "initsol")
			cout << "Warning: expected 'initsol' and found '" << tmp << "'." << endl;

		int initsol_id = scanner->nextInt();

		if (initsol.size() <= initsol_id)
		{
			cout << "Error: init number " << initsol_id << " doesn't exist!" << endl;
			exit(1);
		}

		return initsol[initsol_id];
	}

	Population<R>* read_loadpop(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "loadpop")
			cout << "Warning: expected 'loadpop' and found '" << tmp << "'." << endl;

		int loadpop_id = scanner->nextInt();

		if (loadpop.size() <= loadpop_id)
		{
			cout << "Error: solution number " << loadpop_id << " doesn't exist!" << endl;
			exit(1);
		}

		return loadpop[loadpop_id];
	}

	InitialPopulation<R>* read_initpop(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "initpop")
			cout << "Warning: expected 'initpop' and found '" << tmp << "'." << endl;

		int initpop_id = scanner->nextInt();

		if (initpop.size() <= initpop_id)
		{
			cout << "Error: init number " << initpop_id << " doesn't exist!" << endl;
			exit(1);
		}

		return initpop[initpop_id];
	}

	NS<R, M>* read_ns(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ns")
			cout << "Warning: expected 'ns' and found '" << tmp << "'." << endl;

		int ns_id = scanner->nextInt();

		if (ns.size() <= ns_id)
		{
			cout << "Error: ns number " << ns_id << " doesn't exist!" << endl;
			exit(1);
		}

		return ns[ns_id];
	}

	int read_np(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "np")
			cout << "Warning: expected 'np' and found '" << tmp << "'." << endl;

		int np = scanner->nextInt();

		if (np <= 1)
		{
			cout << "Error: number or processes " << np << " must be greater than or equal 2" << endl;
			exit(1);
		}

		return np;
	}

	vector<NS<R, M>*> read_ns_list(Scanner* scanner)
	{
		string tmp = scanner->next();

		vector<NS<R, M>*> v_ns;

		if (tmp != "[")
		{
			cout << "Error: expected '[' and found '" << tmp << "'!" << endl;

			if (tmp == "ns")
			{
				int ns_id = scanner->nextInt();

				if (ns.size() <= ns_id)
				{
					cout << "Error: ns number " << ns_id << " doesn't exist!" << endl;
					exit(1);
				}

				v_ns.push_back(ns[ns_id]);
			}

			return v_ns; // returning empty list
		}

		tmp = scanner->next();

		while (tmp != "]")
		{
			if (tmp != "ns")
				cout << "Warning: expected 'ns' and found '" << tmp << "'." << endl;

			int ns_id = scanner->nextInt();

			if (ns.size() <= ns_id)
			{
				cout << "Error: ns number " << ns_id << " doesn't exist!" << endl;
				exit(1);
			}

			v_ns.push_back(ns[ns_id]);

			tmp = scanner->next();
		}

		if (v_ns.size() == 0)
		{
			cout << "Error: empty ns list." << endl;
			exit(1);
		}

		return v_ns;
	}

	vector<Evaluator<R, M> *> read_ev_list(Scanner* scanner)
	{
		string tmp = scanner->next();

		vector<Evaluator<R, M> *> v_ev;

		if (tmp != "[")
		{
			cout << "Error: expected '[' and found '" << tmp << "'." << endl;
			exit(1);
		}

		tmp = scanner->next();

		while (tmp != "]")
		{
			if (tmp != "ev")
				cout << "Warning: expected 'ev' and found '" << tmp << "'." << endl;

			int ev_id = scanner->nextInt();

			if (ev.size() <= ev_id)
			{
				cout << "Error: ev number " << ev_id << " doesn't exist!" << endl;
				exit(1);
			}

			v_ev.push_back(ev[ev_id]);

			tmp = scanner->next();
		}

		if (v_ev.size() == 0)
		{
			cout << "Error: empty ev list." << endl;
			exit(1);
		}

		return v_ev;
	}

	vector<Heuristic<R, M>*> read_heuristic_list(Scanner* scanner)
	{
		string tmp = scanner->next();

		vector<Heuristic<R, M>*> v_heuristics;

		if (tmp != "[")
		{
			cout << "Error: expected '[' and found '" << tmp << "'!" << endl;

			return v_heuristics; // returning empty list
		}

		pair<Heuristic<R, M>*, string> method;

		string rest = scanner->rest();

		while (rest[1] != ']')
		{

			if (rest.empty())
				break;

			method = createHeuristic(rest);

			v_heuristics.push_back(method.first);

			rest = method.second;

		}

		if (v_heuristics.size() == 0)
		{
			cout << "Error: empty heuristic list." << endl;
			return v_heuristics;
		}

		(*scanner) = Scanner(rest);

		if (scanner->next() != "]")
			cout << "Warning: expected ']'." << endl;

		return v_heuristics;
	}

	Evaluator<R, M>* read_ev(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp == "moev")
		{
			vector<Evaluator<R, M> *> evs = read_ev_list(scanner);

			MultiObjectiveEvaluator<R, M>* moev = new MultiObjectiveEvaluator<R, M> (*evs[0]);

			for (int i = 1; i < evs.size(); i++)
				moev->add(*evs[i]);

			return moev;
		}
		else if (tmp != "ev")
			cout << "Warning: expected 'ev' and found '" << tmp << "'." << endl;

		int ev_id = scanner->nextInt();

		if (ev.size() <= ev_id)
		{
			cout << "Error: ev number " << ev_id << " doesn't exist!" << endl;
			exit(1);
		}

		return ev[ev_id];
	}

	ILSLPerturbation<R, M>* read_ilsl_pert(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ilsl_pert")
			cout << "Warning: expected 'ilsl_pert' and found '" << tmp << "'." << endl;

		int ilsl_pert_id = scanner->nextInt();

		if (ilsl_pert.size() <= ilsl_pert_id)
		{
			cout << "Error: 'perturbation levels' number " << ilsl_pert_id << " doesn't exist!" << endl;
			exit(1);
		}

		return ilsl_pert[ilsl_pert_id];
	}

	Intensification<R,M> * read_ils_int(Scanner* scanner)
	{
		string tmp = scanner->next();

		if(tmp != "ils_int")
			cout << "Warning: expected 'ils_int' and found '"<<tmp<<"'."<<endl;

		int ils_int_id = scanner->nextInt();

		if(ils_int.size()<=ils_int_id)
		{
			cout << "Error: 'intensification' number "<<ils_int_id<<" doesn't exist!"<<endl;
			exit(1);
		}

		return ils_int[ils_int_id];
	}

	Selection<R, M>* read_ga_sel(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ga_sel")
			cout << "Warning: expected 'ga_sel' and found '" << tmp << "'." << endl;

		int ga_sel_id = scanner->nextInt();

		if (ga_sel.size() <= ga_sel_id)
		{
			cout << "Error: 'genetic selection' number " << ga_sel_id << " doesn't exist!" << endl;
			exit(1);
		}

		return ga_sel[ga_sel_id];
	}

	Mutation<R, M>* read_ga_mut(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ga_mut")
			cout << "Warning: expected 'ga_mut' and found '" << tmp << "'." << endl;

		int ga_mut_id = scanner->nextInt();

		if (ga_mut.size() <= ga_mut_id)
		{
			cout << "Error: 'genetic mutation' number " << ga_mut_id << " doesn't exist!" << endl;
			exit(1);
		}

		return ga_mut[ga_mut_id];
	}

	Elitism<R, M>* read_ga_elt(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ga_elt")
			cout << "Warning: expected 'ga_elt' and found '" << tmp << "'." << endl;

		int ga_elt_id = scanner->nextInt();

		if (ga_elt.size() <= ga_elt_id)
		{
			cout << "Error: 'genetic elitism' number " << ga_elt_id << " doesn't exist!" << endl;
			exit(1);
		}

		return ga_elt[ga_elt_id];
	}

	Crossover<R, M>* read_ga_cross(Scanner* scanner)
	{
		string tmp = scanner->next();

		if (tmp != "ga_cross")
			cout << "Warning: expected 'ga_cross' and found '" << tmp << "'." << endl;

		int ga_cross_id = scanner->nextInt();

		if (ga_cross.size() <= ga_cross_id)
		{
			cout << "Error: 'genetic crossover' number " << ga_cross_id << " doesn't exist!" << endl;
			exit(1);
		}

		return ga_cross[ga_cross_id];
	}

	// ================================================================
	// ================================================================

	HeuristicFactory()
	{
	}

	int add_method(Heuristic<R, M>* _method)
	{
		method.push_back(_method);
		return method.size() - 1;
	}

	int add_methods(vector<Heuristic<R, M>*>& _methods)
	{
		for (int i = 0; i < _methods.size(); ++i)
		{
			cout << "\'method " << i << "\' added." << endl;
			method.push_back(_methods.at(i));
		}
		return method.size() - 1;

	}

	Heuristic<R, M>* get_method(int index)
	{
		return method[index];
	}

	int add_loadsol(Solution<R>* _loadsol)
	{
		loadsol.push_back(_loadsol);
		return loadsol.size() - 1;
	}

	Solution<R>* get_loadsol(int index)
	{
		return loadsol[index];
	}

	int add_initsol(InitialSolution<R>* _initsol)
	{
		initsol.push_back(_initsol);
		return initsol.size() - 1;
	}

	int add_loadpop(Population<R>* _loadpop)
	{
		loadpop.push_back(_loadpop);
		return loadpop.size() - 1;
	}

	Population<R>* get_loadpop(int index)
	{
		return loadpop[index];
	}

	int add_initpop(InitialPopulation<R>* _initpop)
	{
		initpop.push_back(_initpop);
		return initpop.size() - 1;
	}

	InitialSolution<R>* get_initsol(int index)
	{
		return initsol[index];
	}

	Population<R>* get_initpop(int index)
	{
		return initpop[index];
	}

	int add_ns(NS<R, M>* _ns)
	{
		ns.push_back(_ns);
		return ns.size() - 1;
	}

	NS<R, M>* get_ns(int index)
	{
		return ns[index];
	}

	int add_ev(Evaluator<R, M>* _ev)
	{
		ev.push_back(_ev);
		return ev.size() - 1;
	}

	Evaluator<R, M>* get_ev(int index)
	{
		return ev[index];
	}

	int add_ilsl_pert(ILSLPerturbation<R, M>* _ilsl_pert)
	{
		ilsl_pert.push_back(_ilsl_pert);
		return ilsl_pert.size() - 1;
	}

	void add_ils_int(Intensification<R,M>* _ils_int)
	{
		ils_int.push_back(_ils_int);
	}

	ILSLPerturbation<R, M>* get_ilsl_pert(int index)
	{
		return ilsl_pert[index];
	}

	int add_ga_mut(Mutation<R, M>* _ga_mut)
	{
		ga_mut.push_back(_ga_mut);
		return ga_mut.size() - 1;
	}

	Mutation<R, M>* get_ga_mut(int index)
	{
		return ga_mut[index];
	}

	int add_ga_sel(Selection<R, M>* _ga_sel)
	{
		ga_sel.push_back(_ga_sel);
		return ga_sel.size() - 1;
	}

	int add_ga_elt(Elitism<R, M>* _ga_elt)
	{
		ga_elt.push_back(_ga_elt);
		return ga_elt.size() - 1;
	}

	Selection<R, M>* get_ga_sel(int index)
	{
		return ga_sel[index];
	}

	Elitism<R, M>* get_ga_elt(int index)
	{
		return ga_elt[index];
	}

	int add_ga_cross(Crossover<R, M>* _ga_cross)
	{
		ga_cross.push_back(_ga_cross);
		return ga_cross.size() - 1;
	}

	Crossover<R, M>* get_ga_cross(int index)
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

	pair<Heuristic<R, M>*, string> createHeuristic(string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if (!scanner.hasNext())
			return make_pair(new Empty<R, M> , "");

		string h = scanner.next();

		if (h == "method")
		{
			int id = scanner.nextInt();
			return make_pair(method[id], scanner.rest());
		}

		if (h == "Empty")
			return make_pair(new Empty<R, M> , scanner.rest());

		if (h == "BI")
		{
			cout << "Heuristic: Best Improvement" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);
			NSSeq<R, M>* ns_seq = (NSSeq<R, M>*) read_ns(&scanner);

			return make_pair(new BestImprovement<R, M> (*evaluator, *ns_seq), scanner.rest());
		}

		if (h == "FI")
		{
			cout << "Heuristic: First Improvement" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);
			NSSeq<R, M>* ns_seq = (NSSeq<R, M>*) read_ns(&scanner);

			return make_pair(new FirstImprovement<R, M> (*evaluator, *ns_seq), scanner.rest());
		}

		if (h == "HC")
		{
			cout << "Heuristic: Hill Climbing" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);

			string rest = scanner.rest();

			pair<Heuristic<R, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, M>* h = method.first;

			scanner = Scanner(method.second);

			return make_pair(new HillClimbing<R, M> (*evaluator, *h), scanner.rest());
		}

		if (h == "RDM")
		{
			cout << "Heuristic: Random Descent Method" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);
			NS<R, M>* ns = (NS<R, M>*) read_ns(&scanner);
			int iter = scanner.nextInt();

			return make_pair(new RandomDescentMethod<R, M> (*evaluator, *ns, iter), scanner.rest());
		}

		if (h == "GRASP")
		{
			cout << "Heuristic: GRASP" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);
			InitialSolution<R>* initsol = read_initsol(&scanner);

			string rest = scanner.rest();

			pair<Heuristic<R, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, M>* h = method.first;

			scanner = Scanner(method.second);

			int iter = scanner.nextInt();

			return make_pair(new GRASP<R, M> (*evaluator, *initsol, *h, iter), scanner.rest());
		}

		if (h == "TS")
		{
			cout << "Heuristic: Tabu Search" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);
			NSSeq<R, M>* ns = (NSSeq<R, M>*) read_ns(&scanner);
			int tamT = scanner.nextInt();
			int BTmax = scanner.nextInt();

			return make_pair(new TabuSearch<R, M> (*evaluator, *ns, tamT, BTmax), scanner.rest());
		}

		/*
		 * Heuristic "Descida" deprecated! use HillClimbing+BI instead.
		 */

		/*

		 if(h == "SA")
		 {
		 cout << "Heuristic: Simulated Annealing" << endl;

		 Evaluator<R,M>* evaluator = read_ev(&scanner);
		 NS<R,M>* ns = read_ns(&scanner);
		 double alpha = scanner.nextDouble();
		 int SAmax = scanner.nextInt();
		 double Ti = scanner.nextDouble();

		 return make_pair(new SimulatedAnnealing<R,M>(evaluator,ns,alpha,SAmax,Ti),scanner.rest());
		 }

		 if(h == "TS")
		 {
		 cout << "Heuristic: Tabu Search" << endl;

		 Evaluator<R,M>* evaluator = read_ev(&scanner);
		 NSEnum<R,M>* ns = (NSEnum<R,M>*)read_ns(&scanner);
		 int tamT = scanner.nextInt();
		 int BTmax = scanner.nextInt();

		 return make_pair(new BuscaTabu<R,M>(evaluator,ns,tamT,BTmax),scanner.rest());
		 }

		 */
		if (h == "ILSL")
		{
			cout << "Heuristic: Iterated Local Search (Levels)" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<Heuristic<R, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, M>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			ILSLPerturbation<R, M>* ilsl_pert = read_ilsl_pert(&scanner);

			int iterMax = scanner.nextInt();
			int levelMax = scanner.nextInt();

			return make_pair(new IteratedLocalSearchLevels<R, M> (*evaluator, *localSearch, *ilsl_pert, iterMax, levelMax), scanner.rest());
		}

		if (h == "IILSL")
		{
			cout << "Heuristic: Intensified Iterated Local Search (Levels)" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<Heuristic<R, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, M>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			/*rest = scanner.rest();

			pair<Heuristic<R, M>*, string> method2;
			method2 = createHeuristic(rest);

			Heuristic<R, M>* localSearch2 = method2.first;

			scanner = Scanner(method2.second);*/

			Intensification<R,M> * intensification = read_ils_int(&scanner);

			// ====================

			ILSLPerturbation<R, M>* ilsl_pert = read_ilsl_pert(&scanner);

			int iterMax = scanner.nextInt();
			int levelMax = scanner.nextInt();

			//return make_pair(new IntensifiedIteratedLocalSearchLevels<R, M> (*evaluator, *localSearch, *localSearch2, *ilsl_pert, iterMax, levelMax),scanner.rest());
			return make_pair(new IntensifiedIteratedLocalSearchLevels<R,M>(*evaluator, *localSearch, *intensification, *ilsl_pert,iterMax,levelMax),scanner.rest());
		}

		if (h == "VND")
		{
			cout << "Heuristic: Variable Neighborhood Descent" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);
			vector<Heuristic<R, M>*> hlist = read_heuristic_list(&scanner);
			add_methods(hlist);

			return make_pair(new VariableNeighborhoodDescent<R, M> (*evaluator, hlist), scanner.rest());

		}

		if (h == "RVND")
		{
			cout << "Heuristic: RVND" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);
			vector<Heuristic<R, M>*> hlist = read_heuristic_list(&scanner);
			add_methods(hlist);

			return make_pair(new RVND<R, M> (*evaluator, hlist), scanner.rest());

		}

		/*
		 if(h == "VNS")
		 {
		 cout << "Heuristic: VNS" << endl;

		 Evaluator<R,M>* evaluator = read_ev(&scanner);
		 vector<NS<R,M>*> ns_list = read_ns_list(&scanner);

		 // ===================
		 // Read next heuristic
		 // ===================

		 string rest = scanner.rest();

		 pair<Heuristic<R,M>*, string> method;
		 method = createHeuristic(rest);

		 Heuristic<R,M>* localSearch = method.first;
		 rest = method.second;

		 return make_pair(new VNS<R,M>(evaluator,ns_list,localSearch),rest);
		 }
		 */

		// Parallel Support
		/*if (h == "BISeqMR")
		{
			cout << "Heuristic: MapReduce Best Improvement (SeqMR)" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);
			NSSeq<R, M>* ns_seq = (NSSeq<R, M>*) read_ns(&scanner);
			int NP = read_np(&scanner);

			return make_pair(new BestImprovement_SeqMR<R, M> (*evaluator, *ns_seq, NP), scanner.rest());
		}*/

/*
#ifdef MaPI
		if (h == "BIMaPI")
		{
			cout << "Heuristic: MapReduce Best Improvement (MaPI)" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);
			NSSeq<R, M>* ns_seq = (NSSeq<R, M>*) read_ns(&scanner);

			return make_pair(new BestImprovement_MaPI<R, M> (*serializer,*mapReduce,*mapper,*reducer,*evaluator, *ns_seq), scanner.rest());
		}
#endif/**/

		if (h == "MH")
		{
			cout << "Heuristic: MultiHeuristic" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);
			vector<Heuristic<R, M>*> hlist = read_heuristic_list(&scanner);
			add_methods(hlist);

			return make_pair(new MultiHeuristic<R, M> (*evaluator, hlist), scanner.rest());
		}


#ifdef MaPI
		if (h == "MapReduce")
		{
			cout << "Heuristic: MapReduce" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<Heuristic<R, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, M>* hmap = method.first;

			scanner = Scanner(method.second);

			// ===================
			// Read next heuristic
			// ===================

			rest = scanner.rest();

			method = createHeuristic(rest);

			Heuristic<R, M>* hreduce = method.first;

			scanner = Scanner(method.second);

			// ===================
			mapper->setHeuristic(hmap);
			reducer->setHeuristic(hreduce); // reduz usando a heurística hreduce

			return make_pair(new OptFrameMapReduce<R, M> (*serializer,*mapReduce,*mapper,*reducer,*evaluator), scanner.rest());
		}

		if (h == "Map")
		{
			cout << "Heuristic: Map" << endl;

			Evaluator<R, M>* evaluator = read_ev(&scanner);

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<Heuristic<R, M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R, M>* hmap = method.first;

			scanner = Scanner(method.second);

			// ===================
			mapper->setHeuristic(hmap);
			// A não especificação da heurística de redução implina na
			// redução à melhor solução produzida nos mapeamentos

			return make_pair(new OptFrameMapReduce<R, M> (*serializer,*mapReduce,*mapper,*reducer,*evaluator), scanner.rest());
		}
#endif

		cout << "Warning: no heuristic '" << h << "' found! ignoring..." << endl;

		return make_pair(new Empty<R, M> , scanner.rest());
	}

};

#endif /* HEURISTICFACTORY_HPP_ */
