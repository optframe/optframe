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

#include "ADSManager.hpp"

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
#include "Heuristics/LocalSearches/BestImprovement.hpp"
#include "Heuristics/LocalSearches/FirstImprovement.hpp"
#include "Heuristics/LocalSearches/RandomDescentMethod.hpp"
#include "Heuristics/LocalSearches/CircularSearch.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescentUpdateADS.hpp"
#include "Heuristics/LocalSearches/RVND.hpp"
#include "Heuristics/LocalSearches/HillClimbing.hpp"
#include "Heuristics/LocalSearches/LateAcceptanceHillClimbing.hpp"

// single obj search
#include "Heuristics/SimpleLocalSearch.hpp"
#include "Heuristics/ILS/BasicIteratedLocalSearch.hpp"
#include "Heuristics/ILS/BasicILSPerturbation.hpp"
#include "Heuristics/SA/BasicSimulatedAnnealing.hpp"
#include "Heuristics/ILS/IteratedLocalSearchLevels.hpp"
#include "Heuristics/ILS/ILSLPerturbation.hpp"
#include "Heuristics/GRASP/GRASP.hpp"
#include "Heuristics/VNS/VariableNeighborhoodSearch.hpp"
#include "Heuristics/VNS/BasicVNS.hpp"
#include "Heuristics/VNS/ReducedVNS.hpp"
#include "Heuristics/VNS/GeneralVNS.hpp"

// test local searches
#include "Heuristics/CompareLocalSearch.hpp"

// ==================================

#include "PreprocessFunction.hpp"
#include "Module.hpp"

#include "RandGen.hpp"
#include "Util/RandGenMersenneTwister.hpp"

#include "Functions/ListAppendFunction.hpp"
#include "Functions/ListDefinitionFunction.hpp"
#include "Functions/ListElementFunction.hpp"
#include "Functions/ListLengthFunction.hpp"
#include "Functions/ListWordsFunction.hpp"

#include "Functions/MathRoundFunction.hpp"
#include "Functions/MathTruncFunction.hpp"

#include "Functions/OperatorAbsFunction.hpp"
#include "Functions/OperatorCompareFunction.hpp"
#include "Functions/OperatorComputeFunction.hpp"
#include "Functions/OperatorInFunction.hpp"
#include "Functions/OperatorLogicFunction.hpp"

#include "Functions/StatisticsArgMaxFunction.hpp"
#include "Functions/StatisticsArgMinFunction.hpp"
#include "Functions/StatisticsANOVAFunction.hpp"
#include "Functions/StatisticsAvgFunction.hpp"
#include "Functions/StatisticsFriedmanNoBlockTestFunction.hpp"
#include "Functions/StatisticsFriedmanTestFunction.hpp"
#include "Functions/StatisticsMannUTestFunction.hpp"
#include "Functions/StatisticsMaxFunction.hpp"
#include "Functions/StatisticsMedFunction.hpp"
#include "Functions/StatisticsMinFunction.hpp"
#include "Functions/StatisticsPairedTTestFunction.hpp"
#include "Functions/StatisticsShapiroTestFunction.hpp"
#include "Functions/StatisticsSampleStdDevFunction.hpp"
#include "Functions/StatisticsStdDevFunction.hpp"
#include "Functions/StatisticsStudentTTestFunction.hpp"
#include "Functions/StatisticsSumFunction.hpp"
#include "Functions/StatisticsWilcoxonTestFunction.hpp"

#include "Functions/SystemDefinedFunction.hpp"
#include "Functions/SystemDefinitionFunction.hpp"
#include "Functions/SystemInputFunction.hpp"
#include "Functions/SystemPOpenFunction.hpp"
#include "Functions/SystemTimeFunction.hpp"

#include "Functions/TextConcatFunction.hpp"
#include "Functions/TextDefinitionFunction.hpp"
#include "Functions/TextNextFunction.hpp"

// ------------------------------------------------

// to become functions or runtime modules...

#include "Modules/CreateNumericListModule.hpp"
#include "Modules/EmpiricalModule.hpp"
#include "Modules/TestModule.hpp"
#include "Modules/TestLocalSearchModule.hpp"

