#ifndef RANDGENMersenneTwister_HPP_
#define RANDGENMersenneTwister_HPP_

#include "../RandGen.hpp"

class RandGenMersenneTwister: public RandGen
{
public:

	RandGenMersenneTwister() :
		RandGen()
	{
		initialize();
	}

	RandGenMersenneTwister(unsigned int _seed)
	{
		RandGen::seed = _seed;
		initialize();
	}

	void initialize()
	{
		// initialize random number generation
	}

	unsigned int rand()
	{
		// generate random number
		return 0;
	}
};

#endif /* RANDGENMersenneTwister_HPP_ */
