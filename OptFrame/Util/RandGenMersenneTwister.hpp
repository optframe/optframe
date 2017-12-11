// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

//Description and pseudocode in http://en.wikipedia.org/wiki/Mersenne_twister

#ifndef RANDGENMersenneTwister_HPP_
#define RANDGENMersenneTwister_HPP_

#include "../RandGen.hpp"
#include <limits.h>

namespace optframe
{


class RandGenMersenneTwister: public RandGen
{
private:
	static const unsigned int GEN_STATE_LENGTH = 624;
	static const unsigned int MASK_1 = 0xffffffff; //4294967295
	static const unsigned int MASK_2 = 0x80000000; //2147483648
	static const unsigned int MASK_3 = 0x7fffffff; //2147483647

	static const unsigned int CONSTANT_1 = 0x6c078965; //1812433253
	static const unsigned int CONSTANT_2 = 0x9d2c5680; //2636928640
	static const unsigned int CONSTANT_3 = 0xefc60000; //4022730752
	static const unsigned int CONSTANT_4 = 0x9908b0df; //2567483615
	unsigned int MT[GEN_STATE_LENGTH];
	int index;

	void GenNum()
	{
		int i;
		unsigned int y;
		for (i = 0; i < ((int)GEN_STATE_LENGTH); i++)
		{
			y = (MT[i] ^ MASK_2) + (MT[(i + 1) % GEN_STATE_LENGTH] ^ MASK_3);
			MT[i] = MT[(i + 397) % GEN_STATE_LENGTH] ^ (y >> 1);
			if ((y % 2) == 1)
			{
				MT[i] = MT[i] ^ CONSTANT_4;
			}
		}
	}

	unsigned mt_rand()
	{
		if (!RandGen::init)
		{
			initialize();
			RandGen::init = true;
		}

		unsigned int y;

		if (index == 0)
			GenNum();

		y = MT[index];
		y = y ^ (y >> 11);
		y = y ^ ((y << 7) & CONSTANT_2);
		y = y ^ ((y << 15) & CONSTANT_3);
		y = y ^ (y >> 18);

		index = (index + 1) % GEN_STATE_LENGTH;

		return y;
	}

public:

	RandGenMersenneTwister() :
		RandGen()
	{
	}

	RandGenMersenneTwister(long seed) :
		RandGen(seed)
	{
	}

	using RandGen::rand;

	// initialize random number generation
	void initialize()
	{
		int i;

		index = 0;
		MT[0] = seed;
		for (i = 1; i < ((int)GEN_STATE_LENGTH); i++)
		{
			MT[i] = ((CONSTANT_1 * (MT[i - 1] ^ (MT[i - 1] >> 30))) + i) & MASK_1;
		}
	}

	// generate random number
	int rand()
	{
		return mt_rand();
	}

	// generate random number between 0 and n-1
	int rand(int n)
	{
		if (n < 0)
			n *= (-1);

		return mt_rand() % n;
	}

	// random uniform between [0,1)
	double rand01()
	{
		return (double) mt_rand() / UINT_MAX;
	}
};
}

#endif /* RANDGENMersenneTwister_HPP_ */
