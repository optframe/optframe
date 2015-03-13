#ifndef SCP_PROBLEMMODULE_H_
#define SCP_PROBLEMMODULE_H_

#include <iostream>

#include "../../Examples/SCP/ConstructiveConstructiveTest.h"
#include "../../Examples/SCP/DeltaStructure.h"
#include "../../Examples/SCP/Evaluation.h"
#include "../../Examples/SCP/Evaluator.h"
#include "../../Examples/SCP/NSSeqNSTest.h"
#include "../../Examples/SCP/ProblemInstance.h"
#include "../../Examples/SCP/Representation.h"
#include "../../Examples/SCP/Solution.h"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "../../OptFrame/RandGen.hpp"
#include "../../OptFrame/Util/CheckCommand.hpp"

using namespace scannerpp;
using namespace optframe;

namespace SCP
{

class SCPProblemCommand
{
public:

    CheckCommand<RepSCP, MY_ADS >* check;
    RandGen& rg;
    vector<Component*> vd;
    
    void registerComponent(Component* c, string s)
    {
        vd.push_back(c);
    }

    ProblemInstance* p;
    
    SCPProblemCommand(RandGen& _rg) 
        : rg(_rg)
    { 
        p = NULL;
        check = NULL;
    }

    virtual ~SCPProblemCommand()
    {
    }
    
    string id()
    {
        return "problem.SCP";
    }
    
    // Implement 'load' and 'unload' methods in .cpp file
    bool load(string filename);

    bool unload();
};

}

#endif /*SCP_PROBLEMMODULE_H_*/

