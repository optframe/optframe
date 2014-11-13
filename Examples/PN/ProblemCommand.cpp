#include "ProblemCommand.h"

using namespace PN;

bool PNProblemCommand::load(string filename, HeuristicFactory<RepPN , MY_ADS >& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
{
    File* file;

    try
    {
        file = new File(filename);
    }
    catch (FileNotFound& f)
    {
        cout << "File '" << filename <<"' not found" << endl;
        return false;
    }

    Scanner scanner(file);

    // STEP 1: IMPLEMENT ProblemInstance.h/.cpp (YOU MUST READ THE PROBLEM DATA AND STORE INSIDE)
    p = new ProblemInstance(scanner);

    // STEP 2: IMPLEMENT Evaluator.h/.cpp (THIS IS YOUR EVALUATION FUNCTION)
    registerComponent(*new PNEvaluator(*p), "myEvaluator", hf, dictionary);

    // STEP 3: IMPLEMENT ConstructiveGreedyHalfSum.h/.cpp (THIS IS YOUR CONSTRUCTIVE METHOD)
    registerComponent(*new ConstructiveGreedySum(*p), "grdSum", hf, dictionary);
    registerComponent(*new ConstructiveGreedyHalfSum(*p), "grdHalfSum", hf, dictionary);
    registerComponent(*new ConstructiveRandomGreedy(*p), "rndGreedy", hf, dictionary);
    registerComponent(*new ConstructiveRandom(*p), "rnd", hf, dictionary);

    // STEP 3.1 (EXERCISE): DO THE SAME FOR OTHER ALTERNATIVE CONSTRUCTIVE METHODS (REMEMBER TO ADD THE .cpp IN THE makefile)
    // ...
    
    // STEP 4: IMPLEMENT NSSeqShift.h/.cpp (THIS IS YOUR NEIGHBORHOOD STRUCTURE, YOU HAVE TO IMPLEMENT THE CLASSES: Move, NSIterator and NSSeq)
    registerComponent(*new NSSeqShift(*p, hf.getRandGen()), "nsseq_Shift", hf, dictionary);
    registerComponent(*new NSSeqSwap(*p, hf.getRandGen()), "nsseq_Swap", hf, dictionary);

    // STEP 4.1 (EXERCISE): DO THE SAME FOR OTHER ALTERNATIVE NEIGHBORHOOD STRUCTURES (REMEMBER TO ADD THE .cpp IN THE makefile)
    // ...

    cout << "problem '" << filename << "' loaded successfully" << endl;
        
    return true;
}
    
bool PNProblemCommand::unload(HeuristicFactory<RepPN , MY_ADS >& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
{
    if(p)
        delete p;
    p = NULL;
       
    cout << "problem instance for PN unloaded successfully (use 'drop_all' if you want to remove all components)" << endl;
       
    return true;
}
