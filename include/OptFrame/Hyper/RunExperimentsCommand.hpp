// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2025 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_UTIL_RUNEXPERIMENTSCOMMAND_HPP_  // NOLINT
#define OPTFRAME_UTIL_RUNEXPERIMENTSCOMMAND_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

#include <string>
#include <vector>
//
#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/NS.hpp>
#include <OptFrame/Core/NSEnum.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Hyper/HeuristicFactory.hpp>
#include <OptFrame/Hyper/OptFrameList.hpp>
#include <OptFrame/Search/InitialSearch.hpp>
#include <OptFrame/Timer.hpp>
#include <OptFrame/Util/KahanSummation.hpp>
#include <OptFrame/Util/Matrix.hpp>

#define MOD_EXPORT
#else

// CANNOT IMPORT HERE... Already part of optframe.core
/*
import std;
import optframe.component;
import optframe.concepts;
*/

// do NOT import/export modules on .hpp... only on .cppm

#define MOD_EXPORT export

#endif

// Windows does not accept inner namespace!!!
// namespace optframe::hyper {
namespace optframe {

// windows does not  accept this inner namespace!!!
// namespace hyper {

struct TimeDataRunExperimentsCommand {
  // NOLINTNEXTLINE
  TimeDataRunExperimentsCommand() {}

  void initialize(int numRuns) {
    this->buildTimes = vector<vector<double>>(numRuns);
    this->warmupTimes = vector<vector<double>>(numRuns);
    this->runTimes = vector<vector<double>>(numRuns);
    this->fullTimes = vector<vector<double>>(numRuns);
    this->timeToBestTimes = vector<vector<double>>(numRuns);
  }

  // for each method, several runs
  vector<vector<double>> buildTimes;
  vector<vector<double>> warmupTimes;
  vector<vector<double>> runTimes;
  vector<vector<double>> fullTimes;
  vector<vector<double>> timeToBestTimes;
};

template <XESolution XES>
struct SolDataRunExperimentsCommand {
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
  vector<vector<op<XES>>> opResults;
};

template <XESolution XES>
struct AllDataRunExperimentsCommand {
  // NOLINTNEXTLINE
  AllDataRunExperimentsCommand() {}

  TimeDataRunExperimentsCommand timeData;
  SolDataRunExperimentsCommand<XES> solData;

  bool status;
};

#if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
template <XESolution XES, XSolution S = typename XES::first_type,
          XRepresentation REP = S, class ADS = int,
          X2ESolution<XES> X2ES = MultiESolution<XES>,
          XSearch<XES> XSH = std::pair<S, typename XES::second_type>>
#else
template <typename XES, typename S = typename XES::first_type, typename REP = S,
          class ADS = int, typename X2ES = MultiESolution<XES>,
          typename XSH = std::pair<S, typename XES::second_type>>
#endif
class RunExperimentsCommand : public Component {  // NOLINT
  using XEv = typename XES::second_type;

  static_assert(is_same<S, typename XES::first_type>::value);
  static_assert(is_same<XEv, typename XES::second_type>::value);

 public:
  explicit RunExperimentsCommand(bool _verbose = false) {
    //
    if (verbose)
      this->setMessageLevel(modlog::LogLevel::Debug);
    else
      this->setMessageLevel(modlog::LogLevel::Info);
  }

  ~RunExperimentsCommand() override = default;

  // setForceVerbose only works for verbosity ON and OFF (disabled)
  void setForceVerbose(bool forceVerbose) {
    if (forceVerbose) {
      this->setMessageLevel(modlog::LogLevel::Debug);
      Log(modlog::LogLevel::Debug)
          << "RunExperiments setParameters to verbose" << std::endl;
    } else {
      this->setMessageLevel(modlog::LogLevel::Disabled);
    }
  }

  std::string toString() const override { return "RunExperimentsCommand"; }

