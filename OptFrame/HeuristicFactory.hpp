#ifndef HEURISTICFACTORY_HPP_
#define HEURISTICFACTORY_HPP_

#include <iostream>

using namespace std;

#include "./Util/Scanner++/Scanner.h"

#include "Solution.h"

#include "Heuristic.hpp"

//Heuristics
#include "Heuristic/BestImprovement.hpp"
#include "Heuristic/FirstImprovement.hpp"

//Metaheuristics
#include "Heuristic/Empty.hpp"
#include "Heuristic/Descida.hpp"
#include "Heuristic/HillClimbing.hpp"
#include "Heuristic/SimulatedAnnealing.hpp"
#include "Heuristic/BuscaTabu.hpp"
#include "Heuristic/IteratedLocalSearch/IteratedLocalSearch.hpp"
#include "Heuristic/IteratedLocalSearch/IteratedLocalSearchLevels.hpp"
#include "Heuristic/IteratedLocalSearch/PerturbationLPlus2.hpp"
/////#include "Heuristic/NonLinearSimulatedAnnealing.hpp"
#include "Heuristic/MultiStart.hpp"
#include "Heuristic/MRD.hpp"
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

// design pattern: Factory

template<class R, class M>
class HeuristicFactory
{
private:
	vector<InitialSolution<R>*> initsol;
	vector<NS<R,M>*> ns;
	vector<Evaluator<R,M>*> ev;
	vector<PerturbationLevels<R>*> ilsl_pert;
	vector<Solution<R>*> loadsol;
	vector<Heuristic<R,M>*> method;

public:

	Solution<R>* read_loadsol(Scanner* scanner)
	{
		string tmp = scanner->next();

		if(tmp != "loadsol")
			cout << "Warning: expected 'loadsol' and found '"<<tmp<<"'."<<endl;

		int loadsol_id = scanner->nextInt();

		if(loadsol.size()<=loadsol_id)
		{
			cout << "Error: solution number "<<loadsol_id<<" doesn't exist!"<<endl;
			exit(1);
		}

		return loadsol[loadsol_id];
	}


	InitialSolution<R>* read_initsol(Scanner* scanner)
	{
		string tmp = scanner->next();

		if(tmp != "initsol")
			cout << "Warning: expected 'initsol' and found '"<<tmp<<"'."<<endl;

		int initsol_id = scanner->nextInt();

		if(initsol.size()<=initsol_id)
		{
			cout << "Error: init number "<<initsol_id<<" doesn't exist!"<<endl;
			exit(1);
		}

		return initsol[initsol_id];
	}


	NS<R,M>* read_ns(Scanner* scanner)
	{
		string tmp = scanner->next();

		if(tmp != "ns")
			cout << "Warning: expected 'ns' and found '"<<tmp<<"'."<<endl;

		int ns_id = scanner->nextInt();

		if(ns.size()<=ns_id)
		{
			cout << "Error: ns number "<<ns_id<<" doesn't exist!"<<endl;
			exit(1);
		}

		return ns[ns_id];
	}

	vector<NS<R,M>*> read_ns_list(Scanner* scanner)
	{
		string tmp = scanner->next();

		vector<NS<R,M>*> v_ns;

		if(tmp != "[")
		{
			cout << "Error: expected '[' and found '"<<tmp<<"'!" << endl;

			if(tmp=="ns")
			{
				int ns_id = scanner->nextInt();

				if(ns.size()<=ns_id)
				{
					cout << "Error: ns number "<<ns_id<<" doesn't exist!"<<endl;
					exit(1);
				}

				v_ns.push_back(ns[ns_id]);
			}

			return v_ns; // returning empty list
		}

		tmp = scanner->next();

		while(tmp != "]")
		{
			if(tmp != "ns")
				cout << "Warning: expected 'ns' and found '"<<tmp<<"'."<<endl;

			int ns_id = scanner->nextInt();

			if(ns.size()<=ns_id)
			{
				cout << "Error: ns number "<<ns_id<<" doesn't exist!"<<endl;
				exit(1);
			}

			v_ns.push_back(ns[ns_id]);

			tmp = scanner->next();
		}

		if(v_ns.size()==0)
		{
			cout << "Error: empty ns list."<<endl;
			exit(1);
		}

		return v_ns;
	}


