/*
 * Set.h
 *
 *  Created on: 13/03/2015
 *      Author: igor
 */

#ifndef OPTFRAME_SETINSPIRED_SET_H_
#define OPTFRAME_SETINSPIRED_SET_H_

class SetIterator
{
private:
	int begin, end;
public:

	// contiguous set iterator
	SetIterator(int _begin, int _end) :
			begin(_begin), end(_end)
	{
	}

	virtual ~SetIterator()
	{
	}

	// TODO: first, next, current, isDone
	virtual void first()
	{
	}

	virtual void next()
	{
	}

	virtual int current()
	{
		return -1;
	}

	virtual bool isDone()
	{
		return false;
	}
};

// set of values of type int
class Set
{
public:
	int begin, end;
	int range;

public:

	// empty set
	Set()
	{
		begin = 0;
		end = -1;
		range = 0;
	}

	// contiguous set
	Set(int _begin, int _end) :
			begin(_begin), end(_end), range(_end - _begin + 1)
	{
	}

	virtual ~Set()
	{
	}

	virtual bool inSet(int value)
	{
		return (value >= begin) && (value <= end);
	}

	virtual SetIterator& iterator()
	{
		return *new SetIterator(begin, end);
	}
};

#endif /* OPTFRAME_SETINSPIRED_SET_H_ */
