// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_HYPER_LOADER_HPP_
#define OPTFRAME_HYPER_LOADER_HPP_

#if (__cplusplus < 202302L) || !defined(OPTFRAME_CXX_MODULES)

/*! \mainpage OptFrame
 * This is the OptFrame documentation.
 */
// Esta é a documentação do OptFrame.
//
//  C
#include <stdlib.h>
// #include <unistd.h>
// C++
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/Evaluator.hpp>
#include <OptFrame/Core/Move.hpp>
#include <OptFrame/Helper/ADSManager.hpp>
#include <OptFrame/Helper/InitialMultiESolution.hpp>
#include <OptFrame/Helper/Population.hpp>
#include <OptFrame/Helper/Solution.hpp>
#include <OptFrame/Pareto/MultiEvaluator.hpp>
#include <OptFrame/Pareto/MultiEvaluatorBuilder.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Search/InitialPopulation.hpp>
#include <OptFrame/Timer.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

// ==================================
//       Neighborhood Structure
// ==================================

#include <OptFrame/Core/NS.hpp>
#include <OptFrame/Core/NSEnum.hpp>
#include <OptFrame/Core/NSSeq.hpp>

// ==================================
//        Parallel Support
// ==================================

#include <OptFrame/Parallel/Parallel.h>

// ==================================
//            Heuristic
// ==================================

#include <OptFrame/Hyper/HeuristicFactory.hpp>
#include <OptFrame/Hyper/RandGenBuilder.hpp>
#include <OptFrame/Hyper/TimerBuilder.hpp>
#include <OptFrame/Search/MultiObjSearch.hpp>
#include <OptFrame/Search/SingleObjSearch.hpp>

// base components
#include <OptFrame/Core/Constructive.hpp>
#include <OptFrame/Helper/CloneConstructive.hpp>

// local search
#include <OptFrame/Heuristics/LocalSearches/BI.hpp>
#include <OptFrame/Heuristics/LocalSearches/CS.hpp>
#include <OptFrame/Heuristics/LocalSearches/EmptyLS.hpp>
#include <OptFrame/Heuristics/LocalSearches/FI.hpp>
#include <OptFrame/Heuristics/LocalSearches/HC.hpp>
#include <OptFrame/Heuristics/LocalSearches/LAHC.hpp>
#include <OptFrame/Heuristics/LocalSearches/RDM.hpp>
#include <OptFrame/Heuristics/LocalSearches/VND.hpp>
#include <OptFrame/Heuristics/LocalSearches/VNDUpdateADS.hpp>
#include <OptFrame/Heuristics/SingleObjSearchToLocalSearch.hpp>

// general builders
#include <OptFrame/Search/GlobalSearchBuilder.hpp>

// single obj search
#include <OptFrame/Heuristics/GRASP/BasicGRASP.hpp>
#include <OptFrame/Heuristics/ILS/BasicILSPerturbation.hpp>
#include <OptFrame/Heuristics/ILS/BasicIteratedLocalSearch.hpp>
#include <OptFrame/Heuristics/ILS/ILSLPerturbation.hpp>
#include <OptFrame/Heuristics/ILS/ILSLPerturbationBuilder.hpp>
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevelsBuilder.hpp>
#include <OptFrame/Heuristics/MultiStart.hpp>
#include <OptFrame/Heuristics/SA/BasicSimulatedAnnealing.hpp>
#include <OptFrame/Heuristics/SA/BasicSimulatedAnnealingBuilder.hpp>
#include <OptFrame/Heuristics/SA/GeometricSimulatedAnnealing.hpp>
#ifdef MOVE_IS_COMPONENT
#include <OptFrame/Heuristics/SA/SimulatedAnnealingAC.hpp>
#endif
#include <OptFrame/Heuristics/SimpleLocalSearch.hpp>
#include <OptFrame/Heuristics/TS/BasicTabuSearch.hpp>
#include <OptFrame/Heuristics/VNS/BasicVNS.hpp>
#include <OptFrame/Heuristics/VNS/GeneralVNS.hpp>
#include <OptFrame/Heuristics/VNS/ReducedVNS.hpp>
#include <OptFrame/Heuristics/VNS/VariableNeighborhoodSearch.hpp>

// TODO ERROR on DecoderNSGAII
// #include <OptFrame/Heuristics/EvolutionaryAlgorithms/DecoderNSGAII.hpp"

#include <OptFrame/Heuristics/EA/RK/BRKGA.hpp>
#include <OptFrame/Heuristics/EA/RK/BRKGABuilder.hpp>
#include <OptFrame/Heuristics/EA/RK/BasicDecoderRandomKeys.hpp>
#include <OptFrame/Heuristics/EA/RK/BasicInitialEPopulationRK.hpp>
#include <OptFrame/Heuristics/MultiObjective/MOPopulationManagement.hpp>

// test local searches
#include <OptFrame/Concepts/BaseConcepts.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Heuristics/CompareLocalSearch.hpp>
#include <OptFrame/Util/RandGenMersenneTwister.hpp>

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

// using namespace std;