// component

#include "Modules/ComponentBetterThanModule.hpp"
#include "Modules/ComponentBuilderOfComponent.hpp"
#include "Modules/ComponentBuildModule.hpp"
#include "Modules/ComponentCheckModule.hpp"
#include "Modules/ComponentCreateListModule.hpp"
#include "Modules/ComponentDropModule.hpp"
#include "Modules/ComponentDropAllModule.hpp"
#include "Modules/ComponentEvaluateModule.hpp"
#include "Modules/ComponentExecConstructiveModule.hpp"
#include "Modules/ComponentExecLocalSearchModule.hpp"
#include "Modules/ComponentExecModule.hpp"
#include "Modules/ComponentExportLogModule.hpp"
#include "Modules/ComponentExportModule.hpp"
#include "Modules/ComponentListBuildersModule.hpp"
#include "Modules/ComponentListFromPopulationModule.hpp"
#include "Modules/ComponentListModule.hpp"
#include "Modules/ComponentLogModule.hpp"
#include "Modules/ComponentNullModule.hpp"
#include "Modules/ComponentToStringModule.hpp"

#include "Modules/FileEchoModule.hpp"
#include "Modules/FileToListModule.hpp"

#include "Modules/ListAddModule.hpp"
#include "Modules/ListPushBackModule.hpp"
#include "Modules/ListRemoveModule.hpp"
#include "Modules/ListSilentDefineModule.hpp"
#include "Modules/ListSortModule.hpp"

#include "Modules/ModuleCreateModule.hpp"
#include "Modules/ModuleCreateRawModule.hpp"
#include "Modules/ModuleExistsModule.hpp"

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

#include "Modules/OperatorAssignModule.hpp"

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

