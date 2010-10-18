#ifndef RUNNABLE_HPP_
#define RUNNABLE_HPP_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

void * _run(void * arg);

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

		if(e!=0)
			cout << "Error "<<e<<" destroying attr from thread." << endl;
		/*
		rc = pthread_join(thread, &status);
		if (rc)
		{
			printf("ERROR; return code from pthread_join() is %d\n", rc);
			exit(-1);
		}
		*/
		//printf("Completed join with thread %d status= %ld\n",t, (long)status);
		//pthread_exit(NULL);
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
		if(e!=0)
			cout << "Error "<<e<<" stopping thread!" << endl;
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
	void *status;
};

void * _run(void * arg)
{
	Runnable * runnable = (Runnable*) arg;
	runnable->run();
	runnable->finished_last_job = true;
	return NULL;
};

#endif /* RUNNABLE_HPP_ */
