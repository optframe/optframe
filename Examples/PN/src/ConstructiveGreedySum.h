#ifndef PN_CONSTRUCTIVE_GreedySum_H_
#define PN_CONSTRUCTIVE_GreedySum_H_

#include <OptFrame/Constructive.hpp>
//#include "../../OptFrame/Util/TestSolution.hpp"

#include "ProblemInstance.h"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.h"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

using namespace optframe;

namespace PN {

class ConstructiveGreedySum : public Constructive<RepPN> //, MY_ADS>
{
private:
   ProblemInstance& pPN;

   // Your private vars

public:
   ConstructiveGreedySum(ProblemInstance& _pPN);

   virtual ~ConstructiveGreedySum();

   //Solution<RepPN, MY_ADS>* generateSolution(double timelimit);
   std::optional<RepPN> generateSolution(double timelimit) override;

   void print() const
   {
      cout << "ConstructiveGreedySum" << endl;
   }
};

}

#endif /*PN_CONTRUCTIVE_GreedySum_H_*/
