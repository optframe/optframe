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

#ifndef TIMER_HPP
#define TIMER_HPP

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <stdlib.h>

#include "Component.hpp"
#include "Action.hpp"
#include "ComponentBuilder.h"

namespace optframe
{

class Timer : public Component
{
private:
   bool showMessageOnDestroy;

#ifdef WIN32
   LARGE_INTEGER frequency;
   LARGE_INTEGER tstart;
#else
   struct timeval tstart;
#endif

public:

   Timer(bool m = false) :
      showMessageOnDestroy(m)
   {
#ifdef WIN32
      QueryPerformanceFrequency(&frequency);
      QueryPerformanceCounter(&tstart);
#else
      struct timezone tz;
      gettimeofday(&tstart, &tz);
#endif
   }

   virtual ~Timer()
   {
      if(showMessageOnDestroy)
         printf("Spent time: %f secs\n", now());
   }

   double now()
   {
      return inSecs();
   }

   double inSecs()
   {
      return inMicroSecs() * 0.000001;
   }

   double inMilliSecs()
   {
      return inMicroSecs() * 0.001;
   }

   double inMicroSecs()
   {
      double start;
      double end;

#ifdef WIN32
      LARGE_INTEGER t;
      QueryPerformanceCounter(&t);

      start = tstart.QuadPart * (1000000.0 / frequency.QuadPart);
      end = t.QuadPart * (1000000.0 / frequency.QuadPart);
#else
      struct timeval t;
      struct timezone tz;
      gettimeofday(&t, &tz);

      start = ( tstart.tv_sec * 1000000.0 ) + tstart.tv_usec;
      end = ( t.tv_sec * 1000000.0 ) + t.tv_usec;
#endif

      return end - start;
   }

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":Timer";
		return ss.str();
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
class TimerAction: public Action<R, ADS, DS>
{
public:

	virtual ~TimerAction()
	{
	}

	virtual string usage()
	{
		string u;
		u.append("OptFrame:Timer idx   now   output_variable => double\n");
		u.append("OptFrame:Timer idx   inSecs   output_variable => double\n");
		u.append("OptFrame:Timer idx   inMilliSecs   output_variable => double\n");
		u.append("OptFrame:Timer idx   inMicroSecs   output_variable => double\n");
		return u;
	}

	virtual bool handleComponent(string type)
	{
		return Component::compareBase(Timer::idComponent(), type);
	}

	virtual bool handleComponent(Component& component)
	{
		return component.compatible(Timer::idComponent());
	}

	virtual bool handleAction(string action)
	{
		return (action == "now") || (action == "inSecs") || (action == "inMilliSecs") || (action == "inMicroSecs");
	}

	virtual bool doCast(string component, int id, string type, string variable, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& d)
	{
		if (!handleComponent(type))
		{
			cout << "TimerAction::doCast error: can't handle component type '" << type << " " << id << "'" << endl;
			return false;
		}

		Component* comp = hf.components[component].at(id);

		if (!comp)
		{
			cout << "TimerAction::doCast error: NULL component '" << component << " " << id << "'" << endl;
			return false;
		}

		if (!Component::compareBase(comp->id(), type))
		{
			cout << "TimerAction::doCast error: component '" << comp->id() << " is not base of " << type << "'" << endl;
			return false;
		}

		// remove old component from factory
		hf.components[component].at(id) = NULL;

		// cast object to lower type
		Component* final = NULL;

		if (type == Timer::idComponent())
		{
			final = (Timer*) comp;
		}
		else
		{
			cout << "TimerAction::doCast error: no cast for type '" << type << "'" << endl;
			return false;
		}

		// add new component
		Scanner scanner(variable);
		return ComponentAction<R, ADS, DS>::addAndRegister(scanner, *final, hf, d);
	}

	virtual bool doAction(string content, HeuristicFactory<R, ADS, DS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
		//cout << "Timer::doAction '" << content << "'" << endl;

		Scanner scanner(content);

		if (!scanner.hasNext())
			return false;

		Timer* timer;
		hf.assign(timer, scanner.nextInt(), scanner.next());

		if (!timer)
			return false;

		if (!scanner.hasNext())
			return false;

		string action = scanner.next();

		if (!handleAction(action))
			return false;

		if (action == "now")
		{
			double v = timer->now();
			stringstream ss;
			ss << v;

			return Action<R, ADS, DS>::registerText(scanner, ss.str(), dictionary);
		}

		if (action == "inSecs")
		{
			double v = timer->inSecs();
			stringstream ss;
			ss << v;

			return Action<R, ADS, DS>::registerText(scanner, ss.str(), dictionary);
		}

		if (action == "inMilliSecs")
		{
			double v = timer->inMilliSecs();
			stringstream ss;
			ss << v;

			return Action<R, ADS, DS>::registerText(scanner, ss.str(), dictionary);
		}

		if (action == "inMicroSecs")
		{
			double v = timer->inMicroSecs();
			stringstream ss;
			ss << v;

			return Action<R, ADS, DS>::registerText(scanner, ss.str(), dictionary);
		}
		// no action found!
		return false;
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class TimerBuilder : public ComponentBuilder<R, ADS, DS>
{
public:
	virtual ~TimerBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS, DS>& hf, string family = "")
	{
		return new Timer;
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == Timer::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << ComponentBuilder<R, ADS, DS>::idComponent() << "Timer";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* TIMER_HPP */
