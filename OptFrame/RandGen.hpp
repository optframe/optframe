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
#include <cmath>
//#include <tgmath.h>

#include "Component.hpp"
#include "Action.hpp"
#include "ComponentBuilder.h"

#include<vector>

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
		seed = time(NULL);
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
			}
			while (w >= 1.0);

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

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandGenAction: public Action<R, ADS, DS>
{
public:

	virtual ~RandGenAction()
	{
	}

	virtual string usage()
	{
		string u;
		u.append("OptFrame:RandGen idx   setSeed   value\n");
		u.append("OptFrame:RandGen idx   getSeed   output_variable => long\n");
		u.append("OptFrame:RandGen idx   rand   output_variable => long\n");
		u.append("OptFrame:RandGen idx   rand_n   output_variable => long\n");
		u.append("OptFrame:RandGen idx   rand01   output_variable => double\n");
		u.append("OptFrame:RandGen idx   randG   output_variable => double");
		return u;
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(RandGen::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(RandGen::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "getSeed") || (action == "setSeed") || (action == "rand") || (action == "rand_n") || (action == "rand01") || (action == "randG");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		if (!handleComponent(type))
		{
			cout << "RandGenAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if (!comp)
		{
			cout << "RandGenAction::doCast error: NULL component '" << component << " " << id << "'" << endl;
			return false;
		}

		if (!Component::compareBase(comp->id(), type))
		{
			cout << "RandGenAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if (type == RandGen::idComponent())
		{
			final = (RandGen*) comp;
		}
		else
		{
			cout << "RandGenAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS, DS>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		//cout << "RandGen::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		RandGen* rg;
		hf.assign(rg, scanner.nextInt(), scanner.next());

		if (!rg)
			return false;

		if (!scanner.hasNext())
			return false;

		string action = scanner.next();

		if (!handleAction(action))
			return false;

		if (action == "setSeed")
		{
			if (!scanner.hasNext())
				return false;

			long seed = scanner.nextLong();
			rg->setSeed(seed);

			return true;
		}

		if (action == "getSeed")
		{
			long seed = rg->getSeed();
			stringstream ss;
			ss << seed;

			return Action<R, ADS, DS>::registerText(scanner, ss.str(), dictionary);
		}

		if (action == "rand")
		{
			int v = rg->rand();
			stringstream ss;
			ss << v;

			return Action<R, ADS, DS>::registerText(scanner, ss.str(), dictionary);
		}

		if (action == "rand_n")
		{
			if (!scanner.hasNext())
				return false;

			int limit = scanner.nextInt();

			int v = rg->rand(limit);
			stringstream ss;
			ss << v;

			return Action<R, ADS, DS>::registerText(scanner, ss.str(), dictionary);
		}

		if (action == "rand01")
		{
			double v = rg->rand01();
			stringstream ss;
			ss << v;

			return Action<R, ADS, DS>::registerText(scanner, ss.str(), dictionary);
		}

		if (action == "randG")
		{
			double v = rg->randG();
			stringstream ss;
			ss << v;

			return Action<R, ADS, DS>::registerText(scanner, ss.str(), dictionary);
		}

		// no action found!
		return false;
	}

};



template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class RandGenBuilder : public ComponentBuilder<R, ADS, DS>
{
public:
	virtual ~RandGenBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		if(!scanner.hasNext())
			return NULL;

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
		ss << ComponentBuilder<R, ADS, DS>::idComponent() << "RandGen";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* RANDGEN_HPP_ */
