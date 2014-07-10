#ifndef ETII_PROBLEMMODULE_HPP_
#define ETII_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.h"

#include "Evaluator.h"

#include "NSSeqRotate.h"
#include "DeltaMoveRotate.h"

#include "NSSeqSwapCenter.h"
#include "DeltaMoveSwapCenter.h"

#include "NSSeqSwapCorner.h"
#include "DeltaMoveSwapCorner.h"

#include "NSSeqSwapRotateCenter.h"

#include "NSSeqSwapSide.h"
#include "DeltaMoveSwapSide.h"

#include "InitialSolutionGreedy.h"

#include "../../OptFrame/HeuristicFactory.hpp"

using namespace scannerpp;
using namespace optframe;

namespace EtII
{

class EtIIProblemCommand
{
public:

    ProblemInstance* p;
    
    EtIIProblemCommand()
    {
        p = NULL;
    }

    virtual ~EtIIProblemCommand()
    {
        if(p)
           delete p;    
    }

    string id()
    {
    	return "problem.EtII";
    }

    bool registerComponent(Component& component, string type, string name, HeuristicFactory<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& hf, map<string, string>& dictionary)
    {
       int idx = hf.addComponent(component, type);
       stringstream ss;
       ss << type << " " << idx;
       return true; //defineText(name, ss.str(), dictionary);
    }

	bool load(string filename, HeuristicFactory<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
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

    	EtIIInitialSolutionGreedy& is = * new EtIIInitialSolutionGreedy(*p, hf.getRandGen());

    	SolutionEtII& s = is.generateSolution();

    	NSSeqRotate<DeltaMoveRotate>& nsRotate = * new NSSeqRotate<DeltaMoveRotate>(hf.getRandGen());
    	NSSeqSwapCenter<DeltaMoveSwapCenter>& nsSwapCenter = * new NSSeqSwapCenter<DeltaMoveSwapCenter>(hf.getRandGen());
    	NSSeqSwapCorner<DeltaMoveSwapCorner>& nsSwapCorner = * new NSSeqSwapCorner<DeltaMoveSwapCorner>(hf.getRandGen());
    	NSSeqSwapRotateCenter<>& nsSwapRotateCenter = * new NSSeqSwapRotateCenter<>(hf.getRandGen());
    	NSSeqSwapSide<DeltaMoveSwapSide>& nsSwapSide = * new NSSeqSwapSide<DeltaMoveSwapSide>(hf.getRandGen());

    	s.print();

    	EtIIEvaluator& eval = * new EtIIEvaluator(*p);
    	EvaluationEtII* e;

    	e = &eval.evaluate(s);

    	e->print();
    	cout << endl;

    	hf.addComponent(is);
    	hf.addComponent(eval);
    	hf.addComponent(nsRotate);
    	hf.addComponent(nsSwapCenter);
    	hf.addComponent(nsSwapCorner);
    	hf.addComponent(nsSwapRotateCenter);
    	hf.addComponent(nsSwapSide);
		
        cout << "problem '" << filename << "' loaded successfully" << endl;
        
        return true;
    }
    
    bool unload(HeuristicFactory<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
    {
       if(p)
          delete p;
       p = NULL;
       
       return true;
    }
};

}

#endif /*ETII_PROBLEMMODULE_HPP_*/

