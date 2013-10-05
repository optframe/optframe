#ifndef $project_PROBLEMINSTANCE_H_
#define $project_PROBLEMINSTANCE_H_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

using namespace std;
using namespace scannerpp;

namespace $project
{

class ProblemInstance
{
private:
	// Your private vars

public:
	ProblemInstance(Scanner& scanner);
	
	virtual ~ProblemInstance();
};

}

#endif /*$project_PROBLEMINSTANCE_H_*/

