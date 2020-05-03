#ifndef KP2_INITIAL_POPULATION_H_
#define KP2_INITIAL_POPULATION_H_

#include "../../OptFrame/InitialPopulation.h"

#include "./ProblemInstance.h"
#include "./Representation.h"

namespace KP2
{

class MyInitialPopulation : public optframe::RandomInitialPopulation<RepKP2>{
private:
	ProblemInstance& pKP2;

public:
	MyInitialPopulation() = delete;
	MyInitialPopulation(unsigned int initialPopSize, ProblemInstance& _pKP2);

	Individual generateIndividual() override;
};

}


#endif