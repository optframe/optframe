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

#ifndef OPTFRAME_INTERACTIVE_HPP_
#define OPTFRAME_INTERACTIVE_HPP_

/*! \mainpage OptFrame
 * This is the OptFrame documentation.
 */
//Esta é a documentação do OptFrame.
#include <iostream>
#include <ostream>

#include <vector>
#include <map>

#include <unistd.h>
#include <stdlib.h>

#include "Util/printable.h"
#include "Util/Matrix.hpp"
#include "Scanner++/Scanner.h"


#include "Solution.hpp"
//#include "Util/TestSolution.hpp"

#include "ADSManager.hpp"

#include "Population.hpp"

#include "InitialPopulation.h"

#include "Move.hpp"
//#include "Util/TestMove.hpp"

#include "Evaluation.hpp"
//#include "Util/TestEvaluation.hpp"
#include "Evaluator.hpp"
#include "MultiObjectiveEvaluator.hpp"

#include "Util/TestEvaluation.hpp"

//#include "RandGen.hpp"
#include "Timer.hpp"


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
#include "Heuristics/VNS/RandVNS.hpp"

// test local searches
#include "Heuristics/CompareLocalSearch.hpp"

// ==================================

#include "PreprocessFunction.hpp"
#include "Command.hpp"

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
#include "Functions/OperatorSCompareFunction.hpp"

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

#include "Commands/CreateNumericListCommand.hpp"
#include "Commands/EmpiricalCommand.hpp"
#include "Commands/TestCommand.hpp"
#include "Commands/TestLocalSearchCommand.hpp"

// component

#include "Commands/ComponentActionCommand.hpp"
#include "Commands/ComponentBuilderOfComponent.hpp"
#include "Commands/ComponentBuildCommand.hpp"
#include "Commands/ComponentCastCommand.hpp"
#include "Commands/ComponentCheckCommand.hpp"
#include "Commands/ComponentClearCommand.hpp"
#include "Commands/ComponentCreateListCommand.hpp"
#include "Commands/ComponentDropCommand.hpp"
#include "Commands/ComponentListBuildersCommand.hpp"
#include "Commands/ComponentListFromPopulationCommand.hpp"
#include "Commands/ComponentListCommand.hpp"
#include "Commands/ComponentNullCommand.hpp"
#include "Commands/ComponentToStringCommand.hpp"

#include "Commands/FileEchoCommand.hpp"
#include "Commands/FilePrintCommand.hpp"
#include "Commands/FilePrintlnCommand.hpp"
#include "Commands/FileToListCommand.hpp"

#include "Commands/ListAddCommand.hpp"
#include "Commands/ListPushBackCommand.hpp"
#include "Commands/ListRemoveCommand.hpp"
#include "Commands/ListSilentDefineCommand.hpp"
#include "Commands/ListSortCommand.hpp"

#include "Commands/CommandCreateCommand.hpp"
#include "Commands/CommandCreateLocalCommand.hpp"
#include "Commands/CommandCreateRawCommand.hpp"
#include "Commands/CommandExistsCommand.hpp"
#include "Commands/CommandRenameCommand.hpp"
#include "Commands/CommandUnitTestCommand.hpp"

#include "Commands/FunctionCreateRawCommand.hpp"

#include "Commands/ParallelInitServersCommand.hpp"

#include "Commands/Plot2AxisCommand.hpp"
#include "Commands/PlotViewCommand.hpp"

#include "Commands/ProblemCommand.hpp"

#include "Commands/RandGenIntervalCommand.hpp"
#include "Commands/RandGenNumberCommand.hpp"
#include "Commands/RandGenSetSeedCommand.hpp"

