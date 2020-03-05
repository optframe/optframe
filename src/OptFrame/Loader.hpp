// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

//#include "Util/TestEvaluation.hpp"

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
#include "Constructive.hpp"

// local search
#include "Heuristics/Empty.hpp"
#include "Heuristics/LocalSearches/BestImprovement.hpp"
#include "Heuristics/LocalSearches/FirstImprovement.hpp"
#include "Heuristics/LocalSearches/RandomDescentMethod.hpp"
#include "Heuristics/LocalSearches/CircularSearch.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp"
#include "Heuristics/LocalSearches/VariableNeighborhoodDescentUpdateADS.hpp"
#include "Heuristics/LocalSearches/HillClimbing.hpp"
#include "Heuristics/LocalSearches/LateAcceptanceHillClimbing.hpp"

#include "Heuristics/SingleObjSearchToLocalSearch.hpp"

// single obj search
#include "Heuristics/SimpleLocalSearch.hpp"
#include "Heuristics/ILS/BasicIteratedLocalSearch.hpp"
#include "Heuristics/ILS/BasicILSPerturbation.hpp"
#include "Heuristics/SA/BasicSimulatedAnnealing.hpp"
#include "Heuristics/ILS/IteratedLocalSearchLevels.hpp"
#include "Heuristics/ILS/ILSLPerturbation.hpp"
#include "Heuristics/GRASP/BasicGRASP.hpp"
#include "Heuristics/VNS/VariableNeighborhoodSearch.hpp"
#include "Heuristics/VNS/BasicVNS.hpp"
#include "Heuristics/VNS/ReducedVNS.hpp"
#include "Heuristics/VNS/GeneralVNS.hpp"

//TODO ERROR on DecoderNSGAII
//#include "Heuristics/EvolutionaryAlgorithms/DecoderNSGAII.hpp"

// test local searches
#include "Heuristics/CompareLocalSearch.hpp"

#include "RandGen.hpp"
#include "Util/RandGenMersenneTwister.hpp"

#include "BaseConcepts.hpp"

using namespace std;

namespace optframe
{

//template<XSolution S, XEvaluation XEv = Evaluation<>>
// template<XRepresentation R, class ADS, XBaseSolution<R,ADS> S = CopySolution<R,ADS>, XEvaluation XEv = Evaluation<>>
template<XRepresentation R, class ADS, XBaseSolution<R, ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<S, XEv, XES>>
class Loader
{
public:

	HeuristicFactory<S, XEv, XES, X2ES> factory;
	map<string, string> dictionary;
	map<string, vector<string> > ldictionary;


	Loader()
	{
		loadComponentBuilders();
	}

	Loader(RandGen _rg) :
		factory(HeuristicFactory<S, XEv, XES, X2ES> (_rg))
	{
		loadComponentBuilders();
	}


	void loadComponentBuilders()
	{
		// Independent components
		factory.builders.push_back(new RandGenBuilder<S, XEv> );
		factory.builders.push_back(new TimerBuilder<S, XEv> );

		// Base
		factory.builders.push_back(new CloneConstructiveBuilder<S, XEv> );

		// LocalSearch
		factory.builders.push_back(new EmptyLocalSearchBuilder<S, XEv> );
		factory.builders.push_back(new BestImprovementBuilder<S, XEv> );
		factory.builders.push_back(new FirstImprovementBuilder<S, XEv> );
		factory.builders.push_back(new RandomDescentMethodBuilder<S, XEv> );
		factory.builders.push_back(new CircularSearchBuilder<S, XEv> );
		factory.builders.push_back(new VariableNeighborhoodDescentBuilder<S, XEv> );
		factory.builders.push_back(new VariableNeighborhoodDescentUpdateADSBuilder<R, ADS, S, XEv> );
		//factory.builders.push_back(new RVNDBuilder<S, XEv> );
		factory.builders.push_back(new HillClimbingBuilder<S, XEv> );
		factory.builders.push_back(new LateAcceptanceHillClimbingBuilder<S, XEv> );
		factory.builders.push_back(new SingleObjSearchToLocalSearchBuilder<S, XEv> );

		// SingleObjSearch + Parameters
		factory.builders.push_back(new SimpleLocalSearchBuilder<S, XEv> );
		factory.builders.push_back(new BasicSimulatedAnnealingBuilder<S, XEv, XES> );
		factory.builders.push_back(new BasicIteratedLocalSearchBuilder<S, XEv> );
		factory.builders.push_back(new BasicILSPerturbationBuilder<S, XEv> );
		factory.builders.push_back(new IteratedLocalSearchLevelsBuilder<S, XEv> );
		factory.builders.push_back(new ILSLPerturbationLPlus2Builder<S, XEv> );
		factory.builders.push_back(new ILSLPerturbationLPlus2ProbBuilder<S, XEv> );
		factory.builders.push_back(new BasicGRASPBuilder<S, XEv> );
		factory.builders.push_back(new BasicVNSBuilder<S, XEv> );
		factory.builders.push_back(new ReducedVNSBuilder<S, XEv> );
		factory.builders.push_back(new GeneralVNSBuilder<S, XEv> );

		// test local searches
		factory.builders.push_back(new CompareLocalSearchBuilder<S, XEv> );
	}

};

}

#endif /*OPTFRAME_INTERACTIVE_HPP_*/
