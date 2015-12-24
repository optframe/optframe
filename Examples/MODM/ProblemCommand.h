#ifndef MODM_PROBLEMMODULE_H_
#define MODM_PROBLEMMODULE_H_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "Evaluator.h"
#include "ConstructiveBasicGreedyRandomized.h"
#include "NSSeqSWAPInter.h"
#include "NSSeqSWAP.h"

#include "ProblemInstance.h"

#include "../../OptFrame/RandGen.hpp"
#include "../../OptFrame/Util/CheckCommand.hpp"

using namespace scannerpp;
using namespace optframe;

namespace MODM
{

class MODMProblemCommand
{
public:

    CheckCommand<RepMODM, AdsMODM >* check;
    RandGen& rg;
    vector<Component*> vd;
    
    void registerComponent(Component* c, string s)
    {
        vd.push_back(c);
    }

    ProblemInstance* p;
    
    MODMProblemCommand(RandGen& _rg) 
        : rg(_rg)
    { 
        p = NULL;
        check = NULL;
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

