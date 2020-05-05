#ifndef PN_PROBLEMMODULE_H_
#define PN_PROBLEMMODULE_H_

#include <iostream>

#include "../../OptFrame/Commands/ProblemCommand.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "Evaluator.h"
#include "ConstructiveGreedyHalfSum.h"
#include "ConstructiveGreedySum.h"
#include "ConstructiveRandomGreedy.h"
#include "ConstructiveRandom.h"
#include "NSSeqShift.h"
#include "NSSeqSwap.h"

#include "ProblemInstance.h"

using namespace scannerpp;
using namespace optframe;

namespace PN
{

class PNProblemCommand : public ProblemCommand<RepPN , MY_ADS >
{
public:

    ProblemInstance* p;
    
    PNProblemCommand()
    {
        p = NULL;
    }

    virtual ~PNProblemCommand()
    {
    }
    
    string id()
    {
        return "problem.PN";
    }
    
    // Implement 'load' and 'unload' methods in .cpp file
    
	bool load(string filename, HeuristicFactory<RepPN , MY_ADS >& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary);
	    
    bool unload(HeuristicFactory<RepPN , MY_ADS >& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary);
};

}

#endif /*PN_PROBLEMMODULE_H_*/

