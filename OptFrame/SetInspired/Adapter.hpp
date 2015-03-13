/*
 * Adapter.hpp
 *
 *  Created on: 13/03/2015
 *      Author: igor
 */

#ifndef OPTFRAME_SETINSPIRED_ADAPTER_HPP_
#define OPTFRAME_SETINSPIRED_ADAPTER_HPP_

#include "Set.h"
#include "Subset.h"
#include "SetUnion.h"
#include "Permutation.h"

template<class R, class T>
class Adapter
{
	virtual ~Adapter()
	{
	}

	virtual T& adapt(R& r) = 0;
	virtual const T& adapt(const R& r) = 0;
};

// TODO: use lambda functions!! C++11


template<class R>
class SetAdapter: public Adapter<R, Set>
{
	virtual ~SetAdapter()
	{
	}

	virtual Set& adapt(R&) = 0;
	virtual const Set& adapt(const R&) = 0;
};

template<class R>
class SubsetAdapter: public Adapter<R, Subset>
{
	virtual ~SubsetAdapter()
	{
	}

	virtual Subset& adapt(R&) = 0;
	virtual const Subset& adapt(const R&) = 0;
};

template<class R>
class SetUnionAdapter: public Adapter<R, SetUnion>
{
	virtual ~SetUnionAdapter()
	{
	}

	virtual SetUnion& adapt(R&) = 0;
	virtual const SetUnion& adapt(const R&) = 0;
};

template<class R>
class PermAdapter: public Adapter<R, Permutation>
{
	virtual ~PermAdapter()
	{
	}

	virtual Permutation& adapt(R&) = 0;
	virtual const Permutation& adapt(const R&) = 0;
};

#endif /* OPTFRAME_SETINSPIRED_ADAPTER_HPP_ */
