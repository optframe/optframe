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

#ifndef OPTFRAME_CONSTRUCTIVE_H_
#define OPTFRAME_CONSTRUCTIVE_H_

#include "Solution.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Constructive : public OptFrameComponent
{
public:
	virtual ~Constructive()
	{
	}

	virtual Solution<R, ADS>& generateSolution() = 0;

    virtual bool compatible(string s)
    {
    	return ( s == idComponent() ) || (OptFrameComponent::compatible(s));
    }

	static string idComponent()
	{
		stringstream ss;
		ss << OptFrameComponent::idComponent() << "Constructive";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ConstructiveAction: public Action<R, ADS, DS>
{
public:

	virtual ~ConstructiveAction()
	{
	}

	virtual string usage()
	{
		return "OptFrame:Constructive idx  generateSolution  output_variable => OptFrame:Solution idx";
	}

	virtual bool handleComponent(OptFrameComponent& component)
	{
		return component.compatible(Constructive<R, ADS>::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "generateSolution");
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		//cout << "Constructive::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		Constructive<R, ADS>* c;
		hf.assign(c, scanner.nextInt(), scanner.next());

		if (!c)
			return false;

		if (!scanner.hasNext())
			return false;

		string action = scanner.next();

		if (!handleAction(action))
			return false;

		if (action == "generateSolution")
		{
			if (!scanner.hasNext())
				return false;

			Solution<R, ADS>& s = c->generateSolution();

			return Action<R, ADS, DS>::addAndRegister(scanner, s, hf, dictionary);
		}

		// no action found!
		return false;
	}

};

}

#endif /*OPTFRAME_CONSTRUCTIVE_H_*/
