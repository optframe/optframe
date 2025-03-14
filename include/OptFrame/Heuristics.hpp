#ifndef OPTFRAME_HEURISTICS_HPP
#define OPTFRAME_HEURISTICS_HPP

// Heuristics Module: inclusion of common heuristic strategies

// LocalSearches
#include "Heuristics/LocalSearches/BestImprovement.hpp"

// SA family
#include "Heuristics/SA/SA.hpp"
//
#include "Heuristics/SA/BasicSimulatedAnnealing.hpp"
//
// ILS family
#include "Heuristics/ILS/ILS.hpp"
//

#include "Heuristics/ILS/IteratedLocalSearch.hpp"
//
#include "Heuristics/ILS//ILSLPerturbation.hpp"
#include "Heuristics/ILS/IteratedLocalSearchLevels.hpp"

#endif  // OPTFRAME_HEURISTICS_HPP