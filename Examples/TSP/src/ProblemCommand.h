#ifndef TSP_PROBLEMMODULE_HPP_
#define TSP_PROBLEMMODULE_HPP_

#include <iostream>

#include <OptFrame/Scanner++/Scanner.hpp>

#include "Representation.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.h"

#include "Evaluator.h"

#include "RandomInitialSolution.h"
#include "NearestNeighborConstructive.h"
#include "ConstructiveBestInsertion.h"

#include <OptFrame/HeuristicFactory.hpp>
#include <OptFrame/Util/NeighborhoodStructures/NSSeqTSP2Opt.hpp>
#include "DeltaMoveTSP2Opt.h"

#include <OptFrame/Util/NeighborhoodStructures/NSSeqTSPOrOpt.hpp>
#include <OptFrame/Util/NeighborhoodStructures/NSSeqTSPOrOptk.hpp>
#include "DeltaMoveTSPOrOptk.h"

#include <OptFrame/Util/NeighborhoodStructures/NSSeqTSPSwap.hpp>

#include "NSEnumSwap.h"
#include "NSEnumShift.h"

#include <OptFrame/HeuristicFactory.hpp>

using namespace scannerpp;
using namespace optframe;

//using HeuristicFactoryTSP = HeuristicFactory<RepTSP, OPTFRAME_DEFAULT_ADS, SolutionTSP>;
//using HeuristicFactoryTSP = HeuristicFactory<ESolutionTSP>;
// template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>>
using HeuristicFactoryTSP = HeuristicFactory<SolutionTSP, EvaluationTSP, ESolutionTSP>;

namespace TSP
{

class TSPProblemCommand
{
public:

    std::shared_ptr<ProblemInstance> p;
    
    TSPProblemCommand()
    {
        p = nullptr;
    }

    virtual ~TSPProblemCommand()
    {
        //if(p)
        //   delete p;    
    }

    string id()
    {
    	return "problem.TSP";
    }

    bool registerComponent(Component& component, string type, string name, HeuristicFactoryTSP& hf, map<string, string>& dictionary)
    {
       int idx = hf.addComponent(component, type);
       stringstream ss;
       ss << type << " " << idx;
       return true; //defineText(name, ss.str(), dictionary);
    }

	bool load(Scanner& scanner, HeuristicFactoryTSP& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
        p = std::shared_ptr<ProblemInstance>(new ProblemInstance(scanner));

        // add everything to the HeuristicFactory 'hf'

        sref<TSPEvaluator> eval = new TSPEvaluator(p);
        hf.addComponent(eval, Evaluator<ESolutionTSP>::idComponent());

        sref<RandomInitialSolutionTSP> is =  new RandomInitialSolutionTSP(p, eval, hf.getRandGen());
        hf.addComponent(is);

        sref<NearestNeighborConstructive> nnc = new NearestNeighborConstructive(p, eval, hf.getRandGen());
        hf.addComponent( nnc);
        sref<ConstructiveBestInsertion> cbi = new ConstructiveBestInsertion(p, eval, hf.getRandGen());
        hf.addComponent( cbi );


        sref<NSEnumSwap> ns = new NSEnumSwap(p, hf.getRandGen());
        hf.addComponent(ns, "OptFrame:NS:NSSeq");


         //template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, class MOVE = MoveTSPSwap<T, ADS, S>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPSwap<T, ADS, S, MOVE, P>, XEvaluation XEv = Evaluation<>>
        sref<NSSeq<ESolutionTSP>> nsseq_delta_2opt = new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP, DeltaMoveTSP2Opt, ProblemInstance>(p);
        hf.addComponent(nsseq_delta_2opt, "OptFrame:NS:NSSeq");

        sref<NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>> nsseqtsp2opt = new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>();
        hf.addComponent(nsseqtsp2opt , "OptFrame:NS:NSSeq"); // no optimization

        //NSSeq<ESolutionTSP>* nsseq_delta_or1 = new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, DeltaMoveTSPOrOptk, ProblemInstance>(1, p);
        ////MoveTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP, EvaluationTSP, ESolutionTSP>* pmove = new DeltaMoveTSPOrOptk(0,0,0,nullptr);
        //NSSeq<ESolutionTSP>* nsseq_delta_or1;
        // template<class T, class ADS, XBaseSolution<vector<T>,ADS> S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, class MOVE = MoveTSPOrOptk<T, ADS, S, XEv, XES>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorTSPOrOptk<T, ADS, S, XEv, XES, MOVE, P>, XSearch<XES> XSH = std::pair<S, XEv>>
        sref<NSSeq<ESolutionTSP>> nsseq_delta_or1 = new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP, DeltaMoveTSPOrOptk, ProblemInstance>(1, p);
        hf.addComponent(nsseq_delta_or1, "OptFrame:NS:NSSeq");
        sref<NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>> nsseqtsporoptk1 = new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>(1);
        sref<NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>> nsseqtsporoptk2 = new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>(2);
        sref<NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>> nsseqtsporoptk3 = new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>(3);
        hf.addComponent(nsseqtsporoptk1, "OptFrame:NS:NSSeq");
        hf.addComponent(nsseqtsporoptk2, "OptFrame:NS:NSSeq");
        hf.addComponent(nsseqtsporoptk3, "OptFrame:NS:NSSeq");
        //hf.addComponent(*new NSSeqTSPOrOpt<int, OPTFRAME_DEFAULT_ADS>, "OptFrame:NS:NSSeq");
        sref<NSSeqTSPSwap<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>> nsseqtspswap = new NSSeqTSPSwap<int, OPTFRAME_DEFAULT_ADS, SolutionTSP>;
        hf.addComponent(nsseqtspswap, "OptFrame:NS:NSSeq");


        sref<ILSLPerturbationLPlus2<ESolutionTSP>> ilsl_pert =
         new ILSLPerturbationLPlus2<ESolutionTSP> (eval, nsseq_delta_2opt, hf.getRandGen());
        ilsl_pert->add_ns(nsseq_delta_or1);

        hf.addComponent(ilsl_pert);

		
        cout << "TSP problem loaded successfully" << endl;
        
        return true;
    }
    
    bool unload(HeuristicFactoryTSP& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
    {
       //if(p)
       //   delete p;
       //p = NULL;
       
       cout << "problem instance for TSP unloaded successfully (use 'drop_all' if you want to remove all components)" << endl;

       return true;
    }
};

}

#endif /*TSP_PROBLEMMODULE_HPP_*/

