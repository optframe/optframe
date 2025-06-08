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
// Esta é a documentação do OptFrame.
#include <stdlib.h>

#include <iostream>
#include <map>
#include <ostream>
#include <vector>
// #include <unistd.h>

#include <OptFrame/printable/printable.hpp>

#include "Scanner++/Scanner.h"
#include "Solution.hpp"
#include "Util/Matrix.hpp"
// #include "Util/TestSolution.hpp"

#include "ADSManager.hpp"
#include "InitialPopulation.hpp"
#include "Move.hpp"
#include "Population.hpp"
// #include "Util/TestMove.hpp"

#include "Evaluation.hpp"
// #include "Util/TestEvaluation.hpp"
#include "Evaluator.hpp"
#include "MultiObjectiveEvaluator.hpp"

// #include "Util/TestEvaluation.hpp"

// #include "RandGen.hpp"
#include "Timer.hpp"

// ==================================
//       Neighborhood Structure
// ==================================

#include "NS.hpp"
#include "NSEnum.hpp"
#include "NSSeq.hpp"

// ==================================
//        Parallel Support
// ==================================

#include "Parallel/Parallel.h"

// ==================================
//            Heuristic
// ==================================

#include "HeuristicFactory.hpp"
#include "MultiObjSearch.hpp"
#include "SingleObjSearch.hpp"

// base components
#include "CloneConstructive.hpp"

// local search
#include "Heuristics/Empty.hpp"
#include "Heuristics/LocalSearches/BI.hpp"
#include "Heuristics/LocalSearches/CircularSearch.hpp"
#include "Heuristics/LocalSearches/FI.hpp"
#include "Heuristics/LocalSearches/HillClimbing.hpp"
#include "Heuristics/LocalSearches/LateAcceptanceHillClimbing.hpp"
#include "Heuristics/LocalSearches/RandomDescentMethod.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescentUpdateADS.hpp"

// single obj search
#include "Heuristics/GRASP/GRASP.hpp"
#include "Heuristics/ILS/BasicILSPerturbation.hpp"
#include "Heuristics/ILS/BasicIteratedLocalSearch.hpp"
#include "Heuristics/ILS/ILSLPerturbation.hpp"
#include "Heuristics/ILS/IteratedLocalSearchLevels.hpp"
#include "Heuristics/SA/BasicSimulatedAnnealing.hpp"
#include "Heuristics/SimpleLocalSearch.hpp"
#include "Heuristics/VNS/BasicVNS.hpp"
#include "Heuristics/VNS/GeneralVNS.hpp"
#include "Heuristics/VNS/ReducedVNS.hpp"
#include "Heuristics/VNS/VariableNeighborhoodSearch.hpp"

// test local searches
#include "Heuristics/CompareLocalSearch.hpp"

// ==================================

#include "Command.hpp"
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
#include "Functions/StatisticsANOVAFunction.hpp"
#include "Functions/StatisticsArgMaxFunction.hpp"
#include "Functions/StatisticsArgMinFunction.hpp"
#include "Functions/StatisticsAvgFunction.hpp"
#include "Functions/StatisticsFriedmanNoBlockTestFunction.hpp"
#include "Functions/StatisticsFriedmanTestFunction.hpp"
#include "Functions/StatisticsMannUTestFunction.hpp"
#include "Functions/StatisticsMaxFunction.hpp"
#include "Functions/StatisticsMedFunction.hpp"
#include "Functions/StatisticsMinFunction.hpp"
#include "Functions/StatisticsPairedTTestFunction.hpp"
#include "Functions/StatisticsSampleStdDevFunction.hpp"
#include "Functions/StatisticsShapiroTestFunction.hpp"
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
#include "PreprocessFunction.hpp"
#include "RandGen.hpp"
#include "Util/RandGenMersenneTwister.hpp"

// ------------------------------------------------

// to become functions or runtime modules...

#include "Commands/CreateNumericListCommand.hpp"
#include "Commands/EmpiricalCommand.hpp"
// #include "Commands/TestCommand.hpp"
// #include "Commands/TestLocalSearchCommand.hpp"

// component

