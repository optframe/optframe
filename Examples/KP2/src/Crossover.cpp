#include <utility>

#include "./Crossover.h"

using namespace KP2;

#ifndef NDEBUG
#define CR_DEBUG 1 //leave this to debug this code
#endif

#ifdef CR_DEBUG
#define DEBUG
#else
#define DEBUG while (false)
#endif

MyCrossover::MyCrossover(ProblemInstance& _pKP2)
  : pKP2(_pKP2)
{
}

MyCrossover::VPopulation
MyCrossover::cross(const Individual& parent1, const Individual& parent2)
{
   DEBUG
   {
      std::cerr << "Crossing two parents with MyCrossover cross: ";
      std::cerr << parent1 << " / " << parent2;
   }

   const Individual& parent1_dna = parent1;
   const Individual& parent2_dna = parent2;
   Individual offspring1_dna = vector<bool>(pKP2.n, false);
   Individual offspring2_dna = vector<bool>(pKP2.n, false);

   int half_point = pKP2.n / 2;
   for (int i = 0; i < pKP2.n; ++i) {
      if (i < half_point) {
         offspring1_dna[i] = parent1_dna[i];
         offspring2_dna[i] = parent2_dna[i];
      } else {
         offspring1_dna[i] = parent2_dna[i];
         offspring2_dna[i] = parent1_dna[i];
      }
   }

   DEBUG std::cerr << " offspring: " << offspring1_dna << " / " << offspring2_dna << std::endl;

   //VPopulation offspring = { { Individual(offspring1_dna, nullptr), nullptr }, { Individual(offspring2_dna, nullptr), nullptr } };
   VPopulation offspring;
   offspring.push_back(std::pair<Individual, Fitness>{ offspring1_dna, nullptr });
   offspring.push_back(std::pair<Individual, Fitness>{ offspring2_dna, nullptr });
   return offspring;
}