#include "Commands/SystemAssertCommand.hpp"
#include "Commands/SystemCallCommand.hpp"
#include "Commands/SystemChDirCommand.hpp"
#include "Commands/SystemDefineCommand.hpp"
#include "Commands/SystemDictionaryCommand.hpp"
#include "Commands/SystemEchoCommand.hpp"
#include "Commands/SystemErrorCommand.hpp"
#include "Commands/SystemHelpCommand.hpp"
#include "Commands/SystemPauseCommand.hpp"
#include "Commands/SystemPreprocessCommand.hpp"
#include "Commands/SystemPrintCommand.hpp"
#include "Commands/SystemPrintlnCommand.hpp"
#include "Commands/SystemReadCommand.hpp"
#include "Commands/SystemRequireCommand.hpp"
#include "Commands/SystemRunCommand.hpp"
#include "Commands/SystemRunParallelCommand.hpp"
#include "Commands/SystemSilentDefineCommand.hpp"
#include "Commands/SystemUndefineCommand.hpp"
#include "Commands/SystemUnsafeDefineCommand.hpp"
#include "Commands/SystemUsageCommand.hpp"
#include "Commands/SystemUseCommand.hpp"

#include "Commands/OperatorAssignCommand.hpp"

//structural
#include "Commands/IfElseCommand.hpp"
#include "Commands/ForCommand.hpp"
#include "Commands/ForEachCommand.hpp"
#include "Commands/WhileCommand.hpp"
#include "Commands/TryCommand.hpp"
#include "Constructive.hpp"

// ==================================
//            Serializer
// ==================================

//#include "Serializer/Serializer.hpp"

// ==================================

