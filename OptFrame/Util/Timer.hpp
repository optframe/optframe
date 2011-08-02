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

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/resource.h>

class Timer 
{
protected:
	double initialTime;

	bool showMessageOnDestroy;

	double get_clock_sec(void)
	{  struct timeval t;
		struct timezone tz;
		gettimeofday(&t, &tz);
		return (double) t.tv_sec + (double) t.tv_usec * 1E-6;
	};

public:
	Timer(){ initialTime=get_clock_sec(); showMessageOnDestroy=true; };
	Timer(bool m){ initialTime=get_clock_sec(); showMessageOnDestroy=m; };
	virtual ~Timer(){ if(showMessageOnDestroy) printf("Spent time: %f secs\n", get_clock_sec() -initialTime) ; };
	virtual double now() { return (get_clock_sec() - initialTime) ;  };

};

#endif /* TIMER_HPP */
