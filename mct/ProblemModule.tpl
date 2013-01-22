#ifndef $project_PROBLEMMODULE_HPP_
#define $project_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/Modules/ProblemModule.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

using namespace scannerpp;

namespace $project
{

class $projectProblemModule : public ProblemModule<Rep$project , OPTFRAME_DEFAULT_ADS $commadproject>
{
public:

    ProblemInstance* p;
    
    $projectProblemModule()
    {
        p = NULL;
    }

    virtual ~$projectProblemModule()
    {
        if(p)
           delete p;    
    }
    
    string id()
    {
    	string parentId = ProblemModule<Rep$project , OPTFRAME_DEFAULT_ADS $commadproject>::id();
    	parentId.append("$project"); // implements 'problem.project_name' module
    	return parentId;
    }

	bool load(string filename, HeuristicFactory<Rep$project , OPTFRAME_DEFAULT_ADS $commadproject>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
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

        hf.addComponent(*new $projectEvaluator(*p));

        hf.addComponent(*new Constructive$constructive(*p));
		
        cout << "problem '" << filename << "' loaded successfully" << endl;
        
        return true;
    }
    
    bool unload(HeuristicFactory<Rep$project , OPTFRAME_DEFAULT_ADS $commadproject>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
    {
       if(p)
          delete p;
       p = NULL;
       
       return true;
    }
};

}

#endif /*$project_PROBLEMMODULE_HPP_*/

