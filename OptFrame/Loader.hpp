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

#include "Constructive.h"
#include "InitialPopulation.h"

#include "Move.hpp"
//#include "Util/TestMove.hpp"

#include "Evaluation.hpp"
//#include "Util/TestEvaluation.hpp"
#include "Evaluator.hpp"

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

#include "Heuristics/EvolutionaryAlgorithms/DecoderNSGAII.hpp"

// test local searches
#include "Heuristics/CompareLocalSearch.hpp"

#include "RandGen.hpp"
#include "Util/RandGenMersenneTwister.hpp"


using namespace std;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Loader
{
public:

	HeuristicFactory<R, ADS, DS> factory;
	map<string, string> dictionary;
	map<string, vector<string> > ldictionary;


	Loader()
	{
		loadComponentBuilders();
		loadActions();
	}

	Loader(RandGen _rg) :
		factory(HeuristicFactory<R, ADS, DS> (_rg))
	{
		loadComponentBuilders();
		loadActions();
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
		factory.builders.push_back(new SingleObjSearchToLocalSearchBuilder<R, ADS, DS> );

		// SingleObjSearch + Parameters
		factory.builders.push_back(new SimpleLocalSearchBuilder<R, ADS, DS> );
		factory.builders.push_back(new BasicSimulatedAnnealingBuilder<R, ADS, DS> );
		factory.builders.push_back(new BasicIteratedLocalSearchBuilder<R, ADS, DS> );
		factory.builders.push_back(new BasicILSPerturbationBuilder<R, ADS, DS> );
		factory.builders.push_back(new IteratedLocalSearchLevelsBuilder<R, ADS, DS> );
		factory.builders.push_back(new ILSLPerturbationLPlus2Builder<R, ADS, DS> );
		factory.builders.push_back(new ILSLPerturbationLPlus2ProbBuilder<R, ADS, DS> );
		factory.builders.push_back(new BasicGRASPBuilder<R, ADS, DS> );
		factory.builders.push_back(new BasicVNSBuilder<R, ADS, DS> );
		factory.builders.push_back(new ReducedVNSBuilder<R, ADS, DS> );
		factory.builders.push_back(new GeneralVNSBuilder<R, ADS, DS> );

		// test local searches
		factory.builders.push_back(new CompareLocalSearchBuilder<R, ADS, DS> );
	}

};

}

#endif /*OPTFRAME_INTERACTIVE_HPP_*/
