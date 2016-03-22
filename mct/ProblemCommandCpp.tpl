#include "ProblemCommand.h"

using namespace $project;

bool $projectProblemCommand::load(string filename)
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

    // OPTIONAL: FOR TESTING
    check = new CheckCommand<Rep$project, MY_ADS $commadproject>;

    // STEP 2: IMPLEMENT Evaluator.h/.cpp (THIS IS YOUR EVALUATION FUNCTION)
    registerComponent(new $projectEvaluator(*p), "myEvaluator");

    // STEP 3: IMPLEMENT Constructive$constructive.h/.cpp (THIS IS YOUR CONSTRUCTIVE METHOD)
    registerComponent(new Constructive$constructive(*p), "constructive_$constructive");
    
    // STEP 3.1 (EXERCISE): DO THE SAME FOR OTHER ALTERNATIVE CONSTRUCTIVE METHODS (REMEMBER TO ADD THE .cpp IN THE makefile)
    // ...
    
    // STEP 4: IMPLEMENT NSSeq$neighborhood.h/.cpp (THIS IS YOUR NEIGHBORHOOD STRUCTURE, YOU HAVE TO IMPLEMENT THE CLASSES: Move, NSIterator and NSSeq)
    registerComponent(new NSSeq$neighborhood(*p, rg), "nsseq_$neighborhood");

    // STEP 4.1 (EXERCISE): DO THE SAME FOR OTHER ALTERNATIVE NEIGHBORHOOD STRUCTURES (REMEMBER TO ADD THE .cpp IN THE makefile)
    // ...

    cout << "problem '" << filename << "' loaded successfully" << endl;
        
    return true;
}

bool $projectProblemCommand::unload()
{
    if(p)
        delete p;
    p = NULL;
       
    cout << "problem instance for $project unloaded successfully (use 'drop_all' if you want to remove all components)" << endl;
       
    return true;
}
