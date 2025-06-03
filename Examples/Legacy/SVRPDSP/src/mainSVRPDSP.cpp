#include <math.h>
#include <stdlib.h>

#include <iostream>

// #include <OptFrame/OptFrame.hpp>
#include <OptFrame/Core/NSSeq.hpp>
#include <OptFrame/Core/RandGen.hpp>
#include <OptFrame/Util/NSAdapter/NSSeqTSP2Opt.hpp>
#include <OptFrame/Util/NSAdapter/NSSeqTSPOrOptk.hpp>
#include <OptFrame/Util/NSAdapter/NSSeqTSPSwap.hpp>

#define ALL_INTEGER 1

#include <OptFrame/Heuristics/LocalSearches/BestImprovementLOS.hpp>

#include "./DeltaMove2Opt.hpp"
#include "./DeltaMoveOrOptk.hpp"
#include "./DeltaMoveSwap.hpp"
#include "SVRPDSP.h"

using namespace std;
using namespace optframe;
using namespace SVRPDSP;

template <class R = int, class ADS = nullptr_t,
          XBaseSolution<R, ADS> S = CopySolution<R, ADS>>
class Teste {
 public:
  ADS* a;
};

int main(int argc, char** argv) {
  Teste t;
  t.a = nullptr;

  // seed
  int seed = 0;
  RandGen rg(seed);

  std::cout << "Single Vehicle Routing Problem with Deliveries and Selective "
          "Pickups (SVRPDSP)"
       << std::endl;
  std::cout << "By Coelho et al. (2011)" << std::endl;
  std::cout << std::endl;

  string problem = "016_B_half";

  if (argc < 2) {
    std::cout << "missing filename parameter! using '" << problem << "'" << std::endl;
  } else
    problem = argv[1];

  std::cout << "problem: '" << problem << "'" << std::endl << std::endl;

  ProblemInstance* p = NULL;

  stringstream ss_path_p;
  ss_path_p << "./SVRPDSP/all-instances-2013/" << problem << ".txt";
  string path_p = ss_path_p.str();

  Scanner scan_name(problem);
  scan_name.useSeparators("_");
  string prefix = scan_name.next();
  scan_name.next();  // B
  string omega = scan_name.next();
  if (omega == "p") omega = "p_two";

  stringstream ss_path_tsp;
  ss_path_tsp << "./SVRPDSP/tsp-2013-4f/" << prefix << ".tsp";
  string path_tsp = ss_path_tsp.str();

  stringstream ss_path_knp;
  ss_path_knp << "./SVRPDSP/knapsack-2013/" << prefix << "_B_" << omega
              << ".knp";
  string path_knp = ss_path_knp.str();

  File* file;

  try {
    file = new File(path_p);
  } catch (FileNotFound& f) {
    std::cout << "File '" << path_p << "' not found" << std::endl;
    return false;
  }

  Scanner scanner(file);

  p = new ProblemInstance(scanner);

  Scanner tsp_file(new File(path_tsp));
  Scanner knp_file(new File(path_knp));

  ConstructiveTSPOptimal* is_tspopt =
      new ConstructiveTSPOptimal(*p, tsp_file, knp_file);

  SVRPDSPEvaluator* evlr = new SVRPDSPEvaluator(*p);
  S_TSPEvaluator* evlr_tsp = new S_TSPEvaluator(*p);
  S_KNPEvaluator* evlr_knp = new S_KNPEvaluator(*p);

  MySolution& s_lb = *is_tspopt->generateSolution(1000);  // time 1000
  std::cout << "Possible optimal solution" << std::endl;
  // s_lb.print();
  // s_lb.getADS().print();
  Evaluation<> etmp = evlr->evaluate(s_lb);
  etmp.print();

  // ===================================================
  // ===================================================

  delete is_tspopt;
  is_tspopt = NULL;

  // Lower bound estimative

  Evaluation<> etsp1 = evlr_tsp->evaluate(s_lb);
  double tsp_value = etsp1.evaluation();
  Evaluation<> eknp1 = evlr_knp->evaluate(s_lb);
  double knp_value = eknp1.evaluation();

  double lower_bound = tsp_value - knp_value;
  std::cout << "lower bound: " << lower_bound << " (" << tsp_value << " - "
       << knp_value << ")" << std::endl;
  FILE* ftsp = fopen("lower_tsp.out", "a");
  fprintf(ftsp, "%.2f\n", tsp_value);
  fclose(ftsp);
  std::cout << "tsp: ";
  Evaluation<> etsp = evlr_tsp->evaluate(s_lb);
  etsp.print();

  FILE* fknp = fopen("lower_knp.out", "a");
  fprintf(fknp, "%.2f\n", knp_value);
  fclose(fknp);
  std::cout << "knapsack: ";
  Evaluation<> eknp = evlr_knp->evaluate(s_lb);
  eknp.print();

  // s_lb.print();
  Evaluation<> e = evlr->evaluate(s_lb);
  e.print();

  std::cout << std::endl;
  delete &s_lb;

  /*
        std::stringstream ss;
        ss << lower_bound;
        OptFrameModule<RepSVRPDSP, AdsSVRPDSP>::defineText("lower_bound",
     ss.str(), dictionary);

        stringstream ss2;
        ss2 << p->n;
        OptFrameModule<RepSVRPDSP, AdsSVRPDSP>::defineText("n_customers",
     ss2.str(), dictionary);

        stringstream ss3;
        ss3 << p->dN;
        OptFrameModule<RepSVRPDSP, AdsSVRPDSP>::defineText("N", ss3.str(),
     dictionary);

        // add everything to the HeuristicFactory 'hf'

        hf.addComponent(*evlr,     Evaluator<RepSVRPDSP>::idComponent());
        hf.addComponent(*evlr_tsp, Evaluator<RepSVRPDSP>::idComponent());
        hf.addComponent(*evlr_knp, Evaluator<RepSVRPDSP>::idComponent());
        */
  // ----------------------------------------------------------
  // NSSeqTSP2Opt<int, AdsSVRPDSP>* ns_2opt = new NSSeqTSP2Opt<int, AdsSVRPDSP>;
  // NSSeqTSPSwap<int, AdsSVRPDSP>* ns_swap = new NSSeqTSPSwap<int, AdsSVRPDSP>;
  NSSeq<RepSVRPDSP, AdsSVRPDSP>* delta_2opt =
      new NSSeqTSP2Opt<int, AdsSVRPDSP, DeltaMove2Opt, ProblemInstance>(p);
  // if(!registerComponent(*delta_2opt, "OptFrame:NS:NSFind:NSSeq",
  // "nsseq_delta_2opt", hf, dictionary))
  //    return false;

  // registerComponent(*new DeltaMove2Opt(5, 20, p), "OptFrame:Move",
  // "move_2opt_5-20", hf, dictionary);

  NSSeq<RepSVRPDSP, AdsSVRPDSP, MySolution>* delta_swap =
      new NSSeqTSPSwap<int, AdsSVRPDSP, MySolution, DeltaMoveSwap,
                       ProblemInstance>(p);
  // if(!registerComponent(*delta_swap, "OptFrame:NS:NSFind:NSSeq",
  // "nsseq_delta_swap", hf, dictionary))
  //    return false;

  // hf.addComponent(*ns_swap, NSSeq<RepSVRPDSP>::idComponent());
  // hf.addComponent(*ns_2opt, NSSeq<RepSVRPDSP>::idComponent());

  // NSSeqTSPOrOptk<int, AdsSVRPDSP>* ns_or1 = new NSSeqTSPOrOptk<int,
  // AdsSVRPDSP>(1); NSSeqTSPOrOptk<int, AdsSVRPDSP>* ns_or2 = new
  // NSSeqTSPOrOptk<int, AdsSVRPDSP>(2); NSSeqTSPOrOptk<int, AdsSVRPDSP>* ns_or3
  // = new NSSeqTSPOrOptk<int, AdsSVRPDSP>(3);

  NSSeq<RepSVRPDSP, AdsSVRPDSP>* delta_ns_or1 =
      new NSSeqTSPOrOptk<int, AdsSVRPDSP, DeltaMoveOrOptk, ProblemInstance>(1,
                                                                            p);
  // if(!registerComponent(*delta_ns_or1, "OptFrame:NS:NSFind:NSSeq",
  // "nsseq_delta_or1", hf, dictionary))
  //    return false;

  // registerComponent(*new DeltaMoveOrOptk(26,29, 2, p), "OptFrame:Move",
  // "move_or2_26-29", hf, dictionary);

  NSSeq<RepSVRPDSP, AdsSVRPDSP>* delta_ns_or2 =
      new NSSeqTSPOrOptk<int, AdsSVRPDSP, DeltaMoveOrOptk, ProblemInstance>(2,
                                                                            p);
  // if(!registerComponent(*delta_ns_or2, "OptFrame:NS:NSFind:NSSeq",
  // "nsseq_delta_or2", hf, dictionary))
  //    return false;

  // registerComponent(*new DeltaMoveOrOptk(74, 960, 2, p), "OptFrame:Move",
  // "move_or2_74-960", hf, dictionary);

  // NSSeq<RepSVRPDSP, AdsSVRPDSP>* delta_ns_or3 = new NSSeqTSPOrOptk<int,
  // AdsSVRPDSP, DeltaMoveOrOptk, ProblemInstance>(3, p);
  // if(!registerComponent(*delta_ns_or3, "OptFrame:NS:NSFind:NSSeq",
  // "nsseq_delta_or3", hf, dictionary))
  //    return false;

  // hf.addComponent(*ns_or1, NSSeq<RepSVRPDSP>::idComponent());
  // hf.addComponent(*ns_or2, NSSeq<RepSVRPDSP>::idComponent());
  // hf.addComponent(*ns_or3, NSSeq<RepSVRPDSP>::idComponent());

  // cpugpu = 0->cpu 1->gpu 2->both

  /*
        //hf.addComponent(* new CudaBiSwap(*evlr, *p, CPU_GPU),
     LocalSearch<RepSVRPDSP, AdsSVRPDSP>::idComponent());
        if(!registerComponent(* new CudaBiSwap(*evlr, *p, CPU_GPU),
     LocalSearch<RepSVRPDSP, AdsSVRPDSP>::idComponent(), "cuda_bi_swap", hf,
     dictionary)) return false;

        //hf.addComponent(* new CudaBi2Opt(*evlr, *p, CPU_GPU),
     LocalSearch<RepSVRPDSP, AdsSVRPDSP>::idComponent());
        if(!registerComponent(* new CudaBi2Opt(*evlr, *p, CPU_GPU),
     LocalSearch<RepSVRPDSP, AdsSVRPDSP>::idComponent(), "cuda_bi_2opt", hf,
     dictionary)) return false;

        //hf.addComponent(* new CudaBiOr1(*evlr, *p, CPU_GPU),
     LocalSearch<RepSVRPDSP, AdsSVRPDSP>::idComponent());
        if(!registerComponent(* new CudaBiOr1(*evlr, *p, CPU_GPU),
     LocalSearch<RepSVRPDSP, AdsSVRPDSP>::idComponent(), "cuda_bi_or1", hf,
     dictionary)) return false;

        //hf.addComponent(* new CudaBiOr2(*evlr, *p, CPU_GPU),
     LocalSearch<RepSVRPDSP, AdsSVRPDSP>::idComponent());
        if(!registerComponent(* new CudaBiOr2(*evlr, *p, CPU_GPU),
     LocalSearch<RepSVRPDSP, AdsSVRPDSP>::idComponent(), "cuda_bi_or2", hf,
     dictionary)) return false;
        */

  // ----------------------------------------------------------
  ConstructiveRandom* is_random = new ConstructiveRandom(*p);
  // if(!registerComponent(*is_random, Constructive<RepSVRPDSP>::idComponent(),
  // "constructive_random", hf, dictionary))
  //    return false;

  Scanner sc_glo_tsp(new File(path_tsp));
  Scanner sc_glo_knp(new File(path_knp));
  ConstructiveGRandTSPOptimalGLO* is_grandtspopt =
      new ConstructiveGRandTSPOptimalGLO(*p, sc_glo_tsp, sc_glo_knp, 2, 0.0);

  // if(!registerComponent(*is_grandtspopt,
  // Constructive<RepSVRPDSP>::idComponent(), "constructive_grandtspopt2", hf,
  // dictionary))
  //    return false;

  /*
        ILSLPerturbationLPlus2<RepSVRPDSP, AdsSVRPDSP>* ilsl_pert;
        ilsl_pert = new ILSLPerturbationLPlus2<RepSVRPDSP, AdsSVRPDSP> (*evlr,
     50, *ns_swap, hf.getRandGen()); ilsl_pert->add_ns(*ns_2opt);
        ilsl_pert->add_ns(*ns_or2);
        ilsl_pert->add_ns(*ns_or3);
        ilsl_pert->add_ns(*ns_or1);

        hf.addComponent(*ilsl_pert);
        */

  /*
        std::cout << "TESTES COM OR1" << std::endl;
        Solution<RepSVRPDSP>& s = is_random->generateSolution();
        NSIterator<RepSVRPDSP>& it = ns_or1->getIterator(s.getR());
        for(it.first(); !it.isDone(); it.next())
        {
        Move<RepSVRPDSP>& m = it.current();
        std::cout << std::endl;
        m.print();
        s.print(); // print 's'
        Move<RepSVRPDSP>& rev = m.apply(s);
        s.print();
        rev.print();
        delete& rev.apply(s);
        getchar();
        }
        delete &s;
        */

  BestImprovementLOS<RepSVRPDSP, AdsSVRPDSP, MySolution> BILOS(*evlr,
                                                               *delta_swap);

  // cudaInitialize();
  std::cout << "problem.SVRPDSP '" << path_p << "' loaded successfully" << std::endl;

  std::cout << "Program ended successfully" << std::endl;

  return 0;
}