using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class OptFrame
{
private:
	vector<Module<R, ADS, DS>*> modules;
	vector<PreprocessFunction<R, ADS, DS>*> functions;
	map<string, string> dictionary;
	map<string, vector<string> > ldictionary;

public:

	HeuristicFactory<R, ADS, DS> factory;

	OptFrame()
	{
		loadDefault();
		loadComponentBuilders();
	}

	OptFrame(RandGen _rg) :
		factory(HeuristicFactory<R, ADS, DS> (_rg))
	{
		loadDefault();
		loadComponentBuilders();
	}

	virtual ~OptFrame()
	{
		unloadModules();
		unloadFunctions();
	}

	string version()
	{
		return "opti - OptFrame Script Language Interpreter \nDevelopment Version 2.0 \nhttp://sourceforge.net/projects/optframe/";
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

	void loadModule(Module<R, ADS, DS>* module)
	{
		if (module)
			modules.push_back(module);
	}

	void loadFunction(PreprocessFunction<R,ADS,DS>* function)
	{
		if (function)
			functions.push_back(function);
	}

	void loadCallModule()
	{
		cout << "warning: system.call module loaded!" << endl;

		loadModule(new SystemCallModule<R, ADS, DS> );
	}

	void loadDefault(string prefix = "")
	{
		// to become functions or runtime modules...
		if ((prefix == "") || (prefix == "deprecated")) // TODO: remove!
		{
			loadModule(new CreateNumericListModule<R, ADS, DS> );
			loadModule(new EmpiricalModule<R, ADS, DS> );
			loadModule(new TestModule<R, ADS, DS> );
			loadModule(new TestLocalSearchModule<R, ADS, DS> );
		}

		// components
		if ((prefix == "") || (prefix == "component"))
		{
			loadModule(new ComponentBetterThanModule<R, ADS, DS> );
			loadModule(new ComponentBuilderOfComponentModule<R, ADS, DS> );
			loadModule(new ComponentBuildModule<R, ADS, DS> );
			loadModule(new ComponentCheckModule<R, ADS, DS> );
			loadModule(new ComponentCreateListModule<R, ADS, DS> );
			loadModule(new ComponentDropModule<R, ADS, DS> );
			loadModule(new ComponentDropAllModule<R, ADS, DS> );
			loadModule(new ComponentEvaluateModule<R, ADS, DS> );
			loadModule(new ComponentExecConstructiveModule<R, ADS, DS> );
			loadModule(new ComponentExecLocalSearchModule<R, ADS, DS> );
			loadModule(new ComponentExecModule<R, ADS, DS> );
			loadModule(new ComponentExportLogModule<R, ADS, DS> );
			loadModule(new ComponentExportModule<R, ADS, DS> );
			loadModule(new ComponentListBuildersModule<R, ADS, DS> );
			loadModule(new ComponentListFromPopulationModule<R, ADS, DS> );
			loadModule(new ComponentListModule<R, ADS, DS> );
			loadModule(new ComponentLogModule<R, ADS, DS> );
			loadModule(new ComponentNullModule<R, ADS, DS> );
			loadModule(new ComponentToStringModule<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "file"))
		{
			loadModule(new FileEchoModule<R, ADS, DS> );
			loadModule(new FileToListModule<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "list"))
		{
			loadModule(new ListAddModule<R, ADS, DS> );
			loadModule(new ListPushBackModule<R, ADS, DS> );
			loadModule(new ListRemoveModule<R, ADS, DS> );
			loadModule(new ListSilentDefineModule<R, ADS, DS> );
			loadModule(new ListSortModule<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "module"))
		{
			loadModule(new ModuleCreateModule<R, ADS, DS> );
			loadModule(new ModuleCreateRawModule<R, ADS, DS> );
			loadModule(new ModuleExistsModule<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "function"))
		{
			loadModule(new FunctionCreateRawModule<R, ADS, DS> );
		}

		// deprecated
#ifdef MaPI
		loadModule(new InitServersModule<R, ADS, DS> );
#endif

		if ((prefix == "") || (prefix == "plot"))
		{
			loadModule(new Plot2AxisModule<R, ADS, DS> );
			loadModule(new PlotViewModule<R, ADS, DS> );
		}

		// cannot load abstract module
		//loadModule(new ProblemModule<R, ADS, DS> );

		if ((prefix == "") || (prefix == "randgen"))
		{
			loadModule(new RandGenIntervalModule<R, ADS, DS> );
			loadModule(new RandGenNumberModule<R, ADS, DS> );
			loadModule(new RandGenSetSeedModule<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "system"))
		{
			loadModule(new SystemDefineModule<R, ADS, DS> );
			loadModule(new SystemDictionaryModule<R, ADS, DS> );
			loadModule(new SystemEchoModule<R, ADS, DS> );
			loadModule(new SystemErrorModule<R, ADS, DS> );
			loadModule(new SystemHelpModule<R, ADS, DS> );
			loadModule(new SystemPauseModule<R, ADS, DS> );
			loadModule(new SystemPreprocessModule<R, ADS, DS> );
			loadModule(new SystemReadModule<R, ADS, DS> );
			loadModule(new SystemRequireModule<R, ADS, DS> );
			loadModule(new SystemRunModule<R, ADS, DS> );
			loadModule(new SystemSilentDefineModule<R, ADS, DS> );
			loadModule(new SystemUndefineModule<R, ADS, DS> );
			loadModule(new SystemUnsafeDefineModule<R, ADS, DS> );
			loadModule(new SystemUsageModule<R, ADS, DS> );
			loadModule(new SystemUseModule<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "operator"))
		{
			loadModule(new OperatorAssignModule<R, ADS, DS> );
		}

		// ==========================================================
		// language statements (conditional, loop and error handling)
		// ==========================================================
		if ((prefix == "") || (prefix == "statements"))
		{
			loadModule(new ForModule<R, ADS, DS> );
			loadModule(new ForEachModule<R, ADS, DS> );
			loadModule(new IfElseModule<R, ADS, DS> );
			loadModule(new WhileModule<R, ADS, DS> );
			loadModule(new TryModule<R, ADS, DS> );
		}

		// ----------------------------------------------

		if ((prefix == "") || (prefix == "list"))
		{
			loadFunction(new ListAppendFunction<R,ADS,DS>);
			//loadFunction(new ListDefinitionFunction<R,ADS,DS>);
			loadFunction(new ListElementFunction<R,ADS,DS>);
			loadFunction(new ListLengthFunction<R,ADS,DS>);
			loadFunction(new ListWordsFunction<R,ADS,DS>);
		}

		if ((prefix == "") || (prefix == "math"))
		{
			loadFunction(new MathRoundFunction<R,ADS,DS>);
			loadFunction(new MathTruncFunction<R,ADS,DS>);
		}

		if ((prefix == "") || (prefix == "operator"))
		{
			loadFunction(new OperatorAbsFunction<R,ADS,DS>);
			loadFunction(new OperatorCompareFunction<R,ADS,DS>);
			loadFunction(new OperatorComputeFunction<R,ADS,DS>);
			loadFunction(new OperatorInFunction<R,ADS,DS>);
			loadFunction(new OperatorLogicFunction<R,ADS,DS>);
		}

		// statistics
		if ((prefix == "") || (prefix == "statistics"))
		{
			loadFunction(new StatisticsArgMaxFunction<R,ADS,DS>);
			loadFunction(new StatisticsArgMinFunction<R,ADS,DS>);
			loadFunction(new StatisticsANOVAFunction<R,ADS,DS>);
			loadFunction(new StatisticsAvgFunction<R,ADS,DS>);
			loadFunction(new StatisticsFriedmanNoBlockTestFunction<R,ADS,DS>);
			loadFunction(new StatisticsFriedmanTestFunction<R,ADS,DS>);
			loadFunction(new StatisticsMannUTestFunction<R,ADS,DS>);
			loadFunction(new StatisticsMaxFunction<R,ADS,DS>);
			loadFunction(new StatisticsMedFunction<R,ADS,DS>);
			loadFunction(new StatisticsMinFunction<R,ADS,DS>);
			loadFunction(new StatisticsPairedTTestFunction<R,ADS,DS>);
			loadFunction(new StatisticsShapiroTestFunction<R,ADS,DS>);
			loadFunction(new StatisticsSampleStdDevFunction<R,ADS,DS>);
			loadFunction(new StatisticsStdDevFunction<R,ADS,DS>);
			loadFunction(new StatisticsStudentTTestFunction<R,ADS,DS>);
			loadFunction(new StatisticsSumFunction<R,ADS,DS>);
			loadFunction(new StatisticsWilcoxonTestFunction<R,ADS,DS>);
		}

		if ((prefix == "") || (prefix == "system"))
		{
			loadFunction(new SystemDefinedFunction<R,ADS,DS>);
			loadFunction(new SystemDefinitionFunction<R,ADS,DS>);
			loadFunction(new SystemInputFunction<R,ADS,DS>);
			loadFunction(new SystemPOpenFunction<R,ADS,DS>);
			loadFunction(new SystemTimeFunction<R,ADS,DS>);
		}

		if ((prefix == "") || (prefix == "text"))
		{
			loadFunction(new TextConcatFunction<R,ADS,DS>);
			//loadFunction(new TextDefinitionFunction<R,ADS,DS>);
			loadFunction(new TextNextFunction<R,ADS,DS>);
		}
	}

	Module<R, ADS, DS>* getModule(string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

	void loadComponentBuilders()
	{
		// Base
		factory.builders.push_back(new CloneConstructiveBuilder<R, ADS, DS> );

		// LocalSearch
		factory.builders.push_back(new EmptyLocalSearchBuilder<R, ADS, DS> );
		factory.builders.push_back(new BestImprovementBuilder<R, ADS, DS> );
		factory.builders.push_back(new FirstImprovementBuilder<R, ADS, DS> );
		factory.builders.push_back(new RandomDescentMethodBuilder<R, ADS, DS> );
		factory.builders.push_back(new CircularSearchBuilder<R, ADS, DS> );
		factory.builders.push_back(new VariableNeighborhoodDescentBuilder<R, ADS, DS> );
		factory.builders.push_back(new VariableNeighborhoodDescentUpdateADSBuilder<R, ADS, DS> );
		factory.builders.push_back(new RVNDBuilder<R, ADS, DS> );
		factory.builders.push_back(new HillClimbingBuilder<R, ADS, DS> );
		factory.builders.push_back(new LateAcceptanceHillClimbingBuilder<R, ADS, DS> );

		// SingleObjSearch + Parameters
		factory.builders.push_back(new SimpleLocalSearchBuilder<R, ADS, DS> );
		factory.builders.push_back(new BasicSimulatedAnnealingBuilder<R, ADS, DS> );
		factory.builders.push_back(new BasicIteratedLocalSearchBuilder<R, ADS, DS> );
		factory.builders.push_back(new BasicILSPerturbationBuilder<R, ADS, DS> );
		factory.builders.push_back(new IteratedLocalSearchLevelsBuilder<R, ADS, DS> );
		factory.builders.push_back(new ILSLPerturbationLPlus2Builder<R, ADS, DS> );
		factory.builders.push_back(new GRASPBuilder<R, ADS, DS> );
		factory.builders.push_back(new BasicVNSBuilder<R, ADS, DS> );
		factory.builders.push_back(new ReducedVNSBuilder<R, ADS, DS> );
		factory.builders.push_back(new GeneralVNSBuilder<R, ADS, DS> );

		// test local searches
		factory.builders.push_back(new CompareLocalSearchBuilder<R, ADS, DS> );
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
			s2.useSeparators(" \t\r\n=");

			string command = s2.next();

			if (command == "quit" || command == "q" || command == "exit")
				break;

			if (command == "version" || command == "v")
			{
				cout << version() << endl;
				continue;
			}

			bool notfound = true;
			string command_body = s2.rest();

			for (unsigned int i = 0; i < modules.size(); i++)
				if (modules[i]->canHandle(command, command_body))
				{
					string* r1 = modules[i]->preprocess(functions, factory, dictionary, ldictionary, command_body);

					if (!r1)
						break;

					string r = *r1;
					delete r1;

					if (r == "INVALID_PARAM")
					{
						cout << "Population size has to be, at least, equal 2.\n";
						notfound = false;
						break;
					}

					if (!modules[i]->run(modules, functions, factory, dictionary, ldictionary, r, command))
						cout << "command failed!" << endl;

					notfound = false;
					break;
				}

			if (notfound)
			{
				cout << "Sorry, i couldn't understand the command '" << command << "'." << endl << "Please, type 'system.help' or type the command again." << endl;
				printOptions(command, modules);
			}
		}

		cout << "Goodbye." << endl;
	}

	static void printOptions(string part_module, vector<Module<R, ADS, DS>*>& allModules)
	{
		for (unsigned int i = 0; i < allModules.size(); i++)
		{
			bool found = true;

			for (unsigned j = 0; j < part_module.length(); j++)
				if (part_module[j] != allModules[i]->id()[j])
				{
					found = false;
					break;
				}

			if (found)
				cout << "option is: '" << allModules[i]->id() << "'" << endl;
		}
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
		s2.useSeparators(" \t\r\n=");

		string command = s2.next();

		if (command == "quit" || command == "q" || command == "exit")
			return;

		if (command == "version" || command == "v")
		{
			cout << version() << endl;
			return;
		}

		bool notfound = true;
		string command_body = s2.rest();

		for (unsigned int i = 0; i < modules.size(); i++)
			if (modules[i]->canHandle(command, command_body))
			{
				string* r = modules[i]->preprocess(functions, factory, dictionary, ldictionary, command_body);

				if (!r)
					break;

				if (*r == "INVALID_PARAM")
				{
					cout << "Population size has to be, at least, equal 2.\n";
					exit(1);
				}

				if (!modules[i]->run(modules, functions, factory, dictionary, ldictionary, *r, command))
					cout << "error in module: '" << modules[i]->id() << "'" << endl;

				delete r;

				notfound = false;
				break;
			}

		if (notfound)
			cout << "Sorry, i couldn't understand the command '" << command << "'." << endl << "Please, type 'system.help' or type the command again." << endl;

		//cout << "Goodbye." << endl;
	}

};

}

#endif /*OPTFRAME_H_*/
