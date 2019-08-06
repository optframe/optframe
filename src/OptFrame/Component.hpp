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

#ifndef OPTFRAME_COMPONENT_HPP_
#define OPTFRAME_COMPONENT_HPP_

#include <cstdlib>
#include <iostream>
#include <vector>

#include "Scanner++/Scanner.h"
using namespace scannerpp;

using namespace std;

namespace optframe
{

// OPTFRAME ALGORITHM COMPREHENSION FLAG
#ifndef OPTFRAME_FAST
#define OPTFRAME_AC
#endif

struct Log
{
	stringstream data;

	void clear()
	{
		data.clear();
	}

	void append(string s)
	{
		data << s;
	}

	string log()
	{
		return data.str();
	}

	bool toFile(string file, bool append = true)
	{
		FILE* f;
		if(append)
			f = fopen(file.c_str(), "a");
		else
			f = fopen(file.c_str(), "w");

		if(!f)
			return false;

		fprintf(f, "%s", data.str().c_str());

		fclose(f);

		return true;
	}
};

class Component
{
public:

	Log* logdata;

	static bool safe_delete(Component* c)
	{
		if (c)
		{
			delete c;
			return true;
		}
		else
			return false;
	}

	static void safe_print(Component* c)
	{
		if(c)
			c->print();
		else
			cout << "nullptr Component" << endl;
	}

	void initializeLog()
	{
		logdata = new Log;
	}

	void destroyLog()
	{
		if(logdata)
		{
			delete logdata;
			logdata = nullptr;
		}
	}

	int verboseLevel;

	bool error;
	bool warning;
	bool information;
	bool debug, verbose; // same thing for 'debug' or 'verbose'

	// Mode: SILENT  = 0
	// |      0      |      1      |      2      |      3      |
	// |    error    |   warning   | information | debug/verb. |
	// |      -      |      -      |      -      |      -      |

	// Mode: ERROR = 1
	// |      0      |      1      |      2      |      3      |
	// |    error    |   warning   | information | debug/verb. |
	// |      x      |      -      |      -      |      -      |

	// Mode: WARNING = 2 (DEFAULT)
	// |      0      |      1      |      2      |      3      |
	// |    error    |   warning   | information | debug/verb. |
	// |      x      |      x      |      -      |      -      |

	// Mode: INFORMATION = 3
	// |      0      |      1      |      2      |      3      |
	// |    error    |   warning   | information | debug/verb. |
	// |      x      |      x      |      x      |      -      |

	// Mode: VERBOSE/DEBUG = 4
	// |      0      |      1      |      2      |      3      |
	// |    error    |   warning   | information | debug/verb. |
	// |      x      |      x      |      x      |      x      |

	Component()
	{
		setMessageLevel(2);
		//logdata = nullptr;
	}

	virtual ~Component()
	{
	}

	static string idComponent()
	{
		return "OptFrame";
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return (s == id()) || (s == idComponent()); // equal to this component or "OptFrame:" base
	}

	virtual string toString() const
	{
		return id();
	}

	virtual void print() const
	{
		cout << toString() << endl;
	}

	virtual Log* getLog()
	{
		return logdata;
	}

	virtual const Log* getLog() const
	{
		return logdata;
	}

	virtual string log() const
	{
		return id();
	}

	void setSilent()
	{
		setMessageLevel(0);
	}

	void setVerbose()
	{
		setMessageLevel(4);
	}

	void setMessageLevel(int vl)
	{
		error = warning = information = verbose = debug = false;

		verboseLevel = vl;
		switch (verboseLevel)
		{
		case 0: // SILENT
			break;
		case 1: // ERROR
			error = true;
			break;
		case 3: // INFORMATION
			information = true;
			break;
		case 4: // VERBOSE
			debug = verbose = true;
			break;
		default: // 2: WARNING (default)
			warning = true;
		}

		debug = verbose;
		error = error || warning || information || debug;
		warning =  warning || information || debug;
		information =  information || debug;
	}

	bool getVerboseLevel()
	{
		return verboseLevel;
	}

	// taken from HeuristicFactory

	//! \english compareBase is an auxiliar method to compare a pattern to a component id. Check if 'component' inherits from 'base'. \endenglish \portuguese compareBase eh um metodo auxiliar para comparar um padrao a um id de componente. Testa se 'component' herda de 'base'. \endportuguese
	/*!
	 \sa compareBase(string, string)
	 */

	// Check if 'base' is inherited by 'component'
	// EXAMPLE: compareBase("OptFrame:", "OptFrame:Evaluator") returns TRUE!
	static bool compareBase(string _base, string _component)
	{
		if ((_base.length() < 3) || (_component.length() < 3))
		{
			cout << "Component::compareBase warning: comparing less than 3 characters! with base='" << _base << "' component='" << _component << "'" << endl;
			return false;
		}

		bool baseIsList = (_base.at(_base.length() - 2) == '[') && (_base.at(_base.length() - 1) == ']');
		bool componentIsList = (_component.at(_component.length() - 2) == '[') && (_component.at(_component.length() - 1) == ']');

		if (baseIsList != componentIsList)
			return false;

		// remove list (if exists)
		string base = typeOfList(_base);
		string component = typeOfList(_component);

		bool sameBase = true;

		if (base.length() <= component.length())
		{
			for (unsigned i = 0; i < base.length(); i++)
				if (base.at(i) != component.at(i))
					sameBase = false;
		}
		else
			sameBase = false;

		if (sameBase)
			return true;

		// ------------------
		// check last family
		// ------------------

		Scanner scanner(base);
		scanner.useSeparators(":");

		string family = scanner.next();
		while (scanner.hasNext())
			family = scanner.next();

		Scanner scanComponent(component);
		scanComponent.useSeparators(":");
		string part;
		while (scanComponent.hasNext())
		{
			part = scanComponent.next();
			if (part == family)
				sameBase = true;
		}

		return sameBase;
	}

	static string typeOfList(string listId)
	{
		Scanner scanner(listId);
		scanner.useSeparators(" \t\n[]");

		return scanner.next();
	}

};

}

#endif /* OPTFRAME_COMPONENT_HPP_ */
