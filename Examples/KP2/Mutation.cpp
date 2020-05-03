#include <random>
#include <chrono>

#include "./Mutation.h"

using namespace KP2;

#ifndef NDEBUG
#define MT_DEBUG 1 //comment this if you don't want to cdebug this code
#endif

#ifdef MT_DEBUG
#define DEBUG
#else
#define DEBUG while(false)
#endif

MyMutation::MyMutation(double mutationRate, ProblemInstance& _pKP2) : DefaultMutation(mutationRate), pKP2(_pKP2) {
}

void MyMutation::mutate(Chromossome& dna){
	DEBUG std::cerr << "Mutated " << dna << " to ";

	std::uniform_int_distribution dist(0, pKP2.n-1);
	int mutation_pos = dist(pKP2.mersenne_twister);
	dna[mutation_pos] = !dna[mutation_pos];

	DEBUG std::cerr << dna << std::endl;
}