	vector< Evaluator<R,M> * > read_ev_list(Scanner* scanner)
	{
		string tmp = scanner->next();

		vector< Evaluator<R,M> * > v_ev;

		if(tmp != "[")
		{
			cout << "Error: expected '[' and found '"<<tmp<<"'."<<endl;
			exit(1);
		}

		tmp = scanner->next();

		while(tmp != "]")
		{
			if(tmp != "ev")
				cout << "Warning: expected 'ev' and found '"<<tmp<<"'."<<endl;

			int ev_id = scanner->nextInt();

			if(ev.size()<=ev_id)
			{
				cout << "Error: ev number "<<ev_id<<" doesn't exist!"<<endl;
				exit(1);
			}

			v_ev.push_back(ev[ev_id]);

			tmp = scanner->next();
		}

		if(v_ev.size()==0)
		{
			cout << "Error: empty ev list."<<endl;
			exit(1);
		}

		return v_ev;
	}


	Evaluator<R,M>* read_ev(Scanner* scanner)
	{
		string tmp = scanner->next();

		if(tmp == "gev")
		{
			vector< Evaluator<R,M> * > evs = read_ev_list(scanner);
			return new GenericEvaluator<R,M>(evs);
		}
		else
			if(tmp != "ev")
				cout << "Warning: expected 'ev' and found '"<<tmp<<"'."<<endl;

		int ev_id = scanner->nextInt();

		if(ev.size()<=ev_id)
		{
			cout << "Error: ev number "<<ev_id<<" doesn't exist!"<<endl;
			exit(1);
		}

		return ev[ev_id];
	}

	PerturbationLevels<R>* read_ilsl_pert(Scanner* scanner)
	{
		string tmp = scanner->next();

		if(tmp != "ilsl_pert")
			cout << "Warning: expected 'ilsl_pert' and found '"<<tmp<<"'."<<endl;

		int ilsl_pert_id = scanner->nextInt();

		if(ilsl_pert.size()<=ilsl_pert_id)
		{
			cout << "Error: 'perturbation levels' number "<<ilsl_pert_id<<" doesn't exist!"<<endl;
			exit(1);
		}

		return ilsl_pert[ilsl_pert_id];
	}

	// ================================================================
	// ================================================================

	HeuristicFactory(){};

	int add_method(Heuristic<R,M>* _method)
	{
		method.push_back(_method);
		return method.size()-1;
	}

	Heuristic<R,M>* get_method(int index)
	{
		return method[index];
	}

	int add_loadsol(Solution<R>* _loadsol)
	{
		loadsol.push_back(_loadsol);
		return loadsol.size()-1;
	}

	Solution<R>* get_loadsol(int index)
	{
		return loadsol[index];
	}


	int add_initsol(InitialSolution<R>* _initsol)
	{
		initsol.push_back(_initsol);
		return initsol.size()-1;
	}

	InitialSolution<R>* get_initsol(int index)
	{
		return initsol[index];
	}

	int add_ns(NS<R,M>* _ns)
	{
		ns.push_back(_ns);
		return ns.size()-1;
	}

	NS<R,M>* get_ns(int index)
	{
		return ns[index];
	}

	int add_ev(Evaluator<R,M>* _ev)
	{
		ev.push_back(_ev);
		return ev.size()-1;
	}

	Evaluator<R,M>* get_ev(int index)
	{
		return ev[index];
	}

	int add_ilsl_pert(PerturbationLevels<R>* _ilsl_pert)
	{
		ilsl_pert.push_back(_ilsl_pert);
		return ilsl_pert.size()-1;
	}

	PerturbationLevels<R>* get_ilsl_pert(int index)
	{
		return ilsl_pert[index];
	}

