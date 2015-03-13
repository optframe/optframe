/*
 * Subset.h
 *
 *  Created on: 13/03/2015
 *      Author: igor
 */

#ifndef OPTFRAME_SETINSPIRED_SUBSET_H_
#define OPTFRAME_SETINSPIRED_SUBSET_H_

#include <vector>

#include "Set.h"

using namespace std;


class SubsetIterator : public SetIterator
{
public:
	vector<int>& data;
public:

	SubsetIterator(int _begin, int _end, vector<int>& _data) :
			data(_data)
	{
		begin = _begin;
		end = _end;
	}

	// TODO: create methods first, next, isDone, current

};

class Subset: public Set
{
public:
	Set& set;
	vector<bool> elements;

public:

	Subset(Set& _set) :
			set(_set)
	{
		begin = set.begin;
		end = set.end;
		range = set.range;

		elements = vector<int>(range, true);
	}

	virtual ~Subset()
	{
	}

	virtual bool inSet(int value)
	{
		int v = value - begin;
		if((v < 0) || (v >= range))
			return false;
		return elements[v];
	}

	virtual void remove(int value)
	{
		if(!inSet(value)) // ERROR
		{
			cout << "ERROR: REMOVING ELEMENT " << value << " WHICH IS NOT IN SUBSET!" << endl;
			exit(1);
		}

		int v = value - begin;
		elements[v] = false;
	}

	virtual void add(int value)
	{
		int v = value - begin;
		if((v < 0) || (v >= range)) // ERROR
		{
			cout << "ERROR: ADDING ELEMENT " << value << " WHICH IS NOT IN LIMITS SUBSET!" << endl;
			exit(1);
		}

		if(elements[v]) // ERROR
		{
			cout << "ERROR: ADDING ELEMENT " << value << " WHICH IS ALREADY IN SUBSET!" << endl;
			exit(1);
		}

		elements[v] = true;
	}

	virtual SetIterator& iterator()
	{
		return * new SubsetIterator(begin, end, elements);
	}

};

#endif /* OPTFRAME_SETINSPIRED_SUBSET_H_ */
