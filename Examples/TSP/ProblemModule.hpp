#ifndef TSP_PROBLEMMODULE_HPP_
#define TSP_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/Commands/ProblemCommand.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#include "Evaluator.hpp"

#include "RandomInitialSolution.hpp"
#include "NearestNeighborConstructive.hpp"
#include "ConstructiveBestInsertion.hpp"

#include "../../OptFrame/Util/NeighborhoodStructures/NSSeqTSP2Opt.hpp"
#include "DeltaMoveTSP2Opt.hpp"

#include "../../OptFrame/Util/NeighborhoodStructures/NSSeqTSPOrOpt.hpp"
#include "../../OptFrame/Util/NeighborhoodStructures/NSSeqTSPOrOptk.hpp"
#include "DeltaMoveTSPOrOptk.hpp"

#include "../../OptFrame/Util/NeighborhoodStructures/NSSeqTSPSwap.hpp"

using namespace scannerpp;
using namespace optframe;

namespace TSP
{

class TSPProblemCommand : public ProblemCommand<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>
{
public:

    ProblemInstance* p;
    
    TSPProblemCommand()
    {
        p = NULL;
    }

    virtual ~TSPProblemCommand()
    {
        if(p)
           delete p;    
    }

    string id()
    {
    	string parentId = ProblemCommand<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>::id();
    	parentId.append("TSP");
    	return parentId;
    }

    bool registerComponent(Component& component, string type, string name, HeuristicFactory<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& hf, map<string, string>& dictionary)
    {
       int idx = hf.addComponent(component, type);
       stringstream ss;
       ss << type << " " << idx;
       return defineText(name, ss.str(), dictionary);
    }

	bool load(string filename, HeuristicFactory<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
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

        p = new ProblemInstance(scanner);

        // add everything to the HeuristicFactory 'hf'

        RandomInitialSolutionTSP& is = * new RandomInitialSolutionTSP(p, hf.getRandGen());
        hf.addComponent(is);

        hf.addComponent(* new NearestNeighborConstructive(p, hf.getRandGen()));
        hf.addComponent(* new ConstructiveBestInsertion(p, hf.getRandGen()));

        TSPEvaluator& eval = *new TSPEvaluator(p);
        hf.addComponent(eval, Evaluator<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>::idComponent());

        NSEnumSwap& ns = *new NSEnumSwap(p, hf.getRandGen());
        hf.addComponent(ns, "OptFrame:NS:NSSeq");


        NSSeq<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>* nsseq_delta_2opt = new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, MemTSP, DeltaMoveTSP2Opt, ProblemInstance>(p);
        hf.addComponent(*nsseq_delta_2opt, "OptFrame:NS:NSSeq");

        hf.addComponent(*new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, MemTSP>, "OptFrame:NS:NSSeq"); // no optimization

        NSSeq<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>* nsseq_delta_or1 = new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP, DeltaMoveTSPOrOptk, ProblemInstance>(1, p);
        hf.addComponent(*nsseq_delta_or1, "OptFrame:NS:NSSeq");
        hf.addComponent(*new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP>(1), "OptFrame:NS:NSSeq");
        hf.addComponent(*new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP>(2), "OptFrame:NS:NSSeq");
        hf.addComponent(*new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP>(3), "OptFrame:NS:NSSeq");
        //hf.addComponent(*new NSSeqTSPOrOpt<int, OPTFRAME_DEFAULT_ADS, MemTSP>, "OptFrame:NS:NSSeq");
        hf.addComponent(*new NSSeqTSPSwap<int, OPTFRAME_DEFAULT_ADS, MemTSP>, "OptFrame:NS:NSSeq");


        ILSLPerturbationLPlus2<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>* ilsl_pert;
        ilsl_pert = new ILSLPerturbationLPlus2<RepTSP,  OPTFRAME_DEFAULT_ADS, MemTSP> (eval, 50, *nsseq_delta_2opt, hf.getRandGen());
        ilsl_pert->add_ns(*nsseq_delta_or1);

        hf.addComponent(*ilsl_pert);

		
        cout << "problem '" << filename << "' loaded successfully" << endl;
        
        return true;
    }
    
    bool unload(HeuristicFactory<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
    {
       if(p)
          delete p;
       p = NULL;
       
       cout << "problem instance for TSP unloaded successfully (use 'drop_all' if you want to remove all components)" << endl;

       return true;
    }
};

}

#endif /*TSP_PROBLEMMODULE_HPP_*/

