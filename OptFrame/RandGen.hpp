#ifndef RANDGEN_HPP_
#define RANDGEN_HPP_

// reuse of function 'rand()' by using function 'randgen_sys_rand()'
unsigned int randgen_sys_rand()
{
	return rand();
}

class RandGen
{
protected:
	unsigned int seed;

public:
	RandGen()
	{
		seed = time(NULL);
		srand(seed);
	}

	RandGen(unsigned int _seed) :
		seed(_seed)
	{
		srand(seed);
	}

	unsigned int rand()
	{
		// reuse of function 'rand()' by using function 'randgen_sys_rand()'
		return randgen_sys_rand();
	}

	unsigned int getSeed()
	{
		return seed;
	}
};

#endif /* RANDGEN_HPP_ */