using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Interactive
{
private:
	vector<Command<R, ADS, DS>*> modules;
	vector<PreprocessFunction<R, ADS, DS>*> functions;
	map<string, string> dictionary;
	map<string, vector<string> > ldictionary;

public:

	HeuristicFactory<R, ADS, DS> factory;

	Interactive()
	{
		setVariables();
		loadDefault();
		loadComponentBuilders();
		loadActions();
	}

	Interactive(RandGen _rg) :
		factory(HeuristicFactory<R, ADS, DS> (_rg))
	{
		setVariables();
		loadDefault();
		loadComponentBuilders();
		loadActions();
	}

	void setAppPath(string appPath)
	{
		if (appPath != "")
		{
			const char *symlinkpath = appPath.c_str();
			char actualpath[PATH_MAX + 1];
			char *ptr;

			ptr = realpath(symlinkpath, actualpath);
			string fullpath(ptr);

			dictionary["optframeapp"] = fullpath;

			size_t found;
			found=fullpath.find_last_of("/\\");
			string dir = fullpath.substr(0,found);

			dictionary["optframeappdir"] = dir;
		}
	}

	virtual ~Interactive()
	{
		unloadCommands();
		unloadFunctions();
	}

	string welcome()
	{
		string w = "opti - OptFrame Script Language - Interactive Move\nVersion: ";
		w.append(version());
		w.append("\nLGPLv3\nhttp://sourceforge.net/projects/optframe/\n");
		return w;
	}

	string version()
	{
		return "2.0";
	}

	void setVariables()
	{
	    char *path=nullptr;
	    size_t size =0;
	    path=getcwd(path,size);

		dictionary["optframepath"] = path;
		dictionary["optframeversion"] = version();

        free(path);
	}

	void unloadCommands()
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

	void unloadBuilders()
	{
		for (unsigned int i = 0; i < factory.builders.size(); ++i)
			delete factory.builders.at(i);
		factory.builders.clear();
	}

	void unloadActions()
	{
		for (unsigned int i = 0; i < factory.actions.size(); ++i)
			delete factory.actions.at(i);
		factory.actions.clear();
	}

	void loadCommand(Command<R, ADS, DS>* module)
	{
		if (module)
			modules.push_back(module);
	}

	void loadFunction(PreprocessFunction<R,ADS,DS>* function)
	{
		if (function)
			functions.push_back(function);
	}

	void loadCallCommand()
	{
		cout << "warning: system.call module loaded!" << endl;

		loadCommand(new SystemCallCommand<R, ADS, DS> );
	}

	void loadDefault(string prefix = "")
	{
		// to become functions or runtime modules...
		if ((prefix == "") || (prefix == "deprecated")) // TODO: remove!
		{
			loadCommand(new CreateNumericListCommand<R, ADS, DS> );
			loadCommand(new EmpiricalCommand<R, ADS, DS> );
			loadCommand(new TestCommand<R, ADS, DS> );
			loadCommand(new TestLocalSearchCommand<R, ADS, DS> );
		}

		// components
		if ((prefix == "") || (prefix == "component"))
		{
			loadCommand(new ComponentActionCommand<R, ADS, DS> );
			loadCommand(new ComponentBuilderOfComponentCommand<R, ADS, DS> );
			loadCommand(new ComponentBuildCommand<R, ADS, DS> );
			loadCommand(new ComponentCastCommand<R, ADS, DS> );
			loadCommand(new ComponentCheckCommand<R, ADS, DS> );
			loadCommand(new ComponentClearCommand<R, ADS, DS> );
			loadCommand(new ComponentCreateListCommand<R, ADS, DS> );
			loadCommand(new ComponentDropCommand<R, ADS, DS> );
			loadCommand(new ComponentListBuildersCommand<R, ADS, DS> );
			loadCommand(new ComponentListFromPopulationCommand<R, ADS, DS> );
			loadCommand(new ComponentListCommand<R, ADS, DS> );
			loadCommand(new ComponentNullCommand<R, ADS, DS> );
			loadCommand(new ComponentToStringCommand<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "file"))
		{
			loadCommand(new FileEchoCommand<R, ADS, DS> );
			loadCommand(new FilePrintCommand<R, ADS, DS> );
			loadCommand(new FilePrintlnCommand<R, ADS, DS> );
			loadCommand(new FileToListCommand<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "list"))
		{
			loadCommand(new ListAddCommand<R, ADS, DS> );
			loadCommand(new ListPushBackCommand<R, ADS, DS> );
			loadCommand(new ListRemoveCommand<R, ADS, DS> );
			loadCommand(new ListSilentDefineCommand<R, ADS, DS> );
			loadCommand(new ListSortCommand<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "command"))
		{
			loadCommand(new CommandCreateCommand<R, ADS, DS> );
			loadCommand(new CommandCreateLocalCommand<R, ADS, DS> );
			loadCommand(new CommandCreateRawCommand<R, ADS, DS> );
			loadCommand(new CommandExistsCommand<R, ADS, DS> );
			loadCommand(new CommandRenameCommand<R, ADS, DS> );
			loadCommand(new CommandUnitTestCommand<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "function"))
		{
			loadCommand(new FunctionCreateRawCommand<R, ADS, DS> );
		}

		// deprecated
#ifdef MaPI
		loadCommand(new InitServersCommand<R, ADS, DS> );
#endif

		if ((prefix == "") || (prefix == "plot"))
		{
			loadCommand(new Plot2AxisCommand<R, ADS, DS> );
			loadCommand(new PlotViewCommand<R, ADS, DS> );
		}

		// cannot load abstract module
		//loadCommand(new ProblemCommand<R, ADS, DS> );

		if ((prefix == "") || (prefix == "randgen"))
		{
			loadCommand(new RandGenIntervalCommand<R, ADS, DS> );
			loadCommand(new RandGenNumberCommand<R, ADS, DS> );
			loadCommand(new RandGenSetSeedCommand<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "system"))
		{
			loadCommand(new SystemAssertCommand<R, ADS, DS> );
			loadCommand(new SystemChDirCommand<R, ADS, DS> );
			loadCommand(new SystemDefineCommand<R, ADS, DS> );
			loadCommand(new SystemDictionaryCommand<R, ADS, DS> );
			loadCommand(new SystemEchoCommand<R, ADS, DS> );
			loadCommand(new SystemErrorCommand<R, ADS, DS> );
			loadCommand(new SystemHelpCommand<R, ADS, DS> );
			loadCommand(new SystemPauseCommand<R, ADS, DS> );
			loadCommand(new SystemPreprocessCommand<R, ADS, DS> );
			loadCommand(new SystemPrintCommand<R, ADS, DS> );
			loadCommand(new SystemPrintlnCommand<R, ADS, DS> );
			loadCommand(new SystemReadCommand<R, ADS, DS> );
			loadCommand(new SystemRequireCommand<R, ADS, DS> );
			loadCommand(new SystemRunCommand<R, ADS, DS> );
			loadCommand(new SystemSilentDefineCommand<R, ADS, DS> );
			loadCommand(new SystemUndefineCommand<R, ADS, DS> );
			loadCommand(new SystemUnsafeDefineCommand<R, ADS, DS> );
			loadCommand(new SystemUsageCommand<R, ADS, DS> );
			loadCommand(new SystemUseCommand<R, ADS, DS> );
		}

		if ((prefix == "") || (prefix == "operator"))
		{
			loadCommand(new OperatorAssignCommand<R, ADS, DS> );
		}

		// ==========================================================
		// language statements (conditional, loop and error handling)
		// ==========================================================
		if ((prefix == "") || (prefix == "statements"))
		{
			loadCommand(new ForCommand<R, ADS, DS> );
			loadCommand(new ForEachCommand<R, ADS, DS> );
			loadCommand(new IfElseCommand<R, ADS, DS> );
			loadCommand(new WhileCommand<R, ADS, DS> );
			loadCommand(new TryCommand<R, ADS, DS> );
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
			loadFunction(new OperatorSCompareFunction<R,ADS,DS>);
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

	Command<R, ADS, DS>* getCommand(string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return nullptr;
	}

	void loadActions()
	{
		factory.actions.push_back(new ComponentAction<R, ADS, DS>);
		factory.actions.push_back(new RandGenAction<R, ADS, DS>);
		factory.actions.push_back(new TimerAction<R, ADS, DS>);
		factory.actions.push_back(new ConstructiveAction<R, ADS, DS>);
		factory.actions.push_back(new EvaluatorAction<R, ADS, DS>);
		factory.actions.push_back(new EvaluationAction<R, ADS, DS>);
		factory.actions.push_back(new MoveAction<R, ADS, DS>);
		factory.actions.push_back(new NSAction<R, ADS, DS>);
		factory.actions.push_back(new NSSeqAction<R, ADS, DS>);
		factory.actions.push_back(new NSIteratorAction<R, ADS, DS>);
		factory.actions.push_back(new LocalSearchAction<R, ADS, DS>);
		factory.actions.push_back(new SingleObjSearchAction<R, ADS, DS>);
	}

	void loadComponentBuilders()
	{
		// Independent components
		factory.builders.push_back(new RandGenBuilder<R, ADS, DS> );
		factory.builders.push_back(new TimerBuilder<R, ADS, DS> );

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
		factory.builders.push_back(new ILSLPerturbationLPlus2ProbBuilder<R, ADS, DS> );
		factory.builders.push_back(new GRASPBuilder<R, ADS, DS> );
		factory.builders.push_back(new BasicVNSBuilder<R, ADS, DS> );
		factory.builders.push_back(new ReducedVNSBuilder<R, ADS, DS> );
		factory.builders.push_back(new GeneralVNSBuilder<R, ADS, DS> );
		factory.builders.push_back(new RandVNSBuilder<R, ADS, DS> );

		// test local searches
		factory.builders.push_back(new CompareLocalSearchBuilder<R, ADS, DS> );
	}

	//! \english OptFrame Command Line Interface \endenglish \portuguese Interface de Linha de Comando do OptFrame \endportuguese
	/*!
	 \sa execute(string)
	 */

	void execute()
	{
		cout << "Welcome to " << welcome() << endl;

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

	static void printOptions(string part_command, vector<Command<R, ADS, DS>*>& allCommands)
	{
		for (unsigned int i = 0; i < allCommands.size(); i++)
		{
			bool found = true;

			for (unsigned j = 0; j < part_command.length(); j++)
				if (part_command[j] != allCommands[i]->id()[j])
				{
					found = false;
					break;
				}

			if (found)
				cout << "option is: '" << allCommands[i]->id() << "'" << endl;
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
					cout << "error in command: '" << modules[i]->id() << "'" << endl;

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

#endif /*OPTFRAME_INTERACTIVE_HPP_*/