#include "Commands/CommandCreateCommand.hpp"
#include "Commands/CommandCreateLocalCommand.hpp"
#include "Commands/CommandCreateRawCommand.hpp"
#include "Commands/CommandExistsCommand.hpp"
#include "Commands/CommandRenameCommand.hpp"
#include "Commands/CommandUnitTestCommand.hpp"
#include "Commands/ComponentActionCommand.hpp"
#include "Commands/ComponentBuildCommand.hpp"
#include "Commands/ComponentBuilderOfComponent.hpp"
#include "Commands/ComponentCastCommand.hpp"
#include "Commands/ComponentCheckCommand.hpp"
#include "Commands/ComponentClearCommand.hpp"
#include "Commands/ComponentCreateListCommand.hpp"
#include "Commands/ComponentDropCommand.hpp"
#include "Commands/ComponentListBuildersCommand.hpp"
#include "Commands/ComponentListCommand.hpp"
#include "Commands/ComponentListFromPopulationCommand.hpp"
#include "Commands/ComponentNullCommand.hpp"
#include "Commands/ComponentToStringCommand.hpp"
#include "Commands/FileEchoCommand.hpp"
#include "Commands/FilePrintCommand.hpp"
#include "Commands/FilePrintlnCommand.hpp"
#include "Commands/FileToListCommand.hpp"
#include "Commands/FunctionCreateRawCommand.hpp"
#include "Commands/ListAddCommand.hpp"
#include "Commands/ListPushBackCommand.hpp"
#include "Commands/ListRemoveCommand.hpp"
#include "Commands/ListSilentDefineCommand.hpp"
#include "Commands/ListSortCommand.hpp"
#include "Commands/OperatorAssignCommand.hpp"
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

// structural
#include "Commands/ForCommand.hpp"
#include "Commands/ForEachCommand.hpp"
#include "Commands/IfElseCommand.hpp"
#include "Commands/TryCommand.hpp"
#include "Commands/WhileCommand.hpp"
#include "Constructive.hpp"

// ==================================
//            Serializer
// ==================================

// #include "Serializer/Serializer.hpp"

// ==================================

using namespace std;

namespace optframe {

template <XESolution XES, XEvaluation XEv = Evaluation<>>
class Interactive {
 private:
  vector<Command<R, ADS>*> modules;
  vector<PreprocessFunction<R, ADS>*> functions;
  std::map<std::string, std::string> dictionary;
  map<string, vector<string>> ldictionary;

 public:
  HeuristicFactory<R, ADS> factory;

  Interactive() {
    setVariables();
    loadDefault();
    loadComponentBuilders();
    loadActions();
  }

  Interactive(RandGen _rg) : factory(HeuristicFactory<R, ADS>(_rg)) {
    setVariables();
    loadDefault();
    loadComponentBuilders();
    loadActions();
  }

  void setAppPath(string appPath) {
    if (appPath != "") {
      const char* symlinkpath = appPath.c_str();
      char actualpath[PATH_MAX + 1];
      char* ptr;

      ptr = realpath(symlinkpath, actualpath);
      string fullpath(ptr);

      dictionary["optframeapp"] = fullpath;

      size_t found;
      found = fullpath.find_last_of("/\\");
      string dir = fullpath.substr(0, found);

      dictionary["optframeappdir"] = dir;
    }
  }

  virtual ~Interactive() {
    unloadCommands();
    unloadFunctions();
  }

  string welcome() {
    string w = "opti - OptFrame Script Language - Interactive Move\nVersion: ";
    w.append(version());
    w.append("\nLGPLv3\nhttp://sourceforge.net/projects/optframe/\n");
    return w;
  }

  string version() { return "2.0"; }

  void setVariables() {
    char* path = nullptr;
    size_t size = 0;
    path = getcwd(path, size);

    dictionary["optframepath"] = path;
    dictionary["optframeversion"] = version();

    free(path);
  }

  void unloadCommands() {
    for (unsigned int i = 0; i < modules.size(); ++i) delete modules.at(i);
    modules.clear();
  }

  void unloadFunctions() {
    for (unsigned int i = 0; i < functions.size(); ++i) delete functions.at(i);
    functions.clear();
  }

