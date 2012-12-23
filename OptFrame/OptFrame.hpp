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

#include "Functions/AbsFunction.hpp"
#include "Functions/AvgFunction.hpp"
#include "Functions/CompareFunction.hpp"
#include "Functions/ElementFunction.hpp"
#include "Functions/InputFunction.hpp"
#include "Functions/LengthFunction.hpp"
#include "Functions/MaxFunction.hpp"
#include "Functions/MinFunction.hpp"
#include "Functions/NextFunction.hpp"
#include "Functions/POpenFunction.hpp"
#include "Functions/TimeFunction.hpp"
#include "Functions/WordsFunction.hpp"

// lists
#include "Functions/AppendFunction.hpp"
#include "Functions/TextFunction.hpp"

// statistics
#include "Functions/ANOVAFunction.hpp"
#include "Functions/FriedmanTestFunction.hpp"
#include "Functions/MannUTestFunction.hpp"
#include "Functions/PairedTTestFunction.hpp"
#include "Functions/ShapiroTestFunction.hpp"
#include "Functions/StudentTTestFunction.hpp"
#include "Functions/WilcoxonTestFunction.hpp"

// logic
#include "Functions/LogicFunction.hpp"

// arithmetic
#include "Functions/MathFunction.hpp"

// string
#include "Functions/ConcatFunction.hpp"
#include "Functions/DicFunction.hpp"

// ------------------------------------------------

// to become functions or runtime modules...

#include "Modules/CreateNumericListModule.hpp"
#include "Modules/EmpiricalModule.hpp"
#include "Modules/TestModule.hpp"
#include "Modules/TestLocalSearchModule.hpp"

// component

#include "Modules/ComponentBuilderOfComponent.hpp"
#include "Modules/ComponentBuildModule.hpp"
#include "Modules/ComponentCheckModule.hpp"
#include "Modules/ComponentCreateListModule.hpp"
#include "Modules/ComponentDropAllModule.hpp"
#include "Modules/ComponentEvaluateModule.hpp"
#include "Modules/ComponentExecConstructiveModule.hpp"
#include "Modules/ComponentExecModule.hpp"
#include "Modules/ComponentExportLogModule.hpp"
#include "Modules/ComponentExportModule.hpp"
#include "Modules/ComponentListBuildersModule.hpp"
#include "Modules/ComponentListFromPopulationModule.hpp"
#include "Modules/ComponentListModule.hpp"
#include "Modules/ComponentToStringModule.hpp"

#include "Modules/FileEchoModule.hpp"
#include "Modules/FileToListModule.hpp"

#include "Modules/ListAddModule.hpp"
#include "Modules/ListRemoveModule.hpp"
#include "Modules/ListSilentDefineModule.hpp"
#include "Modules/ListSortModule.hpp"

#include "Modules/ModuleCreateModule.hpp"
#include "Modules/ModuleCreateRawModule.hpp"

#include "Modules/FunctionCreateRawModule.hpp"

#include "Modules/ParallelInitServersModule.hpp"

#include "Modules/Plot2AxisModule.hpp"
#include "Modules/PlotViewModule.hpp"

#include "Modules/ProblemModule.hpp"

#include "Modules/RandGenIntervalModule.hpp"
#include "Modules/RandGenNumberModule.hpp"
#include "Modules/RandGenSetSeedModule.hpp"

#include "Modules/SystemCallModule.hpp"
#include "Modules/SystemDefineModule.hpp"
#include "Modules/SystemDictionaryModule.hpp"
#include "Modules/SystemEchoModule.hpp"
#include "Modules/SystemErrorModule.hpp"
#include "Modules/SystemHelpModule.hpp"
#include "Modules/SystemPauseModule.hpp"
#include "Modules/SystemPreprocessModule.hpp"
#include "Modules/SystemReadModule.hpp"
#include "Modules/SystemRequireModule.hpp"
#include "Modules/SystemRunModule.hpp"
#include "Modules/SystemRunParallelModule.hpp"
#include "Modules/SystemSilentDefineModule.hpp"
#include "Modules/SystemUndefineModule.hpp"
#include "Modules/SystemUnsafeDefineModule.hpp"
#include "Modules/SystemUsageModule.hpp"
#include "Modules/SystemUseModule.hpp"

