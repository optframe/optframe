// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef RANDGEN_HPP_
#define RANDGEN_HPP_

#include <math.h>
#include <time.h>

#include<vector>

// reuse of function 'rand()' by using function 'randgen_sys_rand()'
unsigned int randgen_sys_rand()
{
	return rand();
}

class RandGen
{
private:
	double nextG;
	bool hasNextG;

protected:
	long seed;
	bool init;

public:
	RandGen()
	{
		seed = time(NULL);
		init = false;
	}

	RandGen(long _seed) :
		seed(_seed)
	{
		if (seed < 0)
			seed *= (-1);
		init = false;
	}

	virtual ~RandGen()
	{
	}

	// initialize random number generation
	virtual void initialize()
	{
		srand(seed);
		hasNextG = false;
	}

	// random integer
	virtual int rand()
	{
		if (!init)
		{
			initialize();
			init = true;
		}
		// reuse of function 'rand()' by using function 'randgen_sys_rand()'
		return (int) randgen_sys_rand();
	}

	// random positive integer between 0 and n-1
	virtual int rand(int n)
	{
		if (!init)
		{
			initialize();
			init = true;
		}
		// reuse of function 'rand()' by using function 'randgen_sys_rand()'
		return randgen_sys_rand() % n;
	}

	// random uniform between [0,1)
	virtual double rand01()
	{
		if (!init)
		{
			initialize();
			init = true;
		}
		// reuse of function 'rand()' by using function 'randgen_sys_rand()'
		return (double) randgen_sys_rand() / RAND_MAX;
	}

	// random gaussian mean 0.0 stdev 1.0
	virtual double randG()
	{
		if (!init)
		{
			initialize();
			init = true;
		}

		if (hasNextG)
		{
			hasNextG = false;
			return nextG;
		}
		else
		{
			float x1, x2, w, y1, y2;
			do
			{
				x1 = 2.0 * rand01() - 1.0;
				x2 = 2.0 * rand01() - 1.0;
				w = x1 * x1 + x2 * x2;
			} while (w >= 1.0);

			w = sqrt((-2.0 * log(w)) / w);
			y1 = x1 * w;
			y2 = x2 * w;
			nextG = y2; // store y2
			hasNextG = true;
			return y1;
		}
	}

	// returns the random generator seed
	long getSeed()
	{
		return seed;
	}

	template<class T>
	void shuffle(vector<T>& v)
	{
	   if(v.size() > 0)
         for(unsigned int i = 0; i < v.size() - 1; i++)
         {
            int x = i + rand(v.size() - i - 1) + 1;
            T elem = v.at(i);
            v.at(i) = v.at(x);
            v.at(x) = elem;
         }
	}

};

#endif /* RANDGEN_HPP_ */
