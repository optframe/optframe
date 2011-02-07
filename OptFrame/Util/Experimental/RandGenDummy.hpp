#ifndef RANDGENDummy_HPP_
#define RANDGENDummy_HPP_

#include "../../RandGen.hpp"

class RandGenDummy: public RandGen
{
private:
	unsigned int i;

public:

	RandGenDummy() :
		RandGen()
	{
		initialize();
	}

	RandGenDummy(unsigned int _seed)
	{
		RandGen::seed = _seed;
		initialize();
	}

	void initialize()
	{
		i = RandGen::seed;
	}

	unsigned int rand()
	{
		return i++;
	}
};

#endif /* RANDGENDummy_HPP_ */
