#ifndef EXECMODULE_HPP_
#define EXECMODULE_HPP_

#include "../OptFrameTestModule.hpp"

template<class R, class M>
class ExecModule : public OptFrameTestModule<R,M>
{
public:
	string id()
	{
		return "exec";
	}
	string usage()
	{
		return "exec [initsol id | loadsol id] target_fo timelimit method [output_solution_name]";
	}

	void run(vector<OptFrameTestModule<R,M>*> all_modules, HeuristicFactory<R,M>* factory, map<string,string>* dictionary, string input)
	{
		cout << "exec: "<<input<<endl;
		Scanner scanner(input);

		if(!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string sol = scanner.next();

		if((sol!="initsol") && (sol!="loadsol"))
		{
			cout << "First parameter must be either 'initsol' or 'loadsol'!" << endl;
			cout << "Usage: "<<usage()<<endl;
			return;
		}

		string id = scanner.next();

		Solution<R>* s = NULL;
		bool needDelete = false;

		if(sol=="loadsol")
		{
			Scanner s2(sol+" "+id);
			s = factory->read_loadsol(&s2);
		}

		if(sol=="initsol")
		{
			Scanner s2(sol+" "+id);
			InitialSolution<R>* initsol = factory->read_initsol(&s2);
			s = initsol->generateSolution();
			needDelete = true;
		}


		double target_fo = scanner.nextDouble();
		long timelimit = scanner.nextLong();

		pair<Heuristic<R,M>*,string> method = factory->createHeuristic(scanner.rest());
		scanner = Scanner(method.second);

		Solution<R>* sFinal = method.first->safeSearch(target_fo,timelimit,s);

		if(needDelete)
			delete s;

		int new_id = factory->add_loadsol(sFinal);

		stringstream str;
		str << "loadsol "<< new_id;
		string s_new_id = str.str();

		cout << "'"<<s_new_id<<"' added." << endl;

		if(scanner.hasNext())
		{
			string new_name = scanner.next();
			run_module("define",all_modules,factory,dictionary,new_name+" "+s_new_id);
		}


	}

};

#endif /* EXECMODULE_HPP_ */
