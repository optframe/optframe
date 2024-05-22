#include "MODM/ProblemCommand.h"

using namespace MODM;

bool MODMProblemCommand::load(string filename) {
  File file(filename);

  if (!file.isOpen()) {
    cout << "File '" << filename << "' not found" << endl;
    return false;
  }

  Scanner scanner(std::move(file));

  // STEP 1: IMPLEMENT ProblemInstance.h/.cpp (YOU MUST READ THE PROBLEM DATA
  // AND STORE INSIDE)
  p = new ProblemInstance(scanner);
  MODMADSManager adsMan(*p);

  // TODO: fix KahanSummation double defined method... then uncomment here

  /*

       // OPTIONAL: FOR TESTING
       check = new CheckCommand<SolutionMODM>;

       // STEP 2: IMPLEMENT Evaluator.h/.cpp (THIS IS YOUR EVALUATION FUNCTION)
       registerComponent(new MODMEvaluator(*p, adsMan), "myEvaluator");

       // STEP 3: IMPLEMENT ConstructiveBasicGreedyRandomized.h/.cpp (THIS IS
     YOUR CONSTRUCTIVE METHOD)

       registerComponent(new ConstructiveBasicGreedyRandomized(*p, rg, adsMan),
     "constructive_BasicGreedyRandomized");

       // STEP 3.1 (EXERCISE): DO THE SAME FOR OTHER ALTERNATIVE CONSTRUCTIVE
     METHODS (REMEMBER TO ADD THE .cpp IN THE makefile)
       // ...

       // STEP 4: IMPLEMENT NSSeqSWAPInter.h/.cpp (THIS IS YOUR NEIGHBORHOOD
     STRUCTURE, YOU HAVE TO IMPLEMENT THE CLASSES: Move, NSIterator and NSSeq)

       registerComponent(new NSSeqSWAPInter(rg, p), "nsseq_SWAPInter");
       registerComponent(new NSSeqSWAP(rg, p), "nsseq_SWAP");

       // STEP 4.1 (EXERCISE): DO THE SAME FOR OTHER ALTERNATIVE NEIGHBORHOOD
     STRUCTURES (REMEMBER TO ADD THE .cpp IN THE makefile)
       // ...
  */

  cout << "problem '" << filename << "' loaded successfully" << endl;

  return true;
}

bool MODMProblemCommand::unload() {
  if (p) delete p;
  p = NULL;

  cout << "problem instance for MODM unloaded successfully (use 'drop_all' if "
          "you want to remove all components)"
       << endl;

  return true;
}
