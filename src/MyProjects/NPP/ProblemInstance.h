#ifndef NPP_PROBLEMINSTANCE_H_
#define NPP_PROBLEMINSTANCE_H_

#include <iostream>
#include <vector>

#include "../../OptFrame/Scanner++/Scanner.h"
#include "../../OptFrame/Component.hpp"

using namespace std;
using namespace scannerpp;
using namespace optframe;

namespace NPP
{

class ProblemInstance : public Component
{
private:
	int size;
	vector<int> numbers;

public:
	ProblemInstance(Scanner& scanner);
	
	virtual ~ProblemInstance();

	int operator[](int idx) const {
	    return this->numbers[idx];
	}

	int getSize() const {
        return this->size;
	}
};

}

#endif /*NPP_PROBLEMINSTANCE_H_*/

