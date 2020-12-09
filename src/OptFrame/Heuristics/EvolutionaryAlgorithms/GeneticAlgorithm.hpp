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

#ifndef OPTFRAME_GENETICALGORITHM_HPP_
#define OPTFRAME_GENETICALGORITHM_HPP_

#include <iostream>
#include <algorithm>
#include <cassert>
#include <chrono>
#include <limits>
#include <utility>

#include "../../SingleObjSearch.hpp"
#include "../../InitialPopulation.h"
#include "Crossover.hpp"
#include "Mutation.hpp"
#include "Selection.hpp"
#include "Election.hpp"

#ifndef _OPTFRAME_DBG_GA_
#   ifdef OPTFRAME_DEBUG
#       define _OPTFRAME_DBG_GA_ 
#   else
#       define _OPTFRAME_DBG_GA_ while(false)
#   endif /* OPTFRAME_DEBUG */
#endif /* _OPTFRAME_DBG_GA_ */

namespace optframe
{

template<XESolution XES, XEvaluation XEv = Evaluation<>>
class GeneticAlgorithm {
protected:
    using Individual = S;
    //using Chromossome = R;
    using Fitness = XEv*; //nullptr means there's no evaluation
    using VPopulation = std::vector< pair<Individual, Fitness> >;

    Evaluator<S>& evaluator; //standard problem evaluator
    SimpleInitialPopulation<XES>& initPop; //operator that generates the initial population
                                        //It is expected that the initial population will have at least 2 individuals.
                                        //It is expected that the initial population size wont surpass the population max size
    SimpleSelection<S, XEv>& selection; //operator that selects a portion (or entirety) of the current population for the current generation
                                            //It is expected that the size of the selected popluation will be at least 2.
                                            //It is expected that the selected population size wont surpass the population max size
                                            //It is expected that the size of the population will be equal or lower
                                            //Though most applications will have a mindset that the selected population will have 'selectionRate*populationMaxSize' individuals, this rule isn't enforced. 
    SimpleElection<S, XEv>& election; //operator that will return a population to serve as parents for crossover
                                    //It is expected at least 2 parents
    SimpleCrossover<S, XEv>& cross; //operator that will use a population as parents for offspring generation
                                  //It is expected that at least 1 offspring will be generated
                                  //if too many offspring is returned due to max population size, then extra ones will be discarded
    SimpleMutation<S, XEv>* mut; //optional operator that acts over a population to change it unpredicatelly

    unsigned populationMaxSize; //parameter that needs to b calibrated, this is used a various asserts
                                //It is expected to be at least 2
    unsigned numGenerations; //parameter that dictates the standard stopping criteria for GA
                             //if this equals to zero, then the best solution will be the one found in the initial population
    double timelimit_ms; //parameter that dictates the standard stopping criteria for GA
                      //if this is too low, then the best solution will be the one found in the initial population
    //Evaluation<> target; //parameter that dictates the standard stopping criteria for GA TODO

public:
    //optional parameters
    bool mutateSelectedPop = true; //leaving this as true will allow the entire population to be mutated.
                                   //otherwise, selected population will be preserved and only offspring will be mutated
    bool sortPopulationAfterEvaluation = true; //leaving this as true will allow the genetic algorithm to sort the population to rank each individual when evaluating them
    Population lastExecutionPopulation; //last execution full population, for analytical puporses
    double lastExecutionTime = 0.0; //total execution time from last run in seconds

protected:
    virtual void clearLastExecutionPopulation(){
        for(auto& x : lastExecutionPopulation)
            if(x.second)
                delete x.second;
        lastExecutionPopulation.clear();
    }

public:


    GeneticAlgorithm(Evaluator<S>& _evaluator,
                     SimpleInitialPopulation<XES>& _initPop,
                     SimpleSelection<S, XEv>& _selection,
                     SimpleElection<S, XEv>&  _election,
                     SimpleCrossover<S, XEv>& _cross,
                     SimpleMutation<S, XEv>* _mut = nullptr,
                     unsigned populationMaxSize = 30, unsigned numGenerations = 10, double timelimit_ms = numeric_limits<double>::max()) //, Evaluation<DS> target)
    : evaluator(_evaluator), initPop(_initPop), selection(_selection), election(_election), cross(_cross) {
    
        _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Genetic Algorithm constructor started" << std::endl;

        this->mut = _mut;
        this->populationMaxSize = populationMaxSize;
        this->numGenerations = numGenerations;  
        this->timelimit_ms = timelimit_ms;  

        _OPTFRAME_DBG_GA_{
            std::cerr << "-OptDebug- Genetic Algorithm was initialized with the following parameters: " << std::endl;
            std::cerr << "-OptDebug- Max pop size: " << populationMaxSize << std::endl;
            std::cerr << "-OptDebug- Number of generations: " << numGenerations << std::endl;
            std::cerr << "-OptDebug- timelimit: " << timelimit_ms << std::endl;
        }
   }

