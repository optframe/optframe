#ifndef $project_PROBLEMMODULE_HPP_
#define $project_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/Commands/ProblemCommand.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "DeltaStructure.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

using namespace scannerpp;
using namespace optframe;

namespace $project
{

class $projectProblemCommand : public ProblemCommand<Rep$project , OPTFRAME_DEFAULT_ADS $commadproject>
{
public:

    ProblemInstance* p;
    
    $projectProblemCommand()
    {
        p = NULL;
    }

    virtual ~$projectProblemCommand()
    {
        if(p)
           delete p;    
    }
    
    string id()
    {
    	string parentId = ProblemCommand<Rep$project , OPTFRAME_DEFAULT_ADS $commadproject>::id();
    	parentId.append("$project"); // implements 'problem.project_name' module
    	return parentId;
    }
    
    bool registerComponent(Component& component, string type, string name, HeuristicFactory<Rep$project , OPTFRAME_DEFAULT_ADS $commadproject>& hf, map<string, string>& dictionary)
    {
	    int idx = hf.addComponent(component, type);
	    stringstream ss;
	    ss << type << " " << idx;
	    return defineText(name, ss.str(), dictionary);
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
       
       cout << "problem instance for $project unloaded successfully (use 'drop_all' if you want to remove all components)" << endl;
       
       
       return true;
    }
};

}

#endif /*$project_PROBLEMMODULE_HPP_*/

