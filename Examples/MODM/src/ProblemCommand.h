#ifndef MODM_PROBLEMMODULE_H_
#define MODM_PROBLEMMODULE_H_

#include <iostream>

#include <OptFrame/Scanner++/Scanner.hpp>

#include "DeltaStructure.h"
#include "Evaluation.h"
#include "Representation.h"
#include "Solution.h"

#include "ConstructiveBasicGreedyRandomized.h"
#include "Evaluator.h"
#include "NSSeqSWAP.h"
#include "NSSeqSWAPInter.h"

#include "ProblemInstance.h"

#include <OptFrame/RandGen.hpp>
//#include "../../OptFrame/Util/CheckCommand.hpp"

// Problem adding KahanSummation twice on binary...
// it shouln't be on pure .hpp... or require a class at least (with static method, better)

using namespace scannerpp;
using namespace optframe;

namespace MODM {

class MODMProblemCommand
{
public:
   //CheckCommand<RepMODM, AdsMODM >* check;
   sref<RandGen> rg;
   vsref<Component> vd;

   void registerComponent(sref<Component> c, string s)
   {
      vd.push_back(c);
   }

   ProblemInstance* p;

   MODMProblemCommand(sref<RandGen> _rg)
     : rg(_rg)
   {
      p = NULL;
      //check = NULL;
   }

   virtual ~MODMProblemCommand()
   {
   }

   string id()
   {
      return "problem.MODM";
   }

   // Implement 'load' and 'unload' methods in .cpp file
   bool load(string filename);

   bool unload();
};

}

#endif /*MODM_PROBLEMMODULE_H_*/