	pair<Heuristic<R,M>*, string> createHeuristic(string str)
	{
		Scanner scanner(str);

		// No heuristic!
		if(!scanner.hasNext())
			return make_pair(new Empty<R,M>,"");

		string h = scanner.next();

		if(h == "method")
		{
			int id = scanner.nextInt();
			return make_pair(method[id],scanner.rest());
		}

		if(h == "Empty")
			return make_pair(new Empty<R,M>,scanner.rest());

		if(h == "BI")
		{
			cout << "Heuristic: Best Improvement" << endl;

			Evaluator<R,M>* evaluator = read_ev(&scanner);
			NSSeq<R,M>* ns_seq = (NSSeq<R,M>*)read_ns(&scanner);

			return make_pair(new BestImprovement<R,M>(evaluator,ns_seq),scanner.rest());
		}

		if(h == "FI")
		{
			cout << "Heuristic: First Improvement" << endl;

			Evaluator<R,M>* evaluator = read_ev(&scanner);
			NSSeq<R,M>* ns_seq = (NSSeq<R,M>*)read_ns(&scanner);

			return make_pair(new FirstImprovement<R,M>(evaluator,ns_seq),scanner.rest());
		}

		if(h == "Descida") // deprecated! use HillClimbing+BI instead.
		{
			cout << "Heuristic: Descida" << endl;

			Evaluator<R,M>* evaluator = read_ev(&scanner);
			NSSeq<R,M>* ns_seq = (NSSeq<R,M>*)read_ns(&scanner);

			return make_pair(new Descida<R,M>(evaluator,ns_seq),scanner.rest());
		}

		if(h == "HC")
		{
			cout << "Heuristic: Hill Climbing" << endl;

			Evaluator<R,M>* evaluator = read_ev(&scanner);

			string rest = scanner.rest();

			pair<Heuristic<R,M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R,M>* h = method.first;

			scanner = Scanner(method.second);

			return make_pair(new HillClimbing<R,M>(evaluator,h),scanner.rest());
		}

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

		if(h == "MRD")
		{
			cout << "Heuristic: MRD" << endl;

			Evaluator<R,M>* evaluator = read_ev(&scanner);
			NS<R,M>* ns = read_ns(&scanner);
			int iterMax = scanner.nextInt();

			return make_pair(new MRD<R,M>(evaluator,ns,iterMax),scanner.rest());
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

		if(h == "VND")
		{
			cout << "Heuristic: Variable Neighborhood Descent" << endl;

			Evaluator<R,M>* evaluator = read_ev(&scanner);
			vector<NS<R,M>*> ns_list = read_ns_list(&scanner);

			vector<NSSeq<R,M>* > ns_seq_list;

			for(unsigned i=0;i<ns_list.size();i++)
				ns_seq_list.push_back((NSSeq<R,M>*)ns_list[i]);

			return make_pair(new VariableNeighborhoodDescent<R,M>(evaluator,ns_seq_list),scanner.rest());
		}

		if(h == "RVND")
		{
			cout << "Heuristic: RVND" << endl;

			Evaluator<R,M>* evaluator = read_ev(&scanner);
			vector<NS<R,M>*> ns_list = read_ns_list(&scanner);

			vector<NSEnum<R,M>* > ns_enum_list;

			for(unsigned i=0;i<ns_list.size();i++)
				ns_enum_list.push_back((NSEnum<R,M>*)ns_list[i]);

			return make_pair(new RVND<R,M>(evaluator,ns_enum_list),scanner.rest());
		}

		if(h == "ILSL")
		{
			cout << "Heuristic: Iterated Local Search (Levels)" << endl;

			Evaluator<R,M>* evaluator = read_ev(&scanner);

			// ===================
			// Read next heuristic
			// ===================

			string rest = scanner.rest();

			pair<Heuristic<R,M>*, string> method;
			method = createHeuristic(rest);

			Heuristic<R,M>* localSearch = method.first;

			scanner = Scanner(method.second);

			// ====================

			PerturbationLevels<R>* ilsl_pert = read_ilsl_pert(&scanner);

			int iterMax = scanner.nextInt();
			int levelMax = scanner.nextInt();

			return make_pair(new IteratedLocalSearchLevels<R,M>(evaluator, localSearch, ilsl_pert,iterMax,levelMax),scanner.rest());
		}


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

		return make_pair(new Empty<R,M>,scanner.rest());
	}

};

#endif /* HEURISTICFACTORY_HPP_ */
