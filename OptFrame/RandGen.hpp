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

#ifndef OPTFRAME_RANDGEN_HPP_
#define OPTFRAME_RANDGEN_HPP_

#include <vector>
#include <math.h>
#include <time.h>
#include <cmath>
#include <unistd.h>
//#include <tgmath.h>

#include "Component.hpp"
#include "Action.hpp"
#include "ComponentBuilder.h"

#include <random> //used for Binomial distribution requested by NGES
//#include <tr1/random>

namespace optframe
{

/*
 // reuse of function 'rand()' by using function 'randgen_sys_rand()'
 unsigned int randgen_sys_rand()
 {
 return rand();
 }

 float system_log(float v)
 {
 return log(v);
 }
 */

class RandGen: public Component
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
		seed = time(nullptr);
		init = false;
		hasNextG = false;
		nextG = 0.0;
	}

	RandGen(long _seed) :
			seed(_seed)
	{
		if (seed < 0)
			seed *= (-1);
		init = false;
		hasNextG = false;
		nextG = 0.0;
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

		return (int) ::rand();
	}

	// random positive integer between 0 and n-1
	virtual int rand(int n)
	{
		if (!init)
		{
			initialize();
			init = true;
		}

		return ::rand() % n;
	}

	// randomized number between [i, j]
	virtual int rand(unsigned i, unsigned j)
	{
		int k = j - i + 1;
		int number = rand(k) + i;

		return number;
	}

	// random with probability 'p'
	virtual double randP(double p)
	{
		return (rand01() < p);
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
		return (double) ::rand() / RAND_MAX;
	}

	virtual int randBinomial(double p, int tries)
	{
		//std::tr1::variate_generator<std::tr1::mt19937, std::tr1::binomial_distribution<> > rngB(std::tr1::mt19937(123), std::tr1::binomial_distribution<>(tries, p));
		//int y= rngB();
		std::default_random_engine generator;
		std::binomial_distribution<int> distribution(tries, p);
		int y = distribution(generator);

		return y;
	}

	virtual int randBinomialWithNegative(double p, int tries)
	{
		//std::tr1::variate_generator<std::tr1::mt19937, std::tr1::binomial_distribution<> > rngB(std::tr1::mt19937(123), std::tr1::binomial_distribution<>(tries, p));
		//int y = rngB();
		int y = randBinomial(p, tries);

		int sign = this->rand(2);
		if (sign == 1)
			y *= -1;
		return y;
	}

	virtual double randG(double mean, double stdev)
	{
		return randG() * stdev + mean;
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

			w = sqrt((-2.0 * ::log(w)) / w);
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

	// sets the random generator seed
	void setSeed(long _seed)
	{
		seed = _seed;
		initialize();
	}

	unsigned long mix(unsigned long a, unsigned long b, unsigned long c)
	{
		a = a - b;
		a = a - c;
		a = a ^ (c >> 13);
		b = b - c;
		b = b - a;
		b = b ^ (a << 8);
		c = c - a;
		c = c - b;
		c = c ^ (b >> 13);
		a = a - b;
		a = a - c;
		a = a ^ (c >> 12);
		b = b - c;
		b = b - a;
		b = b ^ (a << 16);
		c = c - a;
		c = c - b;
		c = c ^ (b >> 5);
		a = a - b;
		a = a - c;
		a = a ^ (c >> 3);
		b = b - c;
		b = b - a;
		b = b ^ (a << 10);
		c = c - a;
		c = c - b;
		c = c ^ (b >> 15);
		return c;
	}

	unsigned long generateRandomSeed()
	{
		return mix(clock(), time(nullptr), getpid());
	}

	template<class T>
	void shuffle(vector<T>& v)
	{
		if (v.size() > 0)
			for (unsigned int i = 0; i < v.size() - 1; i++)
			{
				int x = i + rand(v.size() - i - 1) + 1;
				T elem = v.at(i);
				v.at(i) = v.at(x);
				v.at(x) = elem;
			}
	}

	static string idComponent()
	{
		return "OptFrame:RandGen";
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class RandGenBuilder: public ComponentBuilder<R, ADS>
{
public:
	virtual ~RandGenBuilder()
	{
	}

	virtual Component*
	buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
	{
		if (!scanner.hasNext())
			return nullptr;

		long seed = scanner.nextLong();

		return new RandGen(seed);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair("long", "seed"));
		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == RandGen::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS>::idComponent() << "RandGen";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* OPTFRAME_RANDGEN_HPP_ */
