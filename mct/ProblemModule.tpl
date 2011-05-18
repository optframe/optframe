#ifndef $project_PROBLEMMODULE_HPP_
#define $project_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/OptFrameModule.hpp"
#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.hpp"

class $projectProblemModule : public OptFrameModule<Rep$project $commamproject>
{
public:
	string id()
	{
		return "problem";
	}

	string usage()
	{
		return "problem filename";
	}

	void run(vector<OptFrameModule<Rep$project $commamproject>*> all_modules, HeuristicFactory<Rep$project $commamproject>* hf, map<string,string>* dictionary, string rest)
	{

		Scanner scanner(new File(rest));

		$projectProblemInstance* p = new $projectProblemInstance(scanner);

		// add everything to the HeuristicFactory 'hf'

		hf->add_ev(new $projectEvaluator(*p));

		hf->add_initsol(new $projectInitialSolution$initialsolution(*p));

	}

};

#endif /*$project_PROBLEMMODULE_HPP_*/