   //evaluates fitness and return the position of the best
    virtual int evaluateFitness(Population& p) const {
        int most_fit_pos = 0;
        evtype sum = static_cast<evtype>(0);
        if(!p[0].second) {
            _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- calculating fitness of individual 0 " << std::endl;
            p[0].second = new Evaluation(evaluator.evaluate(p[0].first.getR(), p[0].first.getADSptr())); //todo: make this better!!!
        }
        _OPTFRAME_DBG_GA_{
            std::cerr << "-OptDebug- Individual 0 fitness: " << p[0].second->getObjFunction() << std::endl;
            sum += static_cast<evtype>(p[0].second->getObjFunction());  
        } 

        for (int i = 1; i < p.size(); ++i){
            if(!p[i].second){
                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- calculating fitness of individual " << i << std::endl;
                p[i].second = new Evaluation(evaluator.evaluate(p[i].first.getR(), p[i].first.getADSptr())); //todo: make this better!!!
            }
            _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Individual " << i << " fitness: " << p[i].second->getObjFunction() << std::endl;
            if(evaluator.betterThan(*p[i].second, *p[most_fit_pos].second)){
                most_fit_pos = i;
                 _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- New best individual" << std::endl;
            }

            _OPTFRAME_DBG_GA_ sum += static_cast<evtype>(p[i].second->getObjFunction());;
        }

        _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Average fitness of population: " << sum/p.size() << std::endl;
    }

    virtual void sortPopulation(Population& population){
        auto compare = [&](const pair<Individual, Fitness>& a, const pair<Individual, Fitness>& b)->bool{
            if(a.second && b.second)
                return evaluator.betterThan(*a.second, *b.second);
            else return a.second != nullptr;
        };

        std::sort(population.begin(), population.end(), compare);
    }

