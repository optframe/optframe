module;

#include <assert.h>
// #include <math.h>  // ::fabs
// #include <stdio.h>

// #include <string>

export module optframe.hyper;
export import optframe.heuristics;
export import optframe.scannerpp;
export import optframe.nnptr;

export import std;

#include <OptFrame/Hyper/ComponentBuilder.hpp>
//
#include <OptFrame/Hyper/ComponentMultiBuilder.hpp>
//
#include <OptFrame/Hyper/ComponentHelper.hpp>
//
#include <OptFrame/Hyper/Action.hpp>
//
#include <OptFrame/Search/GlobalSearchBuilder.hpp>
#include <OptFrame/Search/LocalSearchBuilder.hpp>
//
#include <OptFrame/Search/SingleObjSearchBuilder.hpp>
//
#include <OptFrame/Hyper/HeuristicFactory.hpp>
// general unordered
#include <OptFrame/Heuristics/VNS/VNS.h>

#include <OptFrame/Helper/CloneConstructive.hpp>
#include <OptFrame/Helper/InitialMultiESolution.hpp>
#include <OptFrame/Heuristics/LocalSearches/BestImprovementBuilder.hpp>
#include <OptFrame/Heuristics/LocalSearches/CircularSearch.hpp>  // todo: move to heuristics module
#include <OptFrame/Heuristics/LocalSearches/CircularSearchBuilder.hpp>
#include <OptFrame/Heuristics/LocalSearches/EmptyBuilder.hpp>
#include <OptFrame/Heuristics/LocalSearches/FirstImprovement.hpp>  // todo: move to heuristics module
#include <OptFrame/Heuristics/LocalSearches/FirstImprovementBuilder.hpp>
#include <OptFrame/Heuristics/LocalSearches/HillClimbing.hpp>  // todo: move to heuristics module
#include <OptFrame/Heuristics/LocalSearches/HillClimbingBuilder.hpp>
#include <OptFrame/Heuristics/LocalSearches/LateAcceptanceHillClimbing.hpp>
#include <OptFrame/Heuristics/LocalSearches/RandomDescentMethod.hpp>
#include <OptFrame/Heuristics/LocalSearches/RandomDescentMethodBuilder.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>  // todo: move to heuristics module
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescentBuilder.hpp>
#include <OptFrame/Heuristics/SA/BasicSimulatedAnnealingBuilder.hpp>
#include <OptFrame/Heuristics/SimpleLocalSearch.hpp>
#include <OptFrame/Heuristics/SingleObjSearchToLocalSearch.hpp>
#include <OptFrame/Heuristics/VNS/VariableNeighborhoodSearch.hpp>
#include <OptFrame/Hyper/RandGenBuilder.hpp>
#include <OptFrame/Hyper/TimerBuilder.hpp>
#include <OptFrame/Pareto/MultiEvaluatorBuilder.hpp>
//
// metaheuristics unordered
//
#include <OptFrame/Heuristics/TS/TS.h>

#include <OptFrame/Heuristics/GRASP/BasicGRASP.hpp>
#include <OptFrame/Heuristics/ILS/BasicILSPerturbation.hpp>
#include <OptFrame/Heuristics/ILS/BasicIteratedLocalSearch.hpp>
#include <OptFrame/Heuristics/ILS/ILSLPerturbationBuilder.hpp>
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevelsBuilder.hpp>
#include <OptFrame/Heuristics/MultiStart.hpp>
#include <OptFrame/Heuristics/SA/GeometricSimulatedAnnealing.hpp>
#include <OptFrame/Heuristics/SA/SimulatedAnnealingAC.hpp>
#include <OptFrame/Heuristics/TS/BasicTabuSearch.hpp>
#include <OptFrame/Heuristics/VNS/BasicVNS.hpp>

//
// order-specific
//
#include <OptFrame/Heuristics/EA/GeneralCrossover.hpp>
//
#include <OptFrame/Heuristics/MultiObjective/MOSIndividual.hpp>
//
#include <OptFrame/Heuristics/MultiObjective/MOPopulationManagement.hpp>

// ALMOST DONE (IIP)...
// #include <OptFrame/Hyper/Loader.hpp>
