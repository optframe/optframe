#ifndef $project_PROBLEMINSTANCE_H_
#define $project_PROBLEMINSTANCE_H_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"
#include "../../OptFrame/Component.hpp"

using namespace std;
using namespace scannerpp;
using namespace optframe;

namespace $project
{

class ProblemInstance : public Component
{
private:
	// Your private vars

public:
	ProblemInstance(Scanner& scanner);
	
	virtual ~ProblemInstance();
};

}

#endif /*$project_PROBLEMINSTANCE_H_*/

