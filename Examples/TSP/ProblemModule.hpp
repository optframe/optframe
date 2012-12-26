#ifndef TSP_PROBLEMMODULE_HPP_
#define TSP_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/Modules/ProblemModule.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#include "Evaluator.hpp"

#include "../../OptFrame/Util/NeighborhoodStructures/NSSeqTSP2Opt.hpp"
#include "../../OptFrame/Util/NeighborhoodStructures/NSSeqTSPOrOpt.hpp"
#include "../../OptFrame/Util/NeighborhoodStructures/NSSeqTSPOrOptk.hpp"
#include "../../OptFrame/Util/NeighborhoodStructures/NSSeqTSPSwap.hpp"

using namespace scannerpp;

namespace TSP
{

class TSPProblemModule : public ProblemModule<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>
{
public:

    ProblemInstance* p;
    
    TSPProblemModule()
    {
        p = NULL;
    }

    virtual ~TSPProblemModule()
    {
        if(p)
           delete p;    
    }

    string id()
    {
    	string parentId = ProblemModule<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>::id();
    	parentId.append("TSP");
    	return parentId;
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

        TSPEvaluator& eval = *new TSPEvaluator(p);
        hf.addComponent(eval, Evaluator<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>::idComponent());
        hf.addComponent(eval, Evaluator<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>::idComponent());

        NSEnumSwap& ns = *new NSEnumSwap(p, hf.getRandGen());
        hf.addComponent(ns);


        hf.addComponent(*new NSSeqTSP2Opt<int, OPTFRAME_DEFAULT_ADS, MemTSP>);
        hf.addComponent(*new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP>(1));
        hf.addComponent(*new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP>(2));
        hf.addComponent(*new NSSeqTSPOrOptk<int, OPTFRAME_DEFAULT_ADS, MemTSP>(3));
        //hf.addComponent(*new NSSeqTSPOrOpt<int, OPTFRAME_DEFAULT_ADS, MemTSP>);
        hf.addComponent(*new NSSeqTSPSwap<int, OPTFRAME_DEFAULT_ADS, MemTSP>, "OptFrame:NS:NSSeq");

		
        cout << "problem '" << filename << "' loaded successfully" << endl;
        
        return true;
    }
    
    bool unload(HeuristicFactory<RepTSP, OPTFRAME_DEFAULT_ADS, MemTSP>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
    {
       if(p)
          delete p;
       p = NULL;
       
       return true;
    }
};

}

#endif /*TSP_PROBLEMMODULE_HPP_*/

