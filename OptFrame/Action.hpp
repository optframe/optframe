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

#ifndef OPTFRAME_ACTION_HPP_
#define OPTFRAME_ACTION_HPP_

#include <cstdlib>
#include <iostream>
#include <vector>

#include "Scanner++/Scanner.h"

//#include "Module.hpp"

using namespace std;
using namespace scannerpp;

template<class R, class ADS, class DS> class HeuristicFactory;

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Action
{
public:

	virtual ~Action()
	{
	}

	virtual string usage() = 0;


	virtual bool handleComponent(string type) = 0;

	virtual bool handleComponent(OptFrameComponent& component) = 0;

	virtual bool handleAction(string action) = 0;

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary) = 0;

	static bool addAndRegister(Scanner& scanner, OptFrameComponent& comp, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		int index = hf.addComponent(comp);

		if (index == -1)
			return false;

		if (scanner.hasNext())
		{
			string varName = scanner.next();

			stringstream sscomp;
			sscomp << comp.id() << " " << index;

			d[varName] = sscomp.str(); // TODO: fix!!

			return true;
			//return Module<R, ADS, DS>::defineText(varName, sscomp.str(), d);
		}

		return true;
	}

	static string formatDouble(double d)
	{
		stringstream ss;
		ss << fixed;
		ss << d;
		return ss.str();
	}

	static string formatInt(int i)
	{
		stringstream ss;
		ss << i;
		return ss.str();
	}

	static string formatBool(bool b)
	{
		if(b)
			return "true";
		else
			return "false";
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentAction: public Action<R, ADS, DS>
{
public:

	virtual ~ComponentAction()
	{
	}

	virtual string usage()
	{
		return "OptFrame: idx  log  output_variable\nOptFrame: idx  print";
	}

	virtual bool handleComponent(string type)
	{
		return OptFrameComponent::compareBase(OptFrameComponent::idComponent(), type);
	}

	virtual bool handleComponent(OptFrameComponent& component)
	{
		return component.compatible(OptFrameComponent::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "log") || (action == "print");
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		//cout << "Evaluation::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		OptFrameComponent* c;
		hf.assign(c, scanner.nextInt(), scanner.next());

		if (!c)
			return false;

		if (!scanner.hasNext())
			return false;

		string action = scanner.next();

		if (!handleAction(action))
			return false;

		if (action == "log")
		{
			if (!scanner.hasNext())
				return false;

			string var = scanner.next();

			stringstream ss;
			ss << c->log();

			dictionary[var] = ss.str();

			return true;
		}

		if (action == "print")
		{
			c->print();

			return true;
		}

		// no action found!
		return false;
	}

};

}

#endif /* OPTFRAME_ACTION_HPP_ */