 public:
  AllDataRunExperimentsCommand<XES> run(int numRuns,
                                        std::vector<std::string> builders,
                                        HeuristicFactory<XES>& factory,
                                        double timelimit, op<XEv> targetValue,
                                        std::string_view outputFile = "",
                                        int firstSeed = -1) {
    using modlog::LogLevel::Info;
    AllDataRunExperimentsCommand<XES> data;

    bool printStdOut = false;
    if (outputFile.empty()) printStdOut = true;

    int numMethods = builders.size();

    data.solData.opResults = std::vector<std::vector<op<XES>>>(numRuns);
    data.timeData.initialize(numRuns);
    for (int exp = 0; exp < numRuns; exp++) {
      Log(Info, &factory) << "begin experiment " << exp << " .. "
                          << (numRuns - 1) << std::endl;
      data.solData.opResults[exp] = std::vector<op<XES>>(numMethods);
      data.timeData.fullTimes[exp] = std::vector<double>(numMethods);
      data.timeData.buildTimes[exp] = std::vector<double>(numMethods);
      data.timeData.runTimes[exp] = std::vector<double>(numMethods);
      data.timeData.timeToBestTimes[exp] = std::vector<double>(numMethods);

      for (int method = 0; method < numMethods; method++) {
        Log(Info, &factory)
            << "begin method '" << builders[method] << "'" << std::endl;

        if (firstSeed >= 0) {
          Log(Info, &factory)
              << "setting seed = " << (firstSeed + exp) << std::endl;
          factory.getRandGen()->setSeed(firstSeed + exp);
        } else {
          Log(Info, &factory) << "do not changing seed!" << std::endl;
        }

        Timer tFull;
        Timer tBuild;
        Scanner scanner{builders[method]};
        std::string builder = scanner.next();
        auto* cb = factory.getBuilder(builder);
        using CBSingle = optframe::SingleObjSearchBuilder<XES>;
        CBSingle* cbsingle = (CBSingle*)cb;
        optframe::SingleObjSearch<XES>* single =
            cbsingle->build(scanner, factory);
        data.timeData.buildTimes[exp][method] = tBuild.now();
        Log(Info, &factory) << "Built Method " << single << " in "
                            << tBuild.now() << " secs" << std::endl;
        if (single) single->print();
        Log(Info, &factory)
            << "Running method for max " << timelimit << " secs" << std::endl;
        Timer tRun;
        auto sout = single->search({timelimit});
        data.timeData.runTimes[exp][method] = tBuild.now();
        data.timeData.timeToBestTimes[exp][method] = sout.timeBest;
        Log(Info, &factory)
            << "Run Method in " << tRun.now() << " secs" << std::endl;
        if (sout.best) {
          Log(Info, &factory)
              << "Method best = " << sout.best->second.evaluation() << " in "
              << sout.timeBest << " secs" << std::endl;
          data.solData.opResults[exp][method] = sout.best;
        }
        delete single;
        data.timeData.fullTimes[exp][method] = tFull.now();
        Log(Info, &factory) << "Finished experiment " << exp << " in "
                            << tFull.now() << " secs" << std::endl;
      }
    }

    std::cout << std::endl;
    std::cout << "============= results =============" << std::endl;

    std::cout << "#exp\t";
    for (int method = 0; method < numMethods; method++) {
      std::cout << "method_" << method << "\t";
      std::cout << "method_" << method << "\t";
      std::cout << "method_" << method << "\t";
      std::cout << "method_" << method << "\t";
    }
    std::cout << std::endl;
    std::cout << "#exp\t";
    for (int method = 0; method < numMethods; method++) {
      std::cout << "bestValue" << "\t";
      std::cout << "timeBest" << "\t";
      std::cout << "timeRun" << "\t";
      std::cout << "timeFull" << "\t";
    }
    std::cout << std::endl;

    for (int exp = 0; exp < numRuns; exp++) {
      std::cout << "#" << exp << "\t";
      for (int method = 0; method < numMethods; method++) {
        std::cout << data.solData.opResults[exp][method]->second.evaluation()
                  << "\t";
        std::cout << data.timeData.timeToBestTimes[exp][method] << "\t";
        std::cout << data.timeData.runTimes[exp][method] << "\t";
        std::cout << data.timeData.fullTimes[exp][method] << "\t";
      }
      std::cout << std::endl;
    }
    return data;
  }
};

// }  // namespace hyper

}  // namespace optframe

#endif  // OPTFRAME_UTIL_RUNEXPERIMENTSCOMMAND_HPP_ // NOLINT