  void unloadBuilders() {
    for (unsigned int i = 0; i < factory.builders.size(); ++i)
      delete factory.builders.at(i);
    factory.builders.clear();
  }

  void unloadActions() {
    for (unsigned int i = 0; i < factory.actions.size(); ++i)
      delete factory.actions.at(i);
    factory.actions.clear();
  }

  void loadCommand(Command<R, ADS>* module) {
    if (module) modules.push_back(module);
  }

  void loadFunction(PreprocessFunction<R, ADS, DS>* function) {
    if (function) functions.push_back(function);
  }

  void loadCallCommand() {
    std::cout << "warning: system.call module loaded!" << std::endl;

    loadCommand(new SystemCallCommand<R, ADS>);
  }

  void loadDefault(string prefix = "") {
    // to become functions or runtime modules...
    if ((prefix == "") || (prefix == "deprecated"))  // TODO: remove!
    {
      loadCommand(new CreateNumericListCommand<R, ADS>);
      loadCommand(new EmpiricalCommand<R, ADS>);
      loadCommand(new TestCommand<R, ADS>);
      loadCommand(new TestLocalSearchCommand<R, ADS>);
    }

    // components
    if ((prefix == "") || (prefix == "component")) {
      loadCommand(new ComponentActionCommand<R, ADS>);
      loadCommand(new ComponentBuilderOfComponentCommand<R, ADS>);
      loadCommand(new ComponentBuildCommand<R, ADS>);
      loadCommand(new ComponentCastCommand<R, ADS>);
      loadCommand(new ComponentCheckCommand<R, ADS>);
      loadCommand(new ComponentClearCommand<R, ADS>);
      loadCommand(new ComponentCreateListCommand<R, ADS>);
      loadCommand(new ComponentDropCommand<R, ADS>);
      loadCommand(new ComponentListBuildersCommand<R, ADS>);
      loadCommand(new ComponentListFromPopulationCommand<R, ADS>);
      loadCommand(new ComponentListCommand<R, ADS>);
      loadCommand(new ComponentNullCommand<R, ADS>);
      loadCommand(new ComponentToStringCommand<R, ADS>);
    }

    if ((prefix == "") || (prefix == "file")) {
      loadCommand(new FileEchoCommand<R, ADS>);
      loadCommand(new FilePrintCommand<R, ADS>);
      loadCommand(new FilePrintlnCommand<R, ADS>);
      loadCommand(new FileToListCommand<R, ADS>);
    }

    if ((prefix == "") || (prefix == "list")) {
      loadCommand(new ListAddCommand<R, ADS>);
      loadCommand(new ListPushBackCommand<R, ADS>);
      loadCommand(new ListRemoveCommand<R, ADS>);
      loadCommand(new ListSilentDefineCommand<R, ADS>);
      loadCommand(new ListSortCommand<R, ADS>);
    }

    if ((prefix == "") || (prefix == "command")) {
      loadCommand(new CommandCreateCommand<R, ADS>);
      loadCommand(new CommandCreateLocalCommand<R, ADS>);
      loadCommand(new CommandCreateRawCommand<R, ADS>);
      loadCommand(new CommandExistsCommand<R, ADS>);
      loadCommand(new CommandRenameCommand<R, ADS>);
      loadCommand(new CommandUnitTestCommand<R, ADS>);
    }

    if ((prefix == "") || (prefix == "function")) {
      loadCommand(new FunctionCreateRawCommand<R, ADS>);
    }

    // deprecated
#ifdef MaPI
    loadCommand(new InitServersCommand<R, ADS>);
#endif

    if ((prefix == "") || (prefix == "plot")) {
      loadCommand(new Plot2AxisCommand<R, ADS>);
      loadCommand(new PlotViewCommand<R, ADS>);
    }

    // cannot load abstract module
    // loadCommand(new ProblemCommand<R, ADS> );

    if ((prefix == "") || (prefix == "randgen")) {
      loadCommand(new RandGenIntervalCommand<R, ADS>);
      loadCommand(new RandGenNumberCommand<R, ADS>);
      loadCommand(new RandGenSetSeedCommand<R, ADS>);
    }

    if ((prefix == "") || (prefix == "system")) {
      loadCommand(new SystemAssertCommand<R, ADS>);
      loadCommand(new SystemChDirCommand<R, ADS>);
      loadCommand(new SystemDefineCommand<R, ADS>);
      loadCommand(new SystemDictionaryCommand<R, ADS>);
      loadCommand(new SystemEchoCommand<R, ADS>);
      loadCommand(new SystemErrorCommand<R, ADS>);
      loadCommand(new SystemHelpCommand<R, ADS>);
      loadCommand(new SystemPauseCommand<R, ADS>);
      loadCommand(new SystemPreprocessCommand<R, ADS>);
      loadCommand(new SystemPrintCommand<R, ADS>);
      loadCommand(new SystemPrintlnCommand<R, ADS>);
      loadCommand(new SystemReadCommand<R, ADS>);
      loadCommand(new SystemRequireCommand<R, ADS>);
      loadCommand(new SystemRunCommand<R, ADS>);
      loadCommand(new SystemSilentDefineCommand<R, ADS>);
      loadCommand(new SystemUndefineCommand<R, ADS>);
      loadCommand(new SystemUnsafeDefineCommand<R, ADS>);
      loadCommand(new SystemUsageCommand<R, ADS>);
      loadCommand(new SystemUseCommand<R, ADS>);
    }

    if ((prefix == "") || (prefix == "operator")) {
      loadCommand(new OperatorAssignCommand<R, ADS>);
    }

    // ==========================================================
    // language statements (conditional, loop and error handling)
    // ==========================================================
    if ((prefix == "") || (prefix == "statements")) {
      loadCommand(new ForCommand<R, ADS>);
      loadCommand(new ForEachCommand<R, ADS>);
      loadCommand(new IfElseCommand<R, ADS>);
      loadCommand(new WhileCommand<R, ADS>);
      loadCommand(new TryCommand<R, ADS>);
    }

    // ----------------------------------------------

    if ((prefix == "") || (prefix == "list")) {
      loadFunction(new ListAppendFunction<R, ADS, DS>);
      // loadFunction(new ListDefinitionFunction<R,ADS,DS>);
      loadFunction(new ListElementFunction<R, ADS, DS>);
      loadFunction(new ListLengthFunction<R, ADS, DS>);
      loadFunction(new ListWordsFunction<R, ADS, DS>);
    }

    if ((prefix == "") || (prefix == "math")) {
      loadFunction(new MathRoundFunction<R, ADS, DS>);
      loadFunction(new MathTruncFunction<R, ADS, DS>);
    }

    if ((prefix == "") || (prefix == "operator")) {
      loadFunction(new OperatorAbsFunction<R, ADS, DS>);
      loadFunction(new OperatorCompareFunction<R, ADS, DS>);
      loadFunction(new OperatorComputeFunction<R, ADS, DS>);
      loadFunction(new OperatorInFunction<R, ADS, DS>);
      loadFunction(new OperatorLogicFunction<R, ADS, DS>);
      loadFunction(new OperatorSCompareFunction<R, ADS, DS>);
    }

    // statistics
    if ((prefix == "") || (prefix == "statistics")) {
      loadFunction(new StatisticsArgMaxFunction<R, ADS, DS>);
      loadFunction(new StatisticsArgMinFunction<R, ADS, DS>);
      loadFunction(new StatisticsANOVAFunction<R, ADS, DS>);
      loadFunction(new StatisticsAvgFunction<R, ADS, DS>);
      loadFunction(new StatisticsFriedmanNoBlockTestFunction<R, ADS, DS>);
      loadFunction(new StatisticsFriedmanTestFunction<R, ADS, DS>);
      loadFunction(new StatisticsMannUTestFunction<R, ADS, DS>);
      loadFunction(new StatisticsMaxFunction<R, ADS, DS>);
      loadFunction(new StatisticsMedFunction<R, ADS, DS>);
      loadFunction(new StatisticsMinFunction<R, ADS, DS>);
      loadFunction(new StatisticsPairedTTestFunction<R, ADS, DS>);
      loadFunction(new StatisticsShapiroTestFunction<R, ADS, DS>);
      loadFunction(new StatisticsSampleStdDevFunction<R, ADS, DS>);
      loadFunction(new StatisticsStdDevFunction<R, ADS, DS>);
      loadFunction(new StatisticsStudentTTestFunction<R, ADS, DS>);
      loadFunction(new StatisticsSumFunction<R, ADS, DS>);
      loadFunction(new StatisticsWilcoxonTestFunction<R, ADS, DS>);
    }

    if ((prefix == "") || (prefix == "system")) {
      loadFunction(new SystemDefinedFunction<R, ADS, DS>);
      loadFunction(new SystemDefinitionFunction<R, ADS, DS>);
      loadFunction(new SystemInputFunction<R, ADS, DS>);
      loadFunction(new SystemPOpenFunction<R, ADS, DS>);
      loadFunction(new SystemTimeFunction<R, ADS, DS>);
    }

    if ((prefix == "") || (prefix == "text")) {
      loadFunction(new TextConcatFunction<R, ADS, DS>);
      // loadFunction(new TextDefinitionFunction<R,ADS,DS>);
      loadFunction(new TextNextFunction<R, ADS, DS>);
    }
  }

