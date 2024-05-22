// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#include <PN/ProblemCommand.h>

using namespace PN;

bool PNProblemCommand::load(string filename, HeuristicFactory<ESolutionPN>& hf,
                            map<string, string>& dictionary,
                            map<string, vector<string>>& ldictionary) {
  // File* file = new File(filename);
  /*
  try {
     file = new File(filename);
  } catch (FileNotFound& f) {
     cout << "File '" << filename << "' not found" << endl;
     return false;
  }
  */

  // Scanner scanner(file);
  Scanner scanner{File(filename)};

  // STEP 1: IMPLEMENT ProblemInstance.h/.cpp (YOU MUST READ THE PROBLEM DATA
  // AND STORE INSIDE)
  p = new ProblemInstance(scanner);

  // STEP 2: IMPLEMENT Evaluator.h/.cpp (THIS IS YOUR EVALUATION FUNCTION)
  // this->registerComponent(sref<PNEvaluator>{ new PNEvaluator(*p) },
  // "myEvaluator", hf, dictionary);
  hf.addComponent(sref<PNEvaluator>{new PNEvaluator(*p)}, "OptFrame:Evaluator");

  // STEP 3: IMPLEMENT ConstructiveGreedyHalfSum.h/.cpp (THIS IS YOUR
  // CONSTRUCTIVE METHOD)
  // registerComponent(*new ConstructiveGreedySum(*p), "grdSum", hf,
  // dictionary);
  sref<Constructive<SolutionPN>> c1{new ConstructiveGreedySum(*p)};
  hf.addComponent(c1, "OptFrame:Constructive");

  // registerComponent(*new ConstructiveGreedyHalfSum(*p), "grdHalfSum", hf,
  // dictionary);
  sref<Constructive<SolutionPN>> c2{new ConstructiveGreedyHalfSum(*p)};
  hf.addComponent(c2, "OptFrame:Constructive");

  // registerComponent(*new ConstructiveRandomGreedy(*p), "rndGreedy", hf,
  // dictionary);
  sref<Constructive<SolutionPN>> c3{new ConstructiveRandomGreedy(*p)};
  hf.addComponent(c3, "OptFrame:Constructive");

  // registerComponent(*new ConstructiveRandom(*p), "rnd", hf, dictionary);
  sref<Constructive<SolutionPN>> c4{new ConstructiveRandom(*p)};
  hf.addComponent(c4, "OptFrame:Constructive");

  // STEP 3.1 (EXERCISE): DO THE SAME FOR OTHER ALTERNATIVE CONSTRUCTIVE METHODS
  // (REMEMBER TO ADD THE .cpp IN THE makefile)
  // ...

  // STEP 4: IMPLEMENT NSSeqShift.h/.cpp (THIS IS YOUR NEIGHBORHOOD STRUCTURE,
  // YOU HAVE TO IMPLEMENT THE CLASSES: Move, NSIterator and NSSeq)
  // registerComponent(*new NSSeqShift(*p, hf.getRandGen()), "nsseq_Shift", hf,
  // dictionary);
  sref<NSSeq<ESolutionPN>> n1{new NSSeqShift(*p, hf.getRandGen())};
  hf.addComponent(n1, "OptFrame:NS:NSFind:NSSeq");

  // registerComponent(*new NSSeqSwap(*p, hf.getRandGen()), "nsseq_Swap", hf,
  // dictionary);
  sref<NSSeq<ESolutionPN>> n2{new NSSeqSwap(*p, hf.getRandGen())};
  hf.addComponent(n2, "OptFrame:NS:NSFind:NSSeq");

  // STEP 4.1 (EXERCISE): DO THE SAME FOR OTHER ALTERNATIVE NEIGHBORHOOD
  // STRUCTURES (REMEMBER TO ADD THE .cpp IN THE makefile)
  // ...

  cout << "problem '" << filename << "' loaded successfully" << endl;

  return true;
}

bool PNProblemCommand::unload(HeuristicFactory<ESolutionPN>& factory,
                              map<string, string>& dictionary,
                              map<string, vector<string>>& ldictionary) {
  if (p) delete p;
  p = NULL;

  cout << "problem instance for PN unloaded successfully (use 'drop_all' if "
          "you want to remove all components)"
       << endl;

  return true;
}