//structural
#include "Modules/IfElseModule.hpp"
#include "Modules/ForModule.hpp"
#include "Modules/ForEachModule.hpp"
#include "Modules/WhileModule.hpp"
#include "Modules/TryModule.hpp"

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
	map<string, string> dictionary;
	map< string, vector<string> > ldictionary;

public:

	HeuristicFactory<R, ADS, M> factory;

	OptFrame()
	{
		loadDefaultModules();
		loadComponentBuilders();
	}

	OptFrame(RandGen _rg) :
		factory(HeuristicFactory<R, ADS, M> (_rg))
	{
		loadDefaultModules();
		loadComponentBuilders();
	}

	virtual ~OptFrame()
	{
		unloadModules();
		unloadFunctions();
	}

	string version()
	{
		return "OptFrame - Development Version 2.0 \nhttp://sourceforge.net/projects/optframe/";
	}

	void unloadModules()
	{
		for (unsigned int i = 0; i < modules.size(); ++i)
			delete modules.at(i);
		modules.clear();
	}

	void unloadFunctions()
	{
		for (unsigned int i = 0; i < functions.size(); ++i)
			delete functions.at(i);
		functions.clear();
	}

	void loadModule(OptFrameModule<R, ADS, M>* module)
	{
		if(module)
			modules.push_back(module);
	}

	void loadFunction(OptFrameFunction* function)
	{
		if(function)
			functions.push_back(function);
	}

	void loadCallModule()
	{
		cout << "warning: system.call module loaded!" << endl;

		loadModule(new SystemCallModule<R, ADS, M> );
	}

	void loadDefaultModules()
	{
		unloadModules();

		// to become functions or runtime modules...
		loadModule(new CreateNumericListModule<R, ADS, M> );
		loadModule(new EmpiricalModule<R, ADS, M> );
		loadModule(new TestModule<R, ADS, M> );
		loadModule(new TestLocalSearchModule<R, ADS, M> );

		// components
		loadModule(new ComponentBuilderOfComponentModule<R, ADS, M> );
		loadModule(new ComponentBuildModule<R, ADS, M> );
		loadModule(new ComponentCheckModule<R, ADS, M> );
		loadModule(new ComponentCreateListModule<R, ADS, M> );
		loadModule(new ComponentDropAllModule<R, ADS, M> );
		loadModule(new ComponentEvaluateModule<R, ADS, M> );
		loadModule(new ComponentExecConstructiveModule<R, ADS, M> );
		loadModule(new ComponentExecModule<R, ADS, M> );
		loadModule(new ComponentExportLogModule<R, ADS, M> );
		loadModule(new ComponentExportModule<R, ADS, M> );
		loadModule(new ComponentListBuildersModule<R, ADS, M> );
		loadModule(new ComponentListFromPopulationModule<R, ADS, M> );
		loadModule(new ComponentListModule<R, ADS, M> );
		loadModule(new ComponentToStringModule<R, ADS, M> );

		loadModule(new FileEchoModule<R, ADS, M> );
		loadModule(new FileToListModule<R, ADS, M> );

		loadModule(new ListAddModule<R, ADS, M> );
		loadModule(new ListRemoveModule<R, ADS, M> );
		loadModule(new ListSilentDefineModule<R, ADS, M> );
		loadModule(new ListSortModule<R, ADS, M> );

		loadModule(new ModuleCreateModule<R, ADS, M> );
		loadModule(new ModuleCreateRawModule<R, ADS, M> );

		loadModule(new FunctionCreateRawModule<R, ADS, M> );

		// deprecated
		#ifdef MaPI
				loadModule(new InitServersModule<R, ADS, M> );
		#endif

		loadModule(new Plot2AxisModule<R, ADS, M> );
		loadModule(new PlotViewModule<R, ADS, M> );

		// cannot load abstract module
		//loadModule(new ProblemModule<R, ADS, M> );

		loadModule(new RandGenIntervalModule<R, ADS, M> );
		loadModule(new RandGenNumberModule<R, ADS, M> );
		loadModule(new RandGenSetSeedModule<R, ADS, M> );

		loadModule(new SystemDefineModule<R, ADS, M> );
		loadModule(new SystemDictionaryModule<R, ADS, M> );
		loadModule(new SystemEchoModule<R, ADS, M> );
		loadModule(new SystemErrorModule<R, ADS, M> );
		loadModule(new SystemHelpModule<R, ADS, M> );
		loadModule(new SystemPauseModule<R, ADS, M> );
		loadModule(new SystemPreprocessModule<R, ADS, M> );
		loadModule(new SystemReadModule<R, ADS, M> );
		loadModule(new SystemRequireModule<R, ADS, M> );
		loadModule(new SystemRunModule<R, ADS, M> );
		loadModule(new SystemSilentDefineModule<R, ADS, M> );
		loadModule(new SystemUndefineModule<R, ADS, M> );
		loadModule(new SystemUnsafeDefineModule<R, ADS, M> );
		loadModule(new SystemUsageModule<R, ADS, M> );
		loadModule(new SystemUseModule<R, ADS, M> );

		//structural
		loadModule(new ForModule<R, ADS, M> );
		loadModule(new ForEachModule<R, ADS, M> );
		loadModule(new IfElseModule<R, ADS, M> );
		loadModule(new WhileModule<R, ADS, M> );
		loadModule(new TryModule<R, ADS, M> );

		// ----------------------------------------------

		unloadFunctions();
		loadFunction(new AbsFunction);
		loadFunction(new AvgFunction);
		loadFunction(new CompareFunction);
		loadFunction(new ElementFunction);
		loadFunction(new InputFunction);
		loadFunction(new LengthFunction);
		loadFunction(new MaxFunction);
		loadFunction(new MinFunction);
		loadFunction(new NextFunction);
		loadFunction(new POpenFunction);
		loadFunction(new ShapiroTestFunction);
		loadFunction(new TimeFunction);
		loadFunction(new WordsFunction);

		// lists
		loadFunction(new AppendFunction);
		loadFunction(new TextFunction);

		// statistics
		loadFunction(new ANOVAFunction);
		loadFunction(new FriedmanTestFunction);
		loadFunction(new MannUTestFunction);
		loadFunction(new PairedTTestFunction);
		loadFunction(new ShapiroTestFunction);
		loadFunction(new StudentTTestFunction);
		loadFunction(new WilcoxonTestFunction);

		// logic
		loadFunction(new LogicFunction);

		//arithmetic
		loadFunction(new MathFunction);

		// string
		loadFunction(new ConcatFunction);
		loadFunction(new DicFunction);
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
					string* r1 = modules[i]->preprocess(functions, dictionary, ldictionary, s2.rest());

					if(!r1)
						break;

					string r = *r1;
					delete r1;

					if (r == "INVALID_PARAM")
					{
						cout << "Population size has to be, at least, equal 2.\n";
						notfound = false;
						break;
					}

					if(!modules[i]->run(modules, functions, factory, dictionary, ldictionary, r))
						cout << "command failed!" << endl;

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
				string* r = modules[i]->preprocess(functions, dictionary, ldictionary, s2.rest());

				if(!r)
					break;

				if (*r == "INVALID_PARAM")
				{
					cout << "Population size has to be, at least, equal 2.\n";
					exit(1);
				}

				if(!modules[i]->run(modules, functions, factory, dictionary, ldictionary, *r))
					cout << "error in module: '" << modules[i]->id() << "'" << endl;

				delete r;

				notfound = false;
				break;
			}

		if (notfound)
			cout << "Sorry, i couldn't understand the command '" << command << "'." << endl << "Please, type 'help' or type the command again." << endl;

		//cout << "Goodbye." << endl;
	}

};

#endif /*OPTFRAME_H_*/
