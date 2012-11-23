#ifndef $project_PROBLEMMODULE_HPP_
#define $project_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/Modules/ProblemModule.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

namespace $project
{

class $projectProblemModule : public ProblemModule<Rep$project $commamproject>
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

	bool load(string filename, vector<OptFrameModule<Rep$project $commamproject>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<Rep$project $commamproject>& hf, map<string, string>& dictionary)
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
    
    bool unload(vector<OptFrameModule<Rep$project $commamproject>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<Rep$project $commamproject>& factory, map<string, string>& dictionary)
    {
       if(p)
          delete p;
       p = NULL;
       
       return true;
    }
};

}

#endif /*$project_PROBLEMMODULE_HPP_*/