  Command<R, ADS>* getCommand(string module) {
    for (unsigned int i = 0; i < modules.size(); i++)
      if (module == modules[i]->id()) return modules[i];
    return nullptr;
  }

  void loadActions() {
    factory.actions.push_back(new ComponentAction<R, ADS>);
    factory.actions.push_back(new RandGenAction<R, ADS>);
    factory.actions.push_back(new TimerAction<R, ADS>);
    factory.actions.push_back(new ConstructiveAction<R, ADS>);
    factory.actions.push_back(new EvaluatorAction<R, ADS>);
    factory.actions.push_back(new EvaluationAction<R, ADS>);
    factory.actions.push_back(new MoveAction<R, ADS>);
    factory.actions.push_back(new NSAction<R, ADS>);
    factory.actions.push_back(new NSSeqAction<R, ADS>);
    factory.actions.push_back(new NSIteratorAction<R, ADS>);
    factory.actions.push_back(new LocalSearchAction<R, ADS>);
    factory.actions.push_back(new SingleObjSearchAction<R, ADS>);
  }

  void loadComponentBuilders() {
    // Independent components
    factory.builders.push_back(new RandGenBuilder<R, ADS>);
    factory.builders.push_back(new TimerBuilder<R, ADS>);

    // Base
    factory.builders.push_back(new CloneConstructiveBuilder<R, ADS>);

    // LocalSearch
    factory.builders.push_back(new EmptyLocalSearchBuilder<R, ADS>);
    factory.builders.push_back(new BestImprovementBuilder<R, ADS>);
    factory.builders.push_back(new FirstImprovementBuilder<R, ADS>);
    factory.builders.push_back(new RandomDescentMethodBuilder<R, ADS>);
    factory.builders.push_back(new CircularSearchBuilder<R, ADS>);
    factory.builders.push_back(new VariableNeighborhoodDescentBuilder<R, ADS>);
    factory.builders.push_back(
        new VariableNeighborhoodDescentUpdateADSBuilder<R, ADS>);
    factory.builders.push_back(new RVNDBuilder<R, ADS>);
    factory.builders.push_back(new HillClimbingBuilder<R, ADS>);
    factory.builders.push_back(new LateAcceptanceHillClimbingBuilder<R, ADS>);

    // SingleObjSearch + Parameters
    factory.builders.push_back(new SimpleLocalSearchBuilder<R, ADS>);
    factory.builders.push_back(new BasicSimulatedAnnealingBuilder<R, ADS>);
    factory.builders.push_back(new BasicIteratedLocalSearchBuilder<R, ADS>);
    factory.builders.push_back(new BasicILSPerturbationBuilder<R, ADS>);
    factory.builders.push_back(new IteratedLocalSearchLevelsBuilder<R, ADS>);
    factory.builders.push_back(new ILSLPerturbationLPlus2Builder<R, ADS>);
    factory.builders.push_back(new ILSLPerturbationLPlus2ProbBuilder<R, ADS>);
    factory.builders.push_back(new GRASPBuilder<R, ADS>);
    factory.builders.push_back(new BasicVNSBuilder<R, ADS>);
    factory.builders.push_back(new ReducedVNSBuilder<R, ADS>);
    factory.builders.push_back(new GeneralVNSBuilder<R, ADS>);
    factory.builders.push_back(new RandVNSBuilder<R, ADS>);

    // test local searches
    factory.builders.push_back(new CompareLocalSearchBuilder<R, ADS>);
  }

