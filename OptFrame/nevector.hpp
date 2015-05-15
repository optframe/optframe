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

#ifndef OPTFRAME_NON_EMPTY_VECTOR_HPP_
#define OPTFRAME_NON_EMPTY_VECTOR_HPP_

#include <vector>

using namespace std;

namespace optframe
{

template<class T>
class nevector
{
protected:
	vector<T> v;

public:
	nevector(const T t)
	{
		v.push_back(t);
	}


	nevector(const T discard, const vector<T>& _v)
	{
		if (_v.size() > 0)
			v = _v;
		else
			v.push_back(discard);
	}

	nevector(const nevector<T>& nev) :
			v(nev.v)
	{
	}

	virtual ~nevector()
	{
	}

	void push_back(T t)
	{
		v.push_back(t);
	}

	unsigned size()
	{
		return v.size();
	}

	void erase(unsigned index)
	{
		if (v.size() > 1)
			v.erase(v.begin() + index);
		else
		{
			cerr << "nevector::erase error: not enough elements do delete (" << v.size() << ")";
			cerr << endl;
			exit(1); // TODO: throw exception
		}
	}

	T& at(unsigned index)
	{
		return v.at(index);
	}

	const T& at(unsigned index) const
	{
		return v.at(index);
	}

	T& operator[](unsigned index)
	{
		return v[index];
	}

	const T& operator[](unsigned index) const
	{
		return v[index];
	}


	const vector<T>& getVector() const
	{
		return v;
	}

	virtual nevector<T>& operator=(const nevector<T>& nev)
	{
		if (&nev == this) // auto ref check
			return *this;

		v = nev.v;

		return *this;
	}

	virtual nevector<T>& clone() const
	{
		return *new nevector<T>(*this);
	}
};

}

#endif /*OPTFRAME_NON_EMPTY_VECTOR_HPP_*/