namespace optframe {

// template<XESolution XES, XEvaluation XEv = Evaluation<>>
//  template<XRepresentation R, class ADS, XBaseSolution<R,ADS> S =
//  CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
//
//
#ifdef OPTFRAME_LEGACY_R_ADS
MOD_EXPORT template <XRepresentation R, class ADS, XBaseSolution<R, ADS> S,
                     XEvaluation XEv = Evaluation<>,
                     XESolution XES = pair<S, XEv>, XSearch<XES> XSH = XES,
                     XESolution XES2 = XES,
                     X2ESolution<XES> X2ES = MultiESolution<XES>>
#else

// ==============================================
// Which is better?
// = VEPopulation<XES2> vs = MultiESolution<XES2>
// ==============================================

// #if defined(__cpp_concepts) && (__cpp_concepts >= 201907L)
#ifdef OPTFRAME_USE_STD_CONCEPTS
MOD_EXPORT template <XESolution XES, XSearch<XES> XSH = XES,
                     XESolution XES2 = XES,
                     X2ESolution<XES2> X2ES = VEPopulation<XES2>>
#else
MOD_EXPORT template <typename XES, typename XSH = XES, typename XES2 = XES,
                     typename X2ES = VEPopulation<XES2>>

#endif  // cpp_concepts

#endif
class Loader {
#ifdef OPTFRAME_LEGACY_R_ADS
//
#else
  using S = typename XES::first_type;
  using XEv = typename XES::second_type;
#endif

  // TODO: add to Loader and Factory template!!!
  using XMEv = MultiEvaluation<typename XEv::objType>;
  using XMES = std::pair<S, XMEv>;
  using X2MES = VEPopulation<XMES>;

 public:
  HeuristicFactory<XES> factory;
  std::map<std::string, std::string> dictionary;
  std::map<std::string, std::vector<std::string>> ldictionary;

  Loader() { loadComponentBuilders(); }

  explicit Loader(sref<RandGen> _rg) : factory{HeuristicFactory<XES>{_rg}} {
    loadComponentBuilders();
  }

  void loadComponentBuilders() {
    // Independent components
    factory.builders.push_back(new RandGenBuilder<XES>);
    factory.builders.push_back(new TimerBuilder<XES>);

    // Base
    factory.builders.push_back(new CloneConstructiveBuilder<XES>);

    // official
    factory.builders.push_back(new MultiEvaluatorBuilder<XES>);
    // alternative on 'xmes_builders'... TODO: think about it.
    //
    // we don't REALLY need xmes, as long as we can DEDUCE it from S, XEv.
    // S and XEv can be considered BASE types of Multi types as well.
    ComponentMultiBuilder<S, XMEv, XMES, X2MES>* meb =
        new MultiEvaluatorMultiBuilder<S, XMEv, XMES, X2MES>{};
    factory.xmes_builders.push_back(meb);
    //
    factory.builders.push_back(new BasicInitialMultiESolutionBuilder<XES>);

    factory.builders.push_back(new BasicMOPopulationManagementBuilder<XES>);

    // LocalSearch
    factory.builders.push_back(new BuilderEmptyLS<XES>);
    factory.builders.push_back(new BuilderBI<XES>);
    factory.builders.push_back(new BuilderFI<XES>);
    factory.builders.push_back(new BuilderRDM<XES>);
    factory.builders.push_back(new BuilderCS<XES>);
    factory.builders.push_back(new BuilderVND<XES>);
    factory.builders.push_back(new BuilderRVND<XES>);
#ifdef OPTFRAME_LEGACY_R_ADS
    factory.builders.push_back(new BuilderVNDUpdateADS<R, ADS, S, XEv>);
#endif
    // factory.builders.push_back(new RVNDBuilder<XES> );
    factory.builders.push_back(new BuilderHC<XES>);
    factory.builders.push_back(new BuilderLAHC<XES>);
    factory.builders.push_back(new SingleObjSearchToLocalSearchBuilder<XES>);

    // SingleObjSearch + Parameters
    factory.builders.push_back(new SimpleLocalSearchBuilder<XES>);
    factory.builders.push_back(
        new BasicSimulatedAnnealingBuilder<XSH, XES2, X2ES>);
    factory.builders.push_back(
        new GeometricSimulatedAnnealingBuilder<XSH, XES2, X2ES>);
#ifdef MOVE_IS_COMPONENT
    factory.builders.push_back(
        new SimulatedAnnealingACBuilder<XSH, XES2, X2ES>);
#endif
    factory.builders.push_back(new MultiStartBuilder<XSH, XES2, X2ES>);
    factory.builders.push_back(new BasicTabuSearchBuilder<XES>);
    factory.builders.push_back(new BasicIteratedLocalSearchBuilder<XES>);
    factory.builders.push_back(new BasicILSPerturbationBuilder<XES>);
    factory.builders.push_back(new IteratedLocalSearchLevelsBuilder<XES>);
    factory.builders.push_back(new ILSLPerturbationLPlus2Builder<XES>);
    factory.builders.push_back(new ILSLPerturbationLPlus2ProbBuilder<XES>);
    factory.builders.push_back(new BasicGRASPBuilder<XES>);
    factory.builders.push_back(new BasicVNSBuilder<XES>);
    factory.builders.push_back(new ReducedVNSBuilder<XES>);
    factory.builders.push_back(new GeneralVNSBuilder<XES>);

    // RK family (random keys)
    // static_assert(X2ESolution<XES, MultiESolution<XES>>);
    // factory.builders.push_back(new BRKGABuilder<XES>);

    // GlobalSearchBuilder
    // using XES2_BRKGA = std::pair<std::vector<double>, Evaluation<>>;
    // using XSH2_BRKGA = VEPopulation<XES2_BRKGA>;
    //
    // Ignoring other BRKGA type parameters...
    // In the future, perhaps try to make other specific lists,
    // such as ITrajectory or IPopulational (if users want to have specific
    // features, like onIncumbent(...) callback. For Multi Objective, must see
    // benefits.
    factory.builders.push_back(new BRKGABuilder<XES>);
    factory.builders.push_back(new BasicInitialEPopulationRKBuilder<XES>);
    factory.builders.push_back(new BasicDecoderRandomKeysBuilder<XES>);

    // test local searches
    factory.builders.push_back(new CompareLocalSearchBuilder<XES>);
  }
};

}  // namespace optframe

#endif  // OPTFRAME_HYPER_LOADER_HPP_
