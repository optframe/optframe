#ifndef PN_PROBLEMINSTANCE_H_
#define PN_PROBLEMINSTANCE_H_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"
#include "../../OptFrame/Component.hpp"

using namespace std;
using namespace scannerpp;
using namespace optframe;

namespace PN
{

class ProblemInstance : public Component
{
private:
	// Your private vars

public:
	int size;
	vector<double> nums;
	ProblemInstance(Scanner& scanner);
	
	virtual ~ProblemInstance();
};

}

#endif /*PN_PROBLEMINSTANCE_H_*/

