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


template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class TimerBuilder : public ComponentBuilder<R, ADS>
{
public:
	virtual ~TimerBuilder()
	{
	}

	virtual Component* buildComponent(Scanner& scanner, HeuristicFactory<R, ADS>& hf, string family = "")
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
		ss << ComponentBuilder<R, ADS>::idComponent() << "Timer";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /* TIMER_HPP */
