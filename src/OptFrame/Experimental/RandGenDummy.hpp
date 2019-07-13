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

#ifndef RANDGENDummy_HPP_
#define RANDGENDummy_HPP_

#include "../../RandGen.hpp"

class RandGenDummy: public RandGen
{
private:
	unsigned int i;
	int inc;

	unsigned int d_rand()
	{
		if (!RandGen::init)
		{
			initialize();
			RandGen::init = true;
		}

		i += inc;

		return i;
	}

public:

	RandGenDummy() :
		RandGen()
	{
		inc = 1;
	}

	RandGenDummy(long seed, int _inc = 1) :
		inc(_inc), RandGen(seed)
	{
	}

	using RandGen::rand;

	// initialize random number generation
	void initialize()
	{
		i = RandGen::seed;
	}

	// generate random number
	int rand()
	{
		return d_rand();
	}

	// generate random number between 0 and n-1
	int rand(int n)
	{
		return d_rand() % n;
	}

	// random uniform between [0,1)
	double rand01()
	{
		return rand(1000) / 1000.0;
	}

	// random gaussian mean 0.0 stdev 1.0
	virtual double randG()
	{
		float x, w, y;
		float x_, _x, w_; // dummy version!
		do
		{
			x = 2.0 * rand01() - 1.0;
			w = x * x + x * x;

			// ==============================
			// to avoid infinite loop
			// create another crazy number!
			// ==============================
			x_ = (rand(200) / 100.0) - 1.0;
			_x = ((200 - rand(200)) / 100.0) - 1.0;
			w_ = (x_ * x_ + _x * _x);
			if ((w >= 1.0) && (w_ < w))
			{
				w = w_;
				x = _x; // or x_
			}
			// ==============================

		} while (w >= 1.0);

		w = sqrt((-2.0 * log(w)) / w);
		y = x * w;

		return y;
	}
};

#endif /* RANDGENDummy_HPP_ */