  //! \english OptFrame Command Line Interface \endenglish \portuguese Interface
  //! de Linha de Comando do OptFrame \endportuguese
  /*!
        \sa execute(string)
        */

  void execute() {
    std::cout << "Welcome to " << welcome() << std::endl;

    Scanner scanner(&cin);
    string line;

    while (true) {
      std::cout << ">";
      line = scanner.nextLine();

      Scanner s2(line);
      s2.useSeparators(" \t\r\n=");

      string command = s2.next();

      if (command == "quit" || command == "q" || command == "exit") break;

      if (command == "version" || command == "v") {
        std::cout << version() << std::endl;
        continue;
      }

      bool notfound = true;
      string command_body = s2.rest();

      for (unsigned int i = 0; i < modules.size(); i++)
        if (modules[i]->canHandle(command, command_body)) {
          string* r1 = modules[i]->preprocess(functions, factory, dictionary,
                                              ldictionary, command_body);

          if (!r1) break;

          string r = *r1;
          delete r1;

          if (r == "INVALID_PARAM") {
            std::cout << "Population size has to be, at least, equal 2.\n";
            notfound = false;
            break;
          }

          if (!modules[i]->run(modules, functions, factory, dictionary,
                               ldictionary, r, command))
            std::cout << "command failed!" << std::endl;

          notfound = false;
          break;
        }

      if (notfound) {
        std::cout << "Sorry, i couldn't understand the command '" << command
                  << "'." << std::endl
                  << "Please, type 'system.help' or type the command again."
                  << std::endl;
        printOptions(command, modules);
      }
    }

    std::cout << "Goodbye." << std::endl;
  }

