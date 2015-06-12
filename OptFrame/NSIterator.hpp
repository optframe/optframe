// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_NSITERATOR_HPP_
#define OPTFRAME_NSITERATOR_HPP_

#include "Move.hpp"

#include "Component.hpp"
#include "Action.hpp"

using namespace std;

namespace optframe
{

class IteratorOutOfBound
{
private:
	int id;
public:
	IteratorOutOfBound(int _id) :
			id(_id)
	{
	}

	int getId()
	{
		return id;
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class NSIterator: public Component
{
public:
	virtual ~NSIterator()
	{
	}

	virtual void first() = 0;
	virtual void next() = 0;
	virtual bool isDone() = 0;
	virtual Move<R, ADS>& current() = 0;

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":NSIterator";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif //OPTFRAME_NSITERATOR_HPP_
