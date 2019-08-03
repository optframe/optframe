#ifndef KP2_GENETIC_ALGORITHM_H_
#define KP2_GENETIC_ALGORITHM_H_

#include <vector>
#include <iostream>
#include <random>
#include <cmath>
#include <algorithm>
#include "ProblemInstance.h"
#include "Solution.h"
#include "Evaluator.h"

#ifdef DEBUG
#define GA_DEBUG 1
#endif

namespace KP2
{

class Individual{
public:
	std::vector<bool> chromosome;
	int fitness = 0;
	bool evaluated = false;
	#ifdef PRINT
	int weight = 0;
	#endif

	Individual() = default;
};

class Population{
public:
	std::vector<Individual> individuals;

	Population() = default;
};

class GeneticAlgorithm{
public:
	ProblemInstance& inst;
	Evaluator& eval;
	int seed;
	std::mt19937 mt;
	#ifdef PRINT
	std::vector<int> next_gen;
	#endif
	
	GeneticAlgorithm(ProblemInstance& inst, Evaluator& _eval, int seed = 0);

	Population initial_pop(int max_pop);
	void evaluate(Population&);
	Population select(Population&, double alpha);
	void reproduct(Population&, int max_pop);
	void mutate(Population&, double omega);
	Solution solver(int max_gen = 50, int max_pop = 20, double alpha = 0.5, double omega = 0.15);	
};

}

std::ostream& operator<<(std::ostream& os, const KP2::Individual& ind);
std::ostream& operator<<(std::ostream& os, const KP2::Population& pop);

#endif