#ifndef KP2_MUTATION_H_
#define KP2_MUTATION_H_

#include "../../OptFrame/Heuristics/EvolutionaryAlgorithms/Mutation.hpp"

#include "./ProblemInstance.h"
#include "./Representation.h"

namespace KP2
{

class MyMutation : public optframe::DefaultMutation<RepKP2> {
private:
	ProblemInstance& pKP2;

public:	
	MyMutation() = delete;
	MyMutation(double mutationRate, ProblemInstance& _pKP2);
	~MyMutation() = default;

	void mutate(Individual&) override;
};

}

#endif