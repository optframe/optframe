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

    bool read(string filename, HeuristicFactory<Rep$project $commamproject>* hf)
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

        ProblemInstance* p = new ProblemInstance(scanner);

        // add everything to the HeuristicFactory 'hf'

        hf->addComponent(*new $projectEvaluator(*p));

        hf->addComponent(*new Constructive$constructive(*p));
		
        cout << "problem '" << filename << "' loaded successfully" << endl;
        
        return true;
    }
};

}

#endif /*$project_PROBLEMMODULE_HPP_*/

