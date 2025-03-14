#ifndef TSP_PROBLEMMODULE_HPP_
#define TSP_PROBLEMMODULE_HPP_

#include <OptFrame/Scanner++/Scanner.hpp>
#include <iostream>

#include "Representation.h"
//#include "Memory.h"
//#include "Solution.h"
#include "../../OptFrame/HeuristicFactory.hpp"
#include "../../OptFrame/Util/NSAdapter/NSSeqTSP2Opt.hpp"
#include "../../OptFrame/Util/NSAdapter/NSSeqTSPOrOpt.hpp"
#include "../../OptFrame/Util/NSAdapter/NSSeqTSPOrOptk.hpp"
#include "../../OptFrame/Util/NSAdapter/NSSeqTSPSwap.hpp"
#include "ConstructiveBestInsertion.h"
#include "DeltaMoveTSP2Opt.h"
#include "DeltaMoveTSPOrOptk.h"
#include "Evaluation.h"
#include "Evaluator.h"
#include "NSEnumShift.h"
#include "NSEnumSwap.h"
#include "NearestNeighborConstructive.h"
#include "ProblemInstance.h"
#include "RandomInitialSolution.h"

using namespace scannerpp;
using namespace optframe;

namespace TSP {

class TSPProblemCommand {
 public:
  ProblemInstance* p;

  TSPProblemCommand() { p = NULL; }

  virtual ~TSPProblemCommand() {
    if (p) delete p;
  }

  string id() { return "problem.TSP"; }

  bool registerComponent(
      Component& component, string type, string name,
      HeuristicFactory<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& hf,
      map<std::string, std::string>& dictionary) {
    int idx = hf.addComponent(component, type);
    std::stringstream ss;
    ss << type << " " << idx;
    return true;  // defineText(name, ss.str(), dictionary);
  }

  bool load(string filename,
            HeuristicFactory<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& hf,
            map<std::string, std::string>& dictionary,
            map<string, vector<string>>& ldictionary) {
    File* file;

    try {
      file = new File(filename);
    } catch (FileNotFound& f) {
      std::cout << "File '" << filename << "' not found" << std::endl;
      return false;
    }

    Scanner scanner(file);

    p = new ProblemInstance(scanner);

    // add everything to the HeuristicFactory 'hf'

    RandomInitialSolutionTSP& is =
        *new RandomInitialSolutionTSP(p, hf.getRandGen());
    hf.addComponent(is, "OptFrame:InitialSearch");

    hf.addComponent(*new NearestNeighborConstructive(p, hf.getRandGen()),
                    "OptFrame:InitialSearch");
    hf.addComponent(*new ConstructiveBestInsertion(p, hf.getRandGen()),
                    "OptFrame:InitialSearch");

    TSPEvaluator& eval = *new TSPEvaluator(p);
    hf.addComponent(
        eval, Evaluator<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>::idComponent());

    NSEnumSwap& ns = *new NSEnumSwap(p, hf.getRandGen());
    hf.addComponent(ns, "OptFrame:NS:NSFind:NSSeq");

    NSSeq<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>* nsseq_delta_2opt =
        new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, MemTSP, DeltaMoveTSP2Opt,
                         ProblemInstance>(p);
    hf.addComponent(*nsseq_delta_2opt, "OptFrame:NS:NSFind:NSSeq");

    hf.addComponent(*new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, MemTSP>,
                    "OptFrame:NS:NSFind:NSSeq");  // no optimization

    NSSeq<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>* nsseq_delta_or1 =
        new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP,
                           DeltaMoveTSPOrOptk, ProblemInstance>(1, p);
    hf.addComponent(*nsseq_delta_or1, "OptFrame:NS:NSFind:NSSeq");
    hf.addComponent(*new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP>(1),
                    "OptFrame:NS:NSFind:NSSeq");
    hf.addComponent(*new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP>(2),
                    "OptFrame:NS:NSFind:NSSeq");
    hf.addComponent(*new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP>(3),
                    "OptFrame:NS:NSFind:NSSeq");
    // hf.addComponent(*new NSSeqTSPOrOpt<int, OPTFRAME_DEFAULT_ADS, MemTSP>,
    // "OptFrame:NS:NSFind:NSSeq");
    hf.addComponent(*new NSSeqTSPSwap<int, OPTFRAME_DEFAULT_ADS, MemTSP>,
                    "OptFrame:NS:NSFind:NSSeq");

    ILSLPerturbationLPlus2<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>* ilsl_pert;
    ilsl_pert =
        new ILSLPerturbationLPlus2<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>(
            eval, 50, *nsseq_delta_2opt, hf.getRandGen());
    ilsl_pert->add_ns(*nsseq_delta_or1);

    hf.addComponent(*ilsl_pert);

    std::cout << "problem '" << filename << "' loaded successfully" << std::endl;

    return true;
  }

  bool unload(HeuristicFactory<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& factory,
              map<std::string, std::string>& dictionary,
              map<string, vector<string>>& ldictionary) {
    if (p) delete p;
    p = NULL;

    std::cout << "problem instance for TSP unloaded successfully (use 'drop_all' if "
            "you want to remove all components)"
         << std::endl;

    return true;
  }
};

}  // namespace TSP

#endif /*TSP_PROBLEMMODULE_HPP_*/
