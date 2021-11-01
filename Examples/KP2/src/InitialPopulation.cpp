#include <chrono>
#include <random>

#include "InitialPopulation.h"

using namespace KP2;

#ifndef NDEBUG
#define IP_DEBUG 1 //uncoment this if you want to debug this code
#endif

#ifdef IP_DEBUG
#define DEBUG
#else
#define DEBUG while (false)
#endif

MyInitialPopulation::MyInitialPopulation(unsigned int initialPopSize, ProblemInstance& _pKP2)
  : RandomInitialPopulation(initialPopSize)
  , pKP2(_pKP2)
{
}

MyInitialPopulation::Individual
MyInitialPopulation::generateIndividual()
{
   DEBUG std::cerr << "Generating individual with MyInitialPopulation: ";

   std::uniform_int_distribution dist(0, 1); //true or false

   Individual dna(pKP2.n, false);
   for (int i = 0; i < pKP2.n; ++i)
      dna[i] = static_cast<bool>(dist(pKP2.mersenne_twister));

   DEBUG std::cerr << dna << std::endl;

   return dna;
}