  static void printOptions(string part_command,
                           vector<Command<R, ADS>*>& allCommands) {
    for (unsigned int i = 0; i < allCommands.size(); i++) {
      bool found = true;

      for (unsigned j = 0; j < part_command.length(); j++)
        if (part_command[j] != allCommands[i]->id()[j]) {
          found = false;
          break;
        }

      if (found)
        std::cout << "option is: '" << allCommands[i]->id() << "'" << std::endl;
    }
  }

  //! \english Execute command in OptFrame Command Line Interface \endenglish
  //! \portuguese Executa comando na Interface de Linha de Comando do OptFrame
  //! \endportuguese
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

  void execute(string line) {
    // std::cout << "Welcome to " << version() << std::endl;

    Scanner s2(line);
    s2.useSeparators(" \t\r\n=");

    string command = s2.next();

    if (command == "quit" || command == "q" || command == "exit") return;

    if (command == "version" || command == "v") {
      std::cout << version() << std::endl;
      return;
    }

    bool notfound = true;
    string command_body = s2.rest();

    for (unsigned int i = 0; i < modules.size(); i++)
      if (modules[i]->canHandle(command, command_body)) {
        string* r = modules[i]->preprocess(functions, factory, dictionary,
                                           ldictionary, command_body);

        if (!r) break;

        if (*r == "INVALID_PARAM") {
          std::cout << "Population size has to be, at least, equal 2.\n";
          exit(1);
        }

        if (!modules[i]->run(modules, functions, factory, dictionary,
                             ldictionary, *r, command))
          std::cout << "error in command: '" << modules[i]->id() << "'"
                    << std::endl;

        delete r;

        notfound = false;
        break;
      }

    if (notfound)
      std::cout << "Sorry, i couldn't understand the command '" << command
                << "'." << std::endl
                << "Please, type 'system.help' or type the command again."
                << std::endl;

    // std::cout << "Goodbye." << std::endl;
  }
};

}  // namespace optframe

#endif /*OPTFRAME_INTERACTIVE_HPP_*/
