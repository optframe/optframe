// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
      showMessageOnDestroy = true;
   };
   Timer(bool m)
   {
      gettimeofday(&time1, nullptr);
      showMessageOnDestroy = m;
   };
   virtual ~Timer()
   {
      if (showMessageOnDestroy)
         printf("Elapsed time: %.3f secs\n", now());
   };
   virtual double now()
   {
      gettimeofday(&time2, nullptr);
      elapsed_seconds = time2.tv_sec - time1.tv_sec;
      elapsed_useconds = time2.tv_usec - time1.tv_usec;
      elapsed_utime = (elapsed_seconds)*1000000 + elapsed_useconds;
      elapsed_mtime = ((elapsed_seconds)*1000 + elapsed_useconds / 1000.0) + 0.5;
      return (elapsed_mtime / 1000 + (elapsed_mtime % 1000) * 0.001);
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
