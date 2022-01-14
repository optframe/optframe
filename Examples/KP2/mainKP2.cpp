#include <iomanip>
#include <iostream>
#include <limits>
#include <utility>

#include <OptFrame/Evaluation.hpp>
#include <OptFrame/Heuristics/EvolutionaryAlgorithms/Election.hpp>
#include <OptFrame/Heuristics/EvolutionaryAlgorithms/GeneticAlgorithm.hpp>
#include <OptFrame/Heuristics/EvolutionaryAlgorithms/Selection.hpp>

#include "./src/Crossover.h"
#include "./src/Evaluator.h"
#include "./src/InitialPopulation.hpp"
#include "./src/Mutation.h"
#include "./src/ProblemInstance.h"
#include "./src/Representation.h"

using namespace KP2;

#ifndef NDEBUG
#define MN_DEBUG 1 //uncoment this to debug this code
#endif

#ifdef MN_DEBUG
#define DEBUG
#else
#define DEBUG while (false)
#endif

int
main(int argc, char** argv)
{
   DEBUG std::cerr << "Starting KP2" << std::endl;

   ProblemInstance pi; //le a instancia
   int populationSize = pi.n * 4;
   double selectionRate = 0.5;
   double mutationRate = 0.15;
   int numGenerations = 100;

   DEBUG
   {
      std::cerr << "Parameters that will be used for GA: " << std::endl;
      std::cerr << "Size: " << populationSize << " alpha: " << selectionRate << " omega: " << mutationRate << " num of gen: " << numGenerations << std::endl;
      std::cerr << "Constructing the optframe components that will be used." << std::endl;
   }

   MyEvaluator ev(pi); //inicializa o avaliador
   MyInitialPopulation ip(populationSize, pi);
   optframe::ElitismSelection<RepKP2> sl(ev, selectionRate);
   optframe::RouletteElection<RepKP2> el;
   MyCrossover cr(pi);
   MyMutation mt(mutationRate, pi);
   optframe::GeneticAlgorithm ga(ev, ip, sl, el, cr, &mt, populationSize, numGenerations);

   DEBUG std::cerr << "Starting Optframe GA main execution" << std::endl;

   std::pair<optframe::Solution<RepKP2>, optframe::Evaluation<>> best = ga.exec();

   DEBUG
   {
      std::cerr << "Optframe GA has finished with the following population: " << std::endl;
      int i = 0;
      for (auto& x : ga.lastExecutionPopulation) {
         std::cerr << i++ << "-th Chromossome: " << x.first.getR() << " fitness: ";
         if (x.second)
            std::cerr << x.second->getObjFunction();
         else
            std::cerr << "nullptr";
         std::cerr << std::endl;
      }
      std::cerr << endl;
   }

   std::cout << "Best solution: " << best.first.getR() << std::endl;
   std::cout << "Evaluation: " << best.second.getObjFunction() << std::endl;
   std::cout.setf(ios::fixed);
   std::cout << "Total Execution Time: " << ga.lastExecutionTime << " (s) " << std::endl;

   DEBUG std::cerr << "Program ended successfully" << std::endl;

   return 0;
}
