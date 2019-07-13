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

#include <cstdio>
#include <cstdlib>
#include <sys/time.h>

class Timer 
{
protected:
	struct timeval time1, time2;
    long elapsed_utime;    /* elapsed time in microseconds */
    long elapsed_mtime;    /* elapsed time in milliseconds */
    long elapsed_seconds;  /* diff between seconds counter */
    long elapsed_useconds; /* diff between microseconds counter */

	bool showMessageOnDestroy;
public:
	Timer()
	{ 
		gettimeofday(&time1, nullptr); 
		showMessageOnDestroy=true; 
	};
	Timer(bool m)
	{ 
		gettimeofday(&time1, nullptr); 
		showMessageOnDestroy=m; 
	};
	virtual ~Timer(){ if(showMessageOnDestroy) printf("Elapsed time: %.3f secs\n", now()); };
	virtual double now() 
	{ 
		gettimeofday(&time2, nullptr);
	    elapsed_seconds  = time2.tv_sec  - time1.tv_sec;
	    elapsed_useconds = time2.tv_usec - time1.tv_usec;
	    elapsed_utime = (elapsed_seconds) * 1000000 + elapsed_useconds;
	    elapsed_mtime = ((elapsed_seconds) * 1000 + elapsed_useconds/1000.0) + 0.5;
		return ( elapsed_mtime / 1000 + ( elapsed_mtime % 1000 ) * 0.001 ) ; 
	};
};

/* Old version
#include <ctime>

class Timer 
{
protected:
	clock_t initialTime;
	bool showMessageOnDestroy;
public:
	Timer(){ initialTime=clock(); showMessageOnDestroy=true; };
	Timer(bool m){ initialTime=clock(); showMessageOnDestroy=m; };

	virtual ~Timer(){ if(showMessageOnDestroy) printf("Spent time: %.3f secs\n", (double) (clock() - initialTime) / CLOCKS_PER_SEC ); };
	virtual double now() { return ((double)(clock() - initialTime) / CLOCKS_PER_SEC) ; };
};*/

#endif /* TIMER_HPP */
