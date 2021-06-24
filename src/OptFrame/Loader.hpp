// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_INTERACTIVE_HPP_
#define OPTFRAME_INTERACTIVE_HPP_

/*! \mainpage OptFrame
 * This is the OptFrame documentation.
 */
//Esta é a documentação do OptFrame.
#include <iostream>
#include <ostream>

#include <map>
#include <vector>

#include <stdlib.h>
#include <unistd.h>

#include "Scanner++/Scanner.hpp"
#include "Util/Matrix.hpp"
#include <OptFrame/printable/printable.hpp>

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

//#include "Util/TestEvaluation.hpp"

//#include "RandGen.hpp"
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

#include "MultiObjSearch.hpp"
#include "SingleObjSearch.hpp"

#include "HeuristicFactory.hpp"

// base components
#include "CloneConstructive.hpp"
#include "Constructive.hpp"

// local search
#include "Heuristics/Empty.hpp"
#include "Heuristics/LocalSearches/BestImprovement.hpp"
#include "Heuristics/LocalSearches/CircularSearch.hpp"
#include "Heuristics/LocalSearches/FirstImprovement.hpp"
#include "Heuristics/LocalSearches/HillClimbing.hpp"
#include "Heuristics/LocalSearches/LateAcceptanceHillClimbing.hpp"
#include "Heuristics/LocalSearches/RandomDescentMethod.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescentUpdateADS.hpp"

#include "Heuristics/SingleObjSearchToLocalSearch.hpp"

// single obj search
#include "Heuristics/GRASP/BasicGRASP.hpp"
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

//TODO ERROR on DecoderNSGAII
//#include "Heuristics/EvolutionaryAlgorithms/DecoderNSGAII.hpp"

// test local searches
#include "Heuristics/CompareLocalSearch.hpp"

#include "RandGen.hpp"
#include "Util/RandGenMersenneTwister.hpp"

#include "BaseConcepts.hpp"

using namespace std;

namespace optframe {

//template<XESolution XES, XEvaluation XEv = Evaluation<>>
// template<XRepresentation R, class ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
template<XRepresentation R, class ADS, XBaseSolution<R, ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
class Loader
{
public:
   HeuristicFactory<S, XEv, XES, X2ES> factory;
   map<string, string> dictionary;
   map<string, vector<string>> ldictionary;

   Loader()
   {
      loadComponentBuilders();
   }

   Loader(RandGen _rg)
     : factory(HeuristicFactory<S, XEv, XES, X2ES>(_rg))
   {
      loadComponentBuilders();
   }

   void loadComponentBuilders()
   {
      // Independent components
      factory.builders.push_back(new RandGenBuilder<S, XEv>);
      factory.builders.push_back(new TimerBuilder<S, XEv>);

      // Base
      factory.builders.push_back(new CloneConstructiveBuilder<S, XEv>);

      // LocalSearch
      factory.builders.push_back(new EmptyLocalSearchBuilder<S, XEv>);
      factory.builders.push_back(new BestImprovementBuilder<S, XEv>);
      factory.builders.push_back(new FirstImprovementBuilder<S, XEv>);
      factory.builders.push_back(new RandomDescentMethodBuilder<S, XEv>);
      factory.builders.push_back(new CircularSearchBuilder<S, XEv>);
      factory.builders.push_back(new VariableNeighborhoodDescentBuilder<S, XEv>);
      factory.builders.push_back(new VariableNeighborhoodDescentUpdateADSBuilder<R, ADS, S, XEv>);
      //factory.builders.push_back(new RVNDBuilder<S, XEv> );
      factory.builders.push_back(new HillClimbingBuilder<S, XEv>);
      factory.builders.push_back(new LateAcceptanceHillClimbingBuilder<S, XEv>);
      factory.builders.push_back(new SingleObjSearchToLocalSearchBuilder<S, XEv>);

      // SingleObjSearch + Parameters
      factory.builders.push_back(new SimpleLocalSearchBuilder<S, XEv>);
      factory.builders.push_back(new BasicSimulatedAnnealingBuilder<S, XEv, XES>);
      factory.builders.push_back(new BasicIteratedLocalSearchBuilder<S, XEv>);
      factory.builders.push_back(new BasicILSPerturbationBuilder<S, XEv>);
      factory.builders.push_back(new IteratedLocalSearchLevelsBuilder<S, XEv>);
      factory.builders.push_back(new ILSLPerturbationLPlus2Builder<S, XEv>);
      factory.builders.push_back(new ILSLPerturbationLPlus2ProbBuilder<S, XEv>);
      factory.builders.push_back(new BasicGRASPBuilder<S, XEv>);
      factory.builders.push_back(new BasicVNSBuilder<S, XEv>);
      factory.builders.push_back(new ReducedVNSBuilder<S, XEv>);
      factory.builders.push_back(new GeneralVNSBuilder<S, XEv>);

      // test local searches
      factory.builders.push_back(new CompareLocalSearchBuilder<S, XEv>);
   }
};

}

#endif /*OPTFRAME_INTERACTIVE_HPP_*/
