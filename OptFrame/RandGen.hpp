#ifndef RANDGEN_HPP_
#define RANDGEN_HPP_

// reuse of function 'rand()' by using function 'randgen_sys_rand()'
int randgen_sys_rand()
{
	return rand();
}

class RandGen
{
protected:
	int seed;

public:
	RandGen()
	{
		srand(time(NULL));
	}

	RandGen(int _seed) :
		seed(_seed)
	{
		srand(seed);
	}

	int rand()
	{
		// reuse of function 'rand()' by using function 'randgen_sys_rand()'
		return randgen_sys_rand();
	}

	int getSeed()
	{
		return seed;
	}
};

#endif /* RANDGEN_HPP_ */
