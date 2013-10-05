#ifndef $project_PROBLEMMODULE_H_
#define $project_PROBLEMMODULE_H_

#include <iostream>

#include "../../OptFrame/Commands/ProblemCommand.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "Evaluator.h"
#include "Constructive$constructive.h"
#include "NSSeq$neighborhood.h"

#include "ProblemInstance.h"

using namespace scannerpp;
using namespace optframe;

namespace $project
{

class $projectProblemCommand : public ProblemCommand<Rep$project , MY_ADS $commadproject>
{
public:

    ProblemInstance* p;
    
    $projectProblemCommand();

    virtual ~$projectProblemCommand();
    
    string id();
    
    bool registerComponent(Component& component, string type, string name, HeuristicFactory<Rep$project , MY_ADS $commadproject>& hf, map<string, string>& dictionary);
    
	bool load(string filename, HeuristicFactory<Rep$project , MY_ADS $commadproject>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary);
	    
    bool unload(HeuristicFactory<Rep$project , MY_ADS $commadproject>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary);
};

}

#endif /*$project_PROBLEMMODULE_H_*/

