#ifndef KP2_CROSSOVER_
#define KP2_CROSSOVER_

#include <vector>

#include <OptFrame/Heuristics/EvolutionaryAlgorithms/Crossover.hpp>

#include "ProblemInstance.h"
#include "Representation.h"

namespace KP2 {

class MyCrossover : public optframe::TwoParentsCrossover<RepKP2>
{
private:
   ProblemInstance& pKP2;

public:
   MyCrossover() = delete;
   MyCrossover(ProblemInstance& _pKP2);
   ~MyCrossover() = default;

   VPopulation cross(const Individual& parent1, const Individual& parent2) override;
};

}

#endif