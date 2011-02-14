#ifndef OPTFRAME_H_
#define OPTFRAME_H_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "Util/printable.h"
#include "Util/Matrix.hpp"
#include "Util/Scanner++/Scanner.h"
#include "Util/Timer.hpp"

#include "Solution.hpp"
#include "Util/TestSolution.hpp"

#include "Population.hpp"

#include "InitialSolution.h"
#include "InitialPopulation.h"

#include "Move.hpp"

#include "Evaluation.hpp"
#include "Evaluator.hpp"
#include "MultiObjectiveEvaluator.hpp"

#include "Util/TestEvaluation.hpp"

// ==================================
//       Neighborhood Structure
// ==================================

#include "NS.hpp"
#include "NSSeq.hpp"
#include "NSEnum.hpp"

// ==================================
//           Util NS
// ==================================

#include "Util/NSSeqUnionAdapter.hpp"

#include "Util/NSSeqkRouteAdapter.hpp"
#include "Util/NSSeqMultiRouteAdapter.hpp"

#include "Util/NSSeqVectorOrOptk.hpp"
#include "Util/NSSeqVectorOrOpt.hpp"

/*
 #include "Util/NSEnumVectorShift.hpp"
 #include "Util/NSSeqVVSwapIntra.hpp"

 #include "Util/NSEnumVVShiftk.hpp"
 #include "Util/NSEnumVVSwapk.hpp"
 */

// ==================================
//        Parallel Support
// ==================================

#include "Parallel/Parallel.h"

// ==================================
//            Heuristic
// ==================================

#include "Heuristic.hpp"

#include "HeuristicFactory.hpp"

// ==================================

#include "OptFrameModule.hpp"

#include "RandGen.hpp"
#include "Util/RandGenMersenneTwister.hpp"

#include "Modules/BuildModule.hpp"
#include "Modules/CheckModule.hpp"
#include "Modules/DefineModule.hpp"
#include "Modules/DictionaryModule.hpp"
#include "Modules/EmpiricalModule.hpp"
#include "Modules/EvaluateModule.hpp"
#include "Modules/ExecModule.hpp"
#include "Modules/ExportModule.hpp"
#include "Modules/HelpModule.hpp"
#include "Modules/PrintModule.hpp"
#include "Modules/ReadModule.hpp"
#include "Modules/TestModule.hpp"
#include "Modules/UsageModule.hpp"

/*
 * Mario - ToDo
 *
 * Implement the following modules:
 *
 * 1 - at : extracts a population's solution.
 * 2 - best : returns the best solution in population.
 *
 */

// ==================================
//            Serializer
// ==================================

//#include "Serializer/Serializer.hpp"


// ==================================

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class OptFrame
{
private:
	vector<OptFrameModule<R, M>*> modules;
	map<string, string>* dictionary;

public:

	HeuristicFactory<R, M>& factory;

	OptFrame() :
		factory(*new HeuristicFactory<R, M>(*new RandGenMersenneTwister(time(NULL))) )
	{
		loadDefaultModules();
		dictionary = new map<string, string> ;
	}

	OptFrame(RandGen& _rg) :
			factory(*new HeuristicFactory<R, M>(_rg) )
		{
			loadDefaultModules();
			dictionary = new map<string, string> ;
		}

	OptFrame(HeuristicFactory<R, M>& f) :
		factory(f)
	{
		loadDefaultModules();
		dictionary = new map<string, string> ;
	}


	string version()
	{
		return "OptFrame - Development Version \nhttp://sourceforge.net/projects/optframe/";
	}

	void loadModule(OptFrameModule<R, M>* module)
	{
		modules.push_back(module);
	}

	void loadDefaultModules()
	{
		modules.clear();
		loadModule(new BuildModule<R, M> );
		loadModule(new CheckModule<R, M> );
		loadModule(new DefineModule<R, M> );
		loadModule(new DictionaryModule<R, M> );
		loadModule(new EmpiricalModule<R, M> );
		loadModule(new EvaluateModule<R, M> );
		loadModule(new ExecModule<R, M> );
		loadModule(new ExportModule<R, M> );
		loadModule(new HelpModule<R, M> );
		loadModule(new PrintModule<R, M> );
		loadModule(new ReadModule<R, M> );
		loadModule(new TestModule<R, M> );
		loadModule(new UsageModule<R, M> );
	}

	OptFrameModule<R, M>* getModule(string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

	void execute()
	{
		cout << "Welcome to " << version() << endl;

		Scanner scanner(&cin);
		string line;

		while (true)
		{
			cout << ">";
			line = scanner.nextLine();

			Scanner s2(line);

			string command = s2.next();

			if (command == "quit" || command == "q" || command == "exit")
				break;

			if (command == "version" || command == "v")
			{
				cout << version() << endl;
				continue;
			}

			bool notfound = true;

			for (int i = 0; i < modules.size(); i++)
				if (command == modules[i]->id())
				{
					string r = modules[i]->preprocess(dictionary, s2.rest());

					if (r == "INVALID_PARAM")
					{
						cout << "Population size has to be, at least, equal 2.\n";
						notfound = false;
						break;
					}

					modules[i]->run(modules, &factory, dictionary, r);
					notfound = false;
					break;
				}

			if (notfound)
				cout << "Sorry, i couldn't understand the command '" << command << "'." << endl << "Please, type 'help' or type the command again." << endl;
		}

		cout << "Goodbye." << endl;
	}

	void execute(string line)
	{
		//cout << "Welcome to " << version() << endl;

		Scanner s2(line);

		string command = s2.next();

		if (command == "quit" || command == "q" || command == "exit")
			return;

		if (command == "version" || command == "v")
		{
			cout << version() << endl;
			return;
		}

		bool notfound = true;

		for (int i = 0; i < modules.size(); i++)
			if (command == modules[i]->id())
			{
				string r = modules[i]->preprocess(dictionary, s2.rest());

				if (r == "INVALID_PARAM")
				{
					cout << "Population size has to be, at least, equal 2.\n";
					exit(1);
				}

				modules[i]->run(modules, &factory, dictionary, r);
				notfound = false;
				break;
			}

		if (notfound)
			cout << "Sorry, i couldn't understand the command '" << command << "'." << endl << "Please, type 'help' or type the command again." << endl;

		//cout << "Goodbye." << endl;
	}

};

#endif /*OPTFRAME_H_*/
