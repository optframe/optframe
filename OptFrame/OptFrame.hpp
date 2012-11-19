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

#ifndef OPTFRAME_H_
#define OPTFRAME_H_

/*! \mainpage OptFrame
 * This is the OptFrame documentation.
 */
//Esta é a documentação do OptFrame.
#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include "Util/printable.h"
#include "Util/Matrix.hpp"
#include "Scanner++/Scanner.h"
#include "Util/Timer.hpp"

#include "Solution.hpp"
//#include "Util/TestSolution.hpp"

#include "Population.hpp"

#include "Constructive.h"
#include "InitialPopulation.h"

#include "Move.hpp"
//#include "Util/TestMove.hpp"

#include "Evaluation.hpp"
//#include "Util/TestEvaluation.hpp"
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
//        Parallel Support
// ==================================

#include "Parallel/Parallel.h"

// ==================================
//            Heuristic
// ==================================

#include "SingleObjSearch.hpp"
#include "MultiObjSearch.hpp"

#include "HeuristicFactory.hpp"

// base components
#include "CloneConstructive.hpp"


// local search
#include "Heuristics/Empty.hpp"
#include "Heuristics/BestImprovement.hpp"
#include "Heuristics/FirstImprovement.hpp"
#include "Heuristics/RandomDescentMethod.hpp"
#include "Heuristics/CircularSearch.hpp"
#include "Heuristics/VariableNeighborhoodDescent.hpp"
#include "Heuristics/RVND.hpp"
#include "Heuristics/HillClimbing.hpp"


// single obj search
#include "Heuristics/SimpleLocalSearch.hpp"
#include "Heuristics/BasicIteratedLocalSearch.hpp"
#include "Heuristics/BasicILSPerturbation.hpp"


// ==================================

#include "OptFrameFunction.hpp"
#include "OptFrameModule.hpp"

#include "RandGen.hpp"
#include "Util/RandGenMersenneTwister.hpp"

#include "Functions/LengthFunction.hpp"

#include "Modules/BuildModule.hpp"
#include "Modules/CallModule.hpp"
#include "Modules/CheckModule.hpp"
#include "Modules/CreateListOfComponentsModule.hpp"
#include "Modules/CreateModule.hpp"
#include "Modules/CreateNumericListModule.hpp"
#include "Modules/DefineModule.hpp"
#include "Modules/DictionaryModule.hpp"
#include "Modules/DropAllModule.hpp"
#include "Modules/EchoModule.hpp"
#include "Modules/ElementModule.hpp"
#include "Modules/EmpiricalModule.hpp"
#include "Modules/EvaluateModule.hpp"
#include "Modules/EvaluateToFileModule.hpp"
#include "Modules/ExecModule.hpp"
#include "Modules/ExecConstructiveModule.hpp"
#include "Modules/ExportModule.hpp"
#include "Modules/ExportLogModule.hpp"
#include "Modules/ForEachModule.hpp"
#include "Modules/HelpModule.hpp"
#include "Modules/ListAppendModule.hpp"
#include "Modules/ListBuilderOfComponentModule.hpp"
#include "Modules/ListBuildersModule.hpp"
#include "Modules/ListComponentsModule.hpp"
#include "Modules/ListFromFileModule.hpp"
#include "Modules/ListFromPopulationModule.hpp"
#include "Modules/ListSizeModule.hpp"
#include "Modules/PauseModule.hpp"
#include "Modules/PrintModule.hpp"
#include "Modules/ProblemModule.hpp"
#include "Modules/RandGenModule.hpp"
#include "Modules/RandomNumberModule.hpp"
#include "Modules/RandomNumberIntervalModule.hpp"
#include "Modules/ReadModule.hpp"
#include "Modules/SilentDefineModule.hpp"
#include "Modules/TestModule.hpp"
#include "Modules/TestLocalSearchModule.hpp"
#include "Modules/UndefineModule.hpp"
#include "Modules/UsageModule.hpp"
#include "Modules/InitServersModule.hpp"

// ==================================
//            Serializer
// ==================================

//#include "Serializer/Serializer.hpp"


