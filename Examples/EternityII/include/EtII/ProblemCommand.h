#ifndef ETII_PROBLEMMODULE_HPP_
#define ETII_PROBLEMMODULE_HPP_

#include <OptFrame/Hyper/HeuristicFactory.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <iostream>

#include "DeltaMoveRotate.h"
#include "DeltaMoveSwapCenter.h"
#include "DeltaMoveSwapCorner.h"
#include "DeltaMoveSwapSide.h"
#include "Evaluation.h"
#include "Evaluator.h"
#include "InitialSolutionGreedy.h"
#include "NSSeqRotate.h"
#include "NSSeqSwapCenter.h"
#include "NSSeqSwapCorner.h"
#include "NSSeqSwapRotateCenter.h"
#include "NSSeqSwapSide.h"
#include "ProblemInstance.h"
#include "Representation.h"
#include "Solution.h"

using namespace scannerpp;
using namespace optframe;

namespace EtII {

class EtIIProblemCommand {
 public:
  ProblemInstance* p;

  EtIIProblemCommand() { p = NULL; }

  virtual ~EtIIProblemCommand() {
    if (p) delete p;
  }

  string id() { return "problem.EtII"; }

  bool registerComponent(sref<Component> component, string type, string name,
                         HeuristicFactory<ESolutionEtII>& hf,
                         map<std::string, std::string>& dictionary) {
    int idx = hf.addComponent(component, type);
    std::stringstream ss;
    ss << type << " " << idx;
    return true;  // defineText(name, ss.str(), dictionary);
  }

  bool load(string filename, HeuristicFactory<ESolutionEtII>& hf,
            map<std::string, std::string>& dictionary,
            map<string, vector<string>>& ldictionary) {
    File file(filename);

    if (!file.isOpen()) {
      std::cout << "File '" << filename << "' not found" << std::endl;
      return false;
    }

    Scanner scanner(std::move(file));

    p = new ProblemInstance(scanner);

    // add everything to the HeuristicFactory 'hf'

    // EtIIInitialSolutionGreedy& is = *new EtIIInitialSolutionGreedy(*p,
    // hf.getRandGen());
    sref<Constructive<SolutionEtII>> is(
        new EtIIInitialSolutionGreedy(*p, hf.getRandGen()));

    SolutionEtII s = *is->generateSolution(10);  // TODO: fix time

    sref<NSSeqRotate<DeltaMoveRotate>> nsRotate{
        new NSSeqRotate<DeltaMoveRotate>(hf.getRandGen())};
    sref<NSSeqSwapCenter<DeltaMoveSwapCenter>> nsSwapCenter =
        new NSSeqSwapCenter<DeltaMoveSwapCenter>(hf.getRandGen());
    sref<NSSeqSwapCorner<DeltaMoveSwapCorner>> nsSwapCorner =
        new NSSeqSwapCorner<DeltaMoveSwapCorner>(hf.getRandGen());
    sref<NSSeqSwapRotateCenter<>> nsSwapRotateCenter =
        new NSSeqSwapRotateCenter<>(hf.getRandGen());
    sref<NSSeqSwapSide<DeltaMoveSwapSide>> nsSwapSide =
        new NSSeqSwapSide<DeltaMoveSwapSide>(hf.getRandGen());

    s.print();

    // EtIIEvaluator& eval = *new EtIIEvaluator(*p);
    // sref<GeneralEvaluator<ESolutionEtII>> eval(new EtIIEvaluator(*p));
    sref<Component> eval(new EtIIEvaluator(*p));

    Evaluation<> e =
        ((Evaluator<SolutionEtII, EvaluationEtII, ESolutionEtII>&)*eval)
            .evaluate(s);

    e.print();
    std::cout << std::endl;

    hf.addComponent(is, "OptFrame:Constructive");
    hf.addComponent(eval, "OptFrame:GeneralEvaluator");
    hf.addComponent(nsRotate, "OptFrame:NS:NSFind:NSSeq");
    hf.addComponent(nsSwapCenter, "OptFrame:NS:NSFind:NSSeq");
    hf.addComponent(nsSwapCorner, "OptFrame:NS:NSFind:NSSeq");
    hf.addComponent(nsSwapRotateCenter, "OptFrame:NS:NSFind:NSSeq");
    hf.addComponent(nsSwapSide, "OptFrame:NS:NSFind:NSSeq");

    std::cout << "problem '" << filename << "' loaded successfully" << std::endl;

    return true;
  }

  bool unload(HeuristicFactory<ESolutionEtII>& factory,
              map<std::string, std::string>& dictionary,
              map<string, vector<string>>& ldictionary) {
    if (p) delete p;
    p = NULL;

    return true;
  }
};

}  // namespace EtII

#endif /*ETII_PROBLEMMODULE_HPP_*/
