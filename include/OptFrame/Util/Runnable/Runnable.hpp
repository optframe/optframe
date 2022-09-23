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

#ifndef RUNNABLE_HPP_
#define RUNNABLE_HPP_

#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void*
_run(void* arg);

class Runnable
{
public:
   bool finished_last_job;

   Runnable()
   {
      // Initialize and set thread detached attribute
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

      finished_last_job = true;
   };

   virtual ~Runnable()
   {
      int e = pthread_attr_destroy(&attr);

      if (e != 0)
         cout << "Error " << e << " destroying attr from thread." << endl;
      /*
		rc = pthread_join(thread, &status);
		if (rc)
		{
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
		*/
      //printf("Completed join with thread %d status= %ld\n",t, (long)status);
      //pthread_exit(nullptr);
   }

   virtual void run() = 0;

   void start()
   {
      finished_last_job = false;
      rc = pthread_create(&thread, &attr, _run, this);
   };

   void stop()
   {
      int e = pthread_cancel(thread);
      if (e != 0)
         cout << "Error " << e << " stopping thread!" << endl;
      else
         finished_last_job = true;
   };

   bool finished()
   {
      return finished_last_job;
   }

protected:
   pthread_t thread;
   pthread_attr_t attr;
   int rc, t;
   void* status;
};

void*
_run(void* arg)
{
   Runnable* runnable = (Runnable*)arg;
   runnable->run();
   runnable->finished_last_job = true;
   return nullptr;
};

#endif /* RUNNABLE_HPP_ */
