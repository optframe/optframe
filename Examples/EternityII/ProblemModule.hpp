#ifndef ETII_PROBLEMMODULE_HPP_
#define ETII_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/Modules/ProblemModule.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#include "Evaluator.hpp"

using namespace scannerpp;

namespace EtII
{

class EtIIProblemModule : public ProblemModule<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>
{
public:

    ProblemInstance* p;
    
    EtIIProblemModule()
    {
        p = NULL;
    }

    virtual ~EtIIProblemModule()
    {
        if(p)
           delete p;    
    }

    string id()
    {
    	string parentId = ProblemModule<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::id();
    	parentId.append("EtII");
    	return parentId;
    }

    bool registerComponent(OptFrameComponent& component, string type, string name, HeuristicFactory<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& hf, map<string, string>& dictionary)
    {
       int idx = hf.addComponent(component, type);
       stringstream ss;
       ss << type << " " << idx;
       return defineText(name, ss.str(), dictionary);
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
    	NSSeqSwapCenter& nsSwapCenter = * new NSSeqSwapCenter(hf.getRandGen());
    	NSSeqSwapCorner& nsSwapCorner = * new NSSeqSwapCorner(hf.getRandGen());
    	NSSeqSwapRotateCenter& nsSwapRotateCenter = * new NSSeqSwapRotateCenter(hf.getRandGen());
    	NSSeqSwapSide& nsSwapSide = * new NSSeqSwapSide(hf.getRandGen());

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

