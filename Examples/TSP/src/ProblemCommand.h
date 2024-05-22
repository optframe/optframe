// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2024 - OptFrame - https://github.com/optframe/optframe

#ifndef TSP_PROBLEMMODULE_HPP_
#define TSP_PROBLEMMODULE_HPP_

// C++
#include <iostream>
//

#include <OptFrame/Hyper/HeuristicFactory.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/NSAdapter/NSSeqTSP2Opt.hpp>
#include <OptFrame/Util/NSAdapter/NSSeqTSPOrOpt.hpp>
#include <OptFrame/Util/NSAdapter/NSSeqTSPOrOptk.hpp>
#include <OptFrame/Util/NSAdapter/NSSeqTSPSwap.hpp>

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
#include "Representation.h"
#include "Solution.h"

using namespace scannerpp;
using namespace optframe;

// using HeuristicFactoryTSP = HeuristicFactory<RepTSP, OPTFRAME_DEFAULT_ADS,
// SolutionTSP>; using HeuristicFactoryTSP = HeuristicFactory<ESolutionTSP>;
//  template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES =
//  pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
static_assert(X2ESolution<MultiESolution<ESolutionTSP>, ESolutionTSP>);
//
using HeuristicFactoryTSP = HeuristicFactory<ESolutionTSP>;

namespace TSP {
class TSPProblemCommand {
 public:
  std::shared_ptr<ProblemInstance> p;

  TSPProblemCommand() { p = nullptr; }

  virtual ~TSPProblemCommand() {
    // if(p)
    //    delete p;
  }

  string id() { return "problem.TSP"; }

  bool registerComponent(sref<Component> component, string type, string name,
                         HeuristicFactoryTSP& hf,
                         map<string, string>& dictionary) {
    int idx = hf.addComponent(component, type);
    stringstream ss;
    ss << type << " " << idx;
    return true;  // defineText(name, ss.str(), dictionary);
  }

  bool load(Scanner& scanner, HeuristicFactoryTSP& hf,
            map<string, string>& dictionary,
            map<string, vector<string>>& ldictionary) {
    p = std::shared_ptr<ProblemInstance>(new ProblemInstance(scanner));

    // add everything to the HeuristicFactory 'hf'

    sref<TSPEvaluator> eval{new TSPEvaluator(p)};
    hf.addComponent(eval, Evaluator<SolutionTSP, EvaluationTSP>::idComponent());

    sref<RandomInitialSolutionTSP> is =
        new RandomInitialSolutionTSP(p, eval, hf.getRandGen());
    hf.addComponent(is, "OptFrame:InitialSearch");

    sref<NearestNeighborConstructive> nnc =
        new NearestNeighborConstructive(p, eval, hf.getRandGen());
    hf.addComponent(nnc, "OptFrame:InitialSearch");
    sref<ConstructiveBestInsertion> cbi =
        new ConstructiveBestInsertion(p, eval, hf.getRandGen());
    hf.addComponent(cbi, "OptFrame:InitialSearch");

    sref<NSEnumSwap> ns = new NSEnumSwap(p, hf.getRandGen());
    hf.addComponent(ns, "OptFrame:NS:NSFind:NSSeq");

    // template<class T, class ADS = OPTFRAME_DEFAULT_ADS,
    // XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE
    // = MoveTSPSwap<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class
    // NSITERATOR = NSIteratorTSPSwap<T, ADS, S, MOVE, P>, XEvaluation XEv =
    // Evaluation<>>
    sref<NSSeq<ESolutionTSP>> nsseq_delta_2opt =
        new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP,
                         DeltaMoveTSP2Opt, ProblemInstance>(p);
    hf.addComponent(nsseq_delta_2opt, "OptFrame:NS:NSFind:NSSeq");

    sref<NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>> nsseqtsp2opt =
        new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>();
    hf.addComponent(nsseqtsp2opt,
                    "OptFrame:NS:NSFind:NSSeq");  // no optimization

    // NSSeq<ESolutionTSP>* nsseq_delta_or1 = new NSSeqTSPOrOptk<int,
    // OPTFRAME_DEFAULT_ADS, DeltaMoveTSPOrOptk, ProblemInstance>(1, p);
    ////MoveTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP, EvaluationTSP,
    /// ESolutionTSP>* pmove = new DeltaMoveTSPOrOptk(0,0,0,nullptr);
    // NSSeq<ESolutionTSP>* nsseq_delta_or1;
    //  template<class T, class ADS, XBaseSolution<vector<T>,ADS> S, XEvaluation
    //  XEv = Evaluation<>, XESolution XES = pair<S, XEv>, class MOVE =
    //  MoveTSPOrOptk<T, ADS, S, XEv, XES>, class P = OPTFRAME_DEFAULT_PROBLEM,
    //  class NSITERATOR = NSIteratorTSPOrOptk<T, ADS, S, XEv, XES, MOVE, P>,
    //  XSearch<XES> XSH = std::pair<S, XEv>>
    sref<NSSeq<ESolutionTSP>> nsseq_delta_or1 =
        new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP,
                           DeltaMoveTSPOrOptk, ProblemInstance>(1, p);
    hf.addComponent(nsseq_delta_or1, "OptFrame:NS:NSFind:NSSeq");
    sref<NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>>
        nsseqtsporoptk1 =
            new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>(1);
    sref<NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>>
        nsseqtsporoptk2 =
            new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>(2);
    sref<NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>>
        nsseqtsporoptk3 =
            new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>(3);
    hf.addComponent(nsseqtsporoptk1, "OptFrame:NS:NSFind:NSSeq");
    hf.addComponent(nsseqtsporoptk2, "OptFrame:NS:NSFind:NSSeq");
    hf.addComponent(nsseqtsporoptk3, "OptFrame:NS:NSFind:NSSeq");
    // hf.addComponent(*new NSSeqTSPOrOpt<int, OPTFRAME_DEFAULT_ADS>,
    // "OptFrame:NS:NSFind:NSSeq");
    sref<NSSeqTSPSwap<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>> nsseqtspswap =
        new NSSeqTSPSwap<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>;
    hf.addComponent(nsseqtspswap, "OptFrame:NS:NSFind:NSSeq");

    sref<ILSLPerturbationLPlus2<ESolutionTSP>> ilsl_pert =
        new ILSLPerturbationLPlus2<ESolutionTSP>(eval, nsseq_delta_2opt,
                                                 hf.getRandGen());
    ilsl_pert->add_ns(nsseq_delta_or1);

    hf.addComponent(ilsl_pert, ilsl_pert->id());

    cout << "TSP problem loaded successfully" << endl;

    return true;
  }

  bool unload(HeuristicFactoryTSP& factory, map<string, string>& dictionary,
              map<string, vector<string>>& ldictionary) {
    // if(p)
    //    delete p;
    // p = NULL;

    cout << "problem instance for TSP unloaded successfully (use 'drop_all' if "
            "you want to remove all components)"
         << endl;

    return true;
  }
};

}  // namespace TSP

#endif /*TSP_PROBLEMMODULE_HPP_*/