// ==================================

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class OptFrame
{
private:
	vector<OptFrameModule<R, ADS, M>*> modules;
	vector<OptFrameFunction*> functions;
	map<string, string>* dictionary;

public:

	HeuristicFactory<R, ADS, M> factory;

	OptFrame()
	{
		loadDefaultModules();
		loadComponentBuilders();
		dictionary = new map<string, string> ;
	}

	OptFrame(RandGen _rg) :
		factory(HeuristicFactory<R, ADS, M> (_rg))
	{
		loadDefaultModules();
		loadComponentBuilders();
		dictionary = new map<string, string> ;
	}

	virtual ~OptFrame()
	{
		for (unsigned int i = 0; i < modules.size(); ++i)
		{
			delete modules.at(i);
		}
		delete dictionary;
	}

	string version()
	{
		return "OptFrame - Development Version \nhttp://sourceforge.net/projects/optframe/";
	}

	void loadModule(OptFrameModule<R, ADS, M>* module)
	{
		modules.push_back(module);
	}

	void loadFunction(OptFrameFunction* function)
	{
		functions.push_back(function);
	}

	void loadCallModule()
	{
		cout << "warning: call module loaded!" << endl;

		modules.push_back(new CallModule<R, ADS, M> );
	}

	void loadDefaultModules()
	{
		modules.clear();
		loadModule(new BuildModule<R, ADS, M> );
		loadModule(new CheckModule<R, ADS, M> );
		loadModule(new CreateListOfComponentsModule<R, ADS, M> );
		loadModule(new CreateModule<R, ADS, M> );
		loadModule(new CreateNumericListModule<R, ADS, M> );
		loadModule(new DefineModule<R, ADS, M> );
		loadModule(new DictionaryModule<R, ADS, M> );
		loadModule(new DropAllModule<R, ADS, M> );
		loadModule(new EchoModule<R, ADS, M> );
		loadModule(new ElementModule<R, ADS, M> );
		loadModule(new EmpiricalModule<R, ADS, M> );
		loadModule(new EvaluateModule<R, ADS, M> );
		loadModule(new EvaluateToFileModule<R, ADS, M> );
		loadModule(new ExecModule<R, ADS, M> );
		loadModule(new ExecConstructiveModule<R, ADS, M> );
		loadModule(new ExportModule<R, ADS, M> );
		loadModule(new ExportLogModule<R, ADS, M> );
		loadModule(new ForEachModule<R, ADS, M> );
		loadModule(new HelpModule<R, ADS, M> );
		loadModule(new ListAppendModule<R, ADS, M> );
		loadModule(new ListBuilderOfComponentModule<R, ADS, M> );
		loadModule(new ListBuildersModule<R, ADS, M> );
		loadModule(new ListComponentsModule<R, ADS, M> );
		loadModule(new ListFromFileModule<R, ADS, M> );
		loadModule(new ListFromPopulationModule<R, ADS, M> );
		loadModule(new ListSizeModule<R, ADS, M> );
		loadModule(new PauseModule<R, ADS, M> );
		loadModule(new PrintModule<R, ADS, M> );
		loadModule(new RandGenModule<R, ADS, M> );
		loadModule(new RandomNumberModule<R, ADS, M> );
		loadModule(new RandomNumberIntervalModule<R, ADS, M> );
		loadModule(new ReadModule<R, ADS, M> );
		loadModule(new SilentDefineModule<R, ADS, M> );
		loadModule(new TestModule<R, ADS, M> );
		loadModule(new TestLocalSearchModule<R, ADS, M> );
		loadModule(new UndefineModule<R, ADS, M> );
		loadModule(new UsageModule<R, ADS, M> );
#ifdef MaPI
		loadModule(new InitServersModule<R, ADS, M> );
#endif

		functions.clear();
		loadFunction(new LengthFunction);
	}

	OptFrameModule<R, ADS, M>* getModule(string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

	void loadComponentBuilders()
	{
		// Base
		factory.builders.push_back(new CloneConstructiveBuilder<R, ADS, M>);

		// LocalSearch
		factory.builders.push_back(new EmptyLocalSearchBuilder<R, ADS, M>);
		factory.builders.push_back(new BestImprovementBuilder<R, ADS, M>);
		factory.builders.push_back(new FirstImprovementBuilder<R, ADS, M>);
		factory.builders.push_back(new RandomDescentMethodBuilder<R, ADS, M>);
		factory.builders.push_back(new CircularSearchBuilder<R, ADS, M>);
		factory.builders.push_back(new VariableNeighborhoodDescentBuilder<R, ADS, M>);
		factory.builders.push_back(new RVNDBuilder<R, ADS, M>);
		factory.builders.push_back(new HillClimbingBuilder<R, ADS, M>);

		// SingleObjSearch + Parameters
		factory.builders.push_back(new SimpleLocalSearchBuilder<R, ADS, M>);
		factory.builders.push_back(new BasicIteratedLocalSearchBuilder<R, ADS, M>);
		factory.builders.push_back(new BasicILSPerturbationBuilder<R, ADS, M>);

	}

	//! \english OptFrame Command Line Interface \endenglish \portuguese Interface de Linha de Comando do OptFrame \endportuguese
	/*!
	 \sa execute(string)
	 */

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

			for (unsigned int i = 0; i < modules.size(); i++)
				if (command == modules[i]->id())
				{
					string r = modules[i]->preprocess(dictionary, s2.rest());

					if (r == "INVALID_PARAM")
					{
						cout << "Population size has to be, at least, equal 2.\n";
						notfound = false;
						break;
					}

					modules[i]->run(modules, functions, &factory, dictionary, r);
					notfound = false;
					break;
				}

			if (notfound)
				cout << "Sorry, i couldn't understand the command '" << command << "'." << endl << "Please, type 'help' or type the command again." << endl;
		}

		cout << "Goodbye." << endl;
	}

	//! \english Execute command in OptFrame Command Line Interface \endenglish \portuguese Executa comando na Interface de Linha de Comando do OptFrame \endportuguese
	/*!
	 \english
	 \param line command to be executed
	 \sa execute()
	 \endenglish
	 \portuguese
	 \param line comando a ser executado
	 \sa execute()
	 \endportuguese
	 */

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

		for (unsigned int i = 0; i < modules.size(); i++)
			if (command == modules[i]->id())
			{
				string r = modules[i]->preprocess(functions, dictionary, s2.rest());

				if (r == "INVALID_PARAM")
				{
					cout << "Population size has to be, at least, equal 2.\n";
					exit(1);
				}

				modules[i]->run(modules, functions, &factory, dictionary, r);
				notfound = false;
				break;
			}

		if (notfound)
			cout << "Sorry, i couldn't understand the command '" << command << "'." << endl << "Please, type 'help' or type the command again." << endl;

		//cout << "Goodbye." << endl;
	}

};

#endif /*OPTFRAME_H_*/
