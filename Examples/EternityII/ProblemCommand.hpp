#ifndef ETII_PROBLEMMODULE_HPP_
#define ETII_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/Commands/ProblemCommand.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#include "Evaluator.hpp"

#include "NSSeqRotate.hpp"
#include "DeltaMoveRotate.hpp"

#include "NSSeqSwapCenter.hpp"
#include "DeltaMoveSwapCenter.hpp"

#include "NSSeqSwapCorner.hpp"
#include "DeltaMoveSwapCorner.hpp"

#include "NSSeqSwapRotateCenter.hpp"

#include "NSSeqSwapSide.hpp"
#include "DeltaMoveSwapSide.hpp"

using namespace scannerpp;
using namespace optframe;

namespace EtII
{

class EtIIProblemCommand : public ProblemCommand<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>
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
    	string parentId = ProblemCommand<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>::id();
    	parentId.append("EtII");
    	return parentId;
    }

    bool registerComponent(Component& component, string type, string name, HeuristicFactory<RepEtII, OPTFRAME_DEFAULT_ADS, MemEtII>& hf, map<string, string>& dictionary)
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

