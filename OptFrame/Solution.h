#ifndef SOLUTION_H_
#define SOLUTION_H_

template<class R>
class Solution
{
public:

	virtual Solution<R>* clone() = 0;

	virtual void print() = 0;

	virtual R* getRepresentation() = 0;
	virtual void setRepresentation(R*) = 0;

	virtual ~Solution<R>(){};

};

#endif /*SOLUTION_H_*/
