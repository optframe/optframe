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

#ifndef OPTFRAME_GENERAL_ELECTION_HPP_
#define OPTFRAME_GENERAL_ELECTION_HPP_

#include <chrono>
#include <random>
#include <limits>

#include "../../Evaluation.hpp"
#include "../../BaseConcepts.hpp"
#include "../../Util/printable.h"
#include "../../MultiSolution.hpp"
#include "../../MultiEvaluation.hpp"
#include "EA.h"

#ifndef _OPTFRAME_DBG_ELECTION_
#   ifdef OPTFRAME_DEBUG
#       define _OPTFRAME_DBG_ELECTION_ 
#   else
#       define _OPTFRAME_DBG_ELECTION_ while(false)
#   endif /* OPTFRAME_DEBUG */
#endif /* _OPTFRAME_DBG_ELECTION_ */

namespace optframe
{

//temporary fix for the true basic genetic algorithm! I will revisit this in the future to perform a proper naming convention
template<XESolution XES, XEvaluation XEv = Evaluation<>>
class SimpleElection {
protected:
	using Individual = S;
    //using Chromossome = R;
    using Fitness = Evaluation<>*; //nullptr means there's no evaluation
    using Population = vector< pair<Individual, Fitness> >;

public:
	SimpleElection() = default;
	virtual ~SimpleElection() = default;

	virtual vector<Individual*> elect(Population& population) = 0;

};

/**********************/
/* ELECTION EXAMPLES */
/**********************/

//Chooses two distinct parents randomly
template<XESolution XES, XEvaluation XEv = Evaluation<>>
class RandomElection final : public SimpleElection<S>{
protected:
	using Individual = S;
    //using Chromossome = R;
    using Fitness = Evaluation<>*; //nullptr means there's no evaluation
    using Population = vector< pair<Individual, Fitness> >;

public:
	RandomElection() = default;
	~RandomElection() = default;

	virtual vector<Individual*> elect(Population& population){
		_OPTFRAME_DBG_ELECTION_ std::cerr << "-OptDEbug- RadomElection will elect two random parents" << std::endl;
		
		vector<Individual*> chosen(2, nullptr);
		std::mt19937 mersenne_twister(std::chrono::steady_clock::now().time_since_epoch().count()); //todo use rand gen
		std::uniform_int_distribution<unsigned int> dist(0u, population.size());
		int first_pos =  dist(mersenne_twister);
		int second_pos =  dist(mersenne_twister);
		chosen[0] = &population[first_pos].first;
		chosen[1] = &population[second_pos].first;

		_OPTFRAME_DBG_ELECTION_ std::cerr << "-OptDEbug- Elected individual " << first_pos << " and " << second_pos << std::endl;

		return chosen;
	};

};

//Chooses two distinct parents with the roullete operation:  the most fit has higher chances of being chosen
//Assumes that the objective function is POSITIVE OR ZERO
//Assumes that the entire population has been evaluated
template<XESolution XES, XEvaluation XEv = Evaluation<>>
class RouletteElection final : public SimpleElection<S>{
protected:
	using Individual = S;
    //using Chromossome = R;
    using Fitness = Evaluation<>*; //nullptr means there's no evaluation
    using Population = vector< pair<Individual, Fitness> >;

public:
	RouletteElection() = default;
	~RouletteElection() = default;

	vector<Individual*> elect(Population& population){
		_OPTFRAME_DBG_ELECTION_ std::cerr << "-OptDEbug- RouletteElection will elect two random parents with the roulette operation" << std::endl;

		std::vector<Individual*> chosen(2, nullptr);
		int p1_pos, p2_pos;
		evtype sum = static_cast<evtype>(0), roll;  //todo: do it better
		std::mt19937 mersenne_twister(std::chrono::steady_clock::now().time_since_epoch().count()); //todo: use rand gen

		for(int i = 0; i < population.size(); ++i){
			assert(population[i].second);
			assert(population[i].second->getObjFunction() >= static_cast<evtype>(0));
			sum += population[i].second->getObjFunction();
		}

		std::uniform_real_distribution<evtype> dist(static_cast<evtype>(0), sum); //todo: do it better
		p1_pos = p2_pos = 0;
		roll = dist(mersenne_twister);
		while(roll > population[p1_pos].second->getObjFunction()){
			roll -= population[p1_pos].second->getObjFunction();
			++p1_pos;
		}

		do{
			roll = dist(mersenne_twister);
			p2_pos = 0;
			while(roll > population[p2_pos].second->getObjFunction()){
				roll -= population[p2_pos].second->getObjFunction();
				++p2_pos;
			}
		}while(p1_pos == p2_pos);

		_OPTFRAME_DBG_ELECTION_ std::cerr << "-OptDEbug- RouletteElection elected individual " << p1_pos << " and " << p2_pos << std::endl;

		chosen[0] = &population[p1_pos].first;
		chosen[1] = &population[p2_pos].first;

		return chosen;
	};

};


}



#endif