#ifndef $project_PROBLEMMODULE_H_
#define $project_PROBLEMMODULE_H_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "Evaluator.h"
#include "Constructive$constructive.h"
#include "NSSeq$neighborhood.h"

#include "ProblemInstance.h"

#include "../../OptFrame/RandGen.hpp"
#include "../../OptFrame/Util/CheckCommand.hpp"

using namespace scannerpp;
using namespace optframe;

namespace $project
{

class $projectProblemCommand
{
public:

    CheckCommand<Rep$project, MY_ADS $commadproject>* check;
    RandGen& rg;
    vector<Component*> vd;
    
    void registerComponent(Component* c, string s)
    {
        vd.push_back(c);
    }

    ProblemInstance* p;
    
    $projectProblemCommand(RandGen& _rg) 
        : rg(_rg)
    { 
        p = NULL;
        check = NULL;
    }

    virtual ~$projectProblemCommand()
    {
    }
    
    string id()
    {
        return "problem.$project";
    }
    
    // Implement 'load' and 'unload' methods in .cpp file
    bool load(string filename);

    bool unload();
};

}

#endif /*$project_PROBLEMMODULE_H_*/

