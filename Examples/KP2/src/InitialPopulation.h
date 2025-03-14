#ifndef KP2_INITIAL_POPULATION_H_
#define KP2_INITIAL_POPULATION_H_

#include <OptFrame/Search/InitialPopulation.hpp>

#include "./ProblemInstance.h"
#include "./Representation.h"

namespace KP2 {

class MyInitialPopulation : public optframe::RandomInitialPopulation<RepKP2> {
 private:
  ProblemInstance& pKP2;

  using Individual = RepKP2;

 public:
  MyInitialPopulation() = delete;
  MyInitialPopulation(unsigned int initialPopSize, ProblemInstance& _pKP2);

  Individual generateIndividual() override;
};

}  // namespace KP2

#endif