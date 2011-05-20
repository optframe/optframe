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

class $projectProblemModule : public ProblemModule<Rep$project $commamproject>
{
public:

    void read(string filename, HeuristicFactory<Rep$project $commamproject>* hf)
    {
        File* file;

        try
        {
           file = new File(filename);
        }
        catch (FileNotFound f)
        {
           cout << "File '" << filename <<"' not found" << endl;
           return;
        }

        Scanner scanner(file);

        $projectProblemInstance* p = new $projectProblemInstance(scanner);

        // add everything to the HeuristicFactory 'hf'

        hf->add_ev(new $projectEvaluator(*p));

        hf->add_initsol(new $projectInitialSolution$initialsolution(*p));
		
        cout << "problem '" << filename << "' loaded successfully" << endl;
    }
};

#endif /*$project_PROBLEMMODULE_HPP_*/

