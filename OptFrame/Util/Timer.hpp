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