    virtual pair<Solution<S, XEv>, Evaluation<>> exec(){
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();


        _OPTFRAME_DBG_GA_{
            std::cerr << "-OptDebug- GA exec for " << numGenerations << " generations with time limited by " << timelimit_ms << " seconds." << std::endl;
            std::cerr << "-OptDebug- Generaring init pop" << std::endl;
        }

        Population currentPopulation = initPop.generate();
        assert(currentPopulation.size() > 1);
        assert(currentPopulation.size() <= populationMaxSize);

        _OPTFRAME_DBG_GA_{
            std::cerr << "-OptDebug- Initial Population successfully generated." << std::endl;
            std::cerr << "-OptDebug- Calculating the fitness of each chromossome from Initial Population." << std::endl;
        }

        int best_individual_pos = evaluateFitness(currentPopulation);

        _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Fitness calculated." << std::endl;

        if(sortPopulationAfterEvaluation){
            _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Ranking each individual based on fitness rating." << std::endl;
            sortPopulation(currentPopulation);
            _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Current population sorted." << std::endl;
            best_individual_pos = 0;
        }

        Individual best_individual(currentPopulation[best_individual_pos].first); //copy the best individual, as operators can lose it
        Evaluation<> best_individual_eval(*currentPopulation[best_individual_pos].second); //copy the best eval, as operators can lose it

        _OPTFRAME_DBG_GA_{
            std::cerr << "-OptDebug- Best Individual fitness: " << best_individual_eval.getObjFunction() << std::endl;
            std::cerr << "-OptDebug- Starting generations " << std::endl;
        }

        unsigned g = 1;
        double time_passed = (std::chrono::steady_clock::now() - start).count();
        while (g++ < numGenerations && time_passed < timelimit_ms){

            _OPTFRAME_DBG_GA_{
                std::cerr << std::endl << "-OptDebug- Starting generation: " << g-1 << std::endl;
                std::cerr << std::endl << "-OptDebug- Applying selection operator" << std::endl;
            }

            int size_before_selection = currentPopulation.size();
            selection.select(currentPopulation);
            int size_after_selection = currentPopulation.size();
            assert(currentPopulation.size() > 2);
            assert(currentPopulation.size() <= populationMaxSize);
            assert(size_before_selection >= size_after_selection);

            _OPTFRAME_DBG_GA_{
                std::cerr << "-OptDebug- Size of selected population: " << currentPopulation.size() << std::endl;
                std::cerr << "-OptDebug- Filling the remainder with offspring" << std::endl;
            }

            Population offspring;
            int to_fill = populationMaxSize - currentPopulation.size();
            while(offspring.size() < to_fill){
                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Electing new parents" << std::endl;

                std::vector<Individual*> electedParents = election.elect(currentPopulation);

                _OPTFRAME_DBG_GA_{
                    std::cerr << electedParents.size() << " parents elected." << std::endl;                
                    std::cerr << "-OptDebug- Performing crossover" << std::endl;
                }

                Population newOffspring = cross.cross(electedParents);

                _OPTFRAME_DBG_GA_{
                    std::cerr << newOffspring.size() << " offsprings generated." << std::endl;   
                    std::cerr << "-OptDebug- Storing them" << std::endl;
                }

                assert(newOffspring.size() > 0);
                int to_keep = to_fill - offspring.size();
                std::move(std::begin(newOffspring), (to_keep >= newOffspring.size()) ? std::end(newOffspring) : std::begin(newOffspring) + to_keep, std::back_inserter(offspring));
                newOffspring.clear();
            }   

            _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Crossover terminated with " << offspring.size() << " offsprings." << std::endl;

            if(mutateSelectedPop){ //I will mutate the entire population
                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Mutate selected pop is on. Moving all offspring to current population" << std::endl;

                std::move(std::begin(offspring), std::end(offspring), std::back_inserter(currentPopulation));
                offspring.clear();

                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Calling the mutation operator on current population" << std::endl;

                assert(mut != nullptr); //why would you change 'mutateSelectedPop' option if no mutation is required?
                if(mut) mut->mutate(currentPopulation);
                assert(currentPopulation.size() > 2);
                assert(currentPopulation.size() <= populationMaxSize);
            }
            else{ //only mutate the new offspring
                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Mutate selected pop is off. Calling mutation before moving offspring to current population." << std::endl;
                
                if(mut) mut->mutate(offspring);
                assert(currentPopulation.size() > 2);
                assert(currentPopulation.size() <= populationMaxSize);

                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Moving offspring to current population" << std::endl;

                std::move(std::begin(offspring), std::end(offspring), std::back_inserter(currentPopulation));
                offspring.clear();
            }

            _OPTFRAME_DBG_GA_{
                std::cerr << "-OptDebug- Current population size: " << currentPopulation.size() << std::endl;
                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Evaluating population" << std::endl;
            }


            int best_of_generation_pos = evaluateFitness(currentPopulation);

            _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Fitness calculated." << std::endl;

            if(sortPopulationAfterEvaluation){
                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Ranking each individual based on fitness rating." << std::endl;
                sortPopulation(currentPopulation);
                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Current generation ranked." << std::endl;
                best_of_generation_pos = 0;
            }

            //Won't keep the best, but it will be stored in lastExecutionPopulation
            _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Checking time limit condition" << std::endl;
            time_passed = (std::chrono::steady_clock::now() - start).count();
            if(time_passed > timelimit_ms){
                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Leaving due to timelimit_ms" << std::endl;
                break;
            }

            _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Checking most fit" << std::endl;

            if(evaluator.betterThan(*currentPopulation[best_of_generation_pos].second, best_individual_eval)){
                best_individual = currentPopulation[best_of_generation_pos].first;
                best_individual_eval = *currentPopulation[best_of_generation_pos].second;
                _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- New best individual: " << best_individual_eval.getObjFunction() << std::endl;
            } //keeping best

            _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Generation " << g-1 << " finished with most fit individual being " << best_individual_eval.getObjFunction() << " and the most fit of the generation is " << currentPopulation[best_of_generation_pos].second->getObjFunction() << std::endl;
        }

        _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Stopping criterium achieved. Keeping time passed and population" << std::endl;

        std::chrono::duration<double> duration = std::chrono::steady_clock::now() - start;
        lastExecutionTime = duration.count();
        clearLastExecutionPopulation();
        std::move(std::begin(currentPopulation), std::end(currentPopulation), std::back_inserter(lastExecutionPopulation));
        currentPopulation.clear();    

        _OPTFRAME_DBG_GA_ std::cerr << "-OptDebug- Best evaluation found: " << best_individual_eval.getObjFunction() << std::endl;

        return {best_individual, best_individual_eval};
    }

    virtual ~GeneticAlgorithm(){
        clearLastExecutionPopulation();
    }
};

}

#endif /*OPTFRAME_GENETICALGORITHM_HPP_*/