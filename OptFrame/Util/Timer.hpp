#ifndef TIMER_HPP
#define TIMER_HPP

#include <cstdio>
#include <cstdlib>
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
};

#endif /* TIMER_HPP */
