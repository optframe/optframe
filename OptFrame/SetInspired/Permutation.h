/*
 * Permutation.h
 *
 *  Created on: 13/03/2015
 *      Author: igor
 */

#ifndef OPTFRAME_SETINSPIRED_PERMUTATION_H_
#define OPTFRAME_SETINSPIRED_PERMUTATION_H_

#include<vector>

#include "Set.h"

using namespace std;

class Permutation
{
public:
	Set& set;
	vector<int> p;

public:

	Permutation(Set& _set) :
			set(_set)
	{
		initialize();
	}

	// permutation
	void initialize()
	{
		p = vector<int>(set.range);
		for (int i = set.begin; i <= set.end; i++)
		{
			int j = i - set.begin;
			p[j] = i;
		}
	}

	void shuffle()
	{
		//std::shuffle(p.begin(), p.end());
	}

	vector<int>& getVector()
	{
		return p;
	}

	int at(int index)
	{
		return p.at(index);
	}

};

#endif /* OPTFRAME_SETINSPIRED_PERMUTATION_H_ */
