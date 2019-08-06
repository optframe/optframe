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

#ifndef OPTFRAME_ITERATORVVSWAPINTRA_HPP_
#define OPTFRAME_ITERATORVVSWAPINTRA_HPP_

// Framework includes
#include "../../NSIterator.hpp"

#include "../Moves/MoveVVSwapIntra.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class DS = OPTFRAME_DEFAULT_EMEMORY>
class NSIteratorVVSwapIntra: public NSIterator<vector<vector<T> > , DS >
{
	typedef vector<vector<T> > Routes;
	const Routes& routes;

	int m_i, m_j, m_k;
public:

	NSIteratorVVSwapIntra(const Routes& r) :
		routes(r)
	{
	}

	virtual ~NSIteratorVVSwapIntra()
	{
	}

	void first()
	{
		// initialize vars
		m_i = 0;
		m_j = 0;
		m_k = 0;

		// go to a valid move
		next();
	}

	void next()
	{
		int n2 = routes.at(m_i).size();

		if (m_k < n2 - 1)
			m_k++;
		else if (++m_j < n2 - 1)
		{
			m_k = m_j + 1;
		}
		else
		{
			m_i++;
			m_j = 0;
			m_k = 0;
		}
	}

	bool isDone()
	{
		int n1 = routes.size();
		int n2 = routes.at(m_i).size();

		if ((m_i >= n1 - 1) && (m_j >= n2 - 2) && (m_k >= n2 - 1))
			return true;
		else
			return false;
	}

	Move<Routes, DS >& current()
	{
		return *new MoveVVSwapIntra<T, DS > (m_i, m_j, m_k);
	}
};

#endif /*OPTFRAME_ITERATORVVSWAPINTRA_HPP_*/
