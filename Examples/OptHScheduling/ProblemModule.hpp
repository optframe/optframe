#ifndef OptHS_PROBLEMMODULE_HPP_
#define OptHS_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/Modules/ProblemModule.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

#include "Evaluator.hpp"

#include "ConstructiveRandom.hpp"
#include "NSSwap.hpp"


using namespace scannerpp;
using namespace optframe;

namespace OptHS
{

class OptHSProblemModule : public ProblemModule<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>
{
public:

    ProblemInstance* p;
    
    OptHSProblemModule()
    {
        p = NULL;
    }

    virtual ~OptHSProblemModule()
    {
        if(p)
           delete p;    
    }

    string id()
    {
    	string parentId = ProblemModule<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>::id();
    	parentId.append("OptHS");
    	return parentId;
    }

    bool registerComponent(Component& component, string type, string name, HeuristicFactory<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>& hf, map<string, string>& dictionary)
    {
       int idx = hf.addComponent(component, type);
       stringstream ss;
       ss << type << " " << idx;
       return defineText(name, ss.str(), dictionary);
    }

	bool load(string members, HeuristicFactory<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
	    Scanner scanner(members);

        p = new ProblemInstance(scanner);

        // add everything to the HeuristicFactory 'hf'

        ConstructiveRandom& is = * new ConstructiveRandom(*p, hf.getRandGen());
        hf.addComponent(is);

        OptHSEvaluator& eval = *new OptHSEvaluator(*p);
        hf.addComponent(eval, Evaluator<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>::idComponent());
		
        NSSwap* ns = new NSSwap(*p, hf.getRandGen());
        hf.addComponent(*ns);

        cout << "problem '" << members << "' loaded successfully" << endl;
        
        return true;
    }
    
    bool unload(HeuristicFactory<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
    {
       if(p)
          delete p;
       p = NULL;
       
       cout << "problem instance for OptHS unloaded successfully (use 'drop_all' if you want to remove all components)" << endl;

       return true;
    }
};

}

#endif /*OptHS_PROBLEMMODULE_HPP_*/

