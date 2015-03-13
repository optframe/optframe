/*
 * SetUnion.h
 *
 *  Created on: 13/03/2015
 *      Author: igor
 */

#ifndef OPTFRAME_SETINSPIRED_SETUNION_H_
#define OPTFRAME_SETINSPIRED_SETUNION_H_

#include "Set.h"

class SetUnionIterator: public SetIterator
{
public:
	SetIterator& si1;
	SetIterator& si2;

public:

	SetUnionIterator(SetIterator& _si1, SetIterator& _si2) :
			si1(_si1), si2(_si2)
	{
	}

	// TODO: create methods first, next, current, isDone

};



class SetUnion
{
public:
	Set& s1;
	Set& s2;

	// DO NOT USE this.begin, this.end

public:

	SetUnion(Set& _s1, Set& _s2) :
			s1(_s1), s2(_s2)
	{
	}

	virtual ~SetUnion()
	{
	}

	virtual bool inSet(int value)
	{
		return s1.inSet(value) || s2.inSet(value);
	}

	virtual SetIterator& iterator()
	{
		return *new SetUnionIterator(s1.iterator, s2.iterator);
	}

};

#endif /* OPTFRAME_SETINSPIRED_SETUNION_H_ */
