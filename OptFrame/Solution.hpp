// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_SOLUTION_HPP_
#define OPTFRAME_SOLUTION_HPP_

#include <cstdlib>
#include <iostream>

using namespace std;

//! \english The Solution class is a container class for the Representation structure R. \endenglish \portuguese A classe Solution é uma classe contêiner para a Representação R. \endportuguese

/*!
  \english
  In the constructor, a copy of R is stored inside the Solution class.
  The getR() method returns a reference to the stored Representation R.
  \endenglish

  \portuguese
  No construtor, uma cópia de R é armazenada dentro da classe Solution.
  O método getR() retorna uma referência à Representação R armazenada.
  \endportuguese
*/

template<class R>
class Solution
{
protected:
	R& r;

public:
	Solution(R& rr):r(*new R(rr)){};
	Solution(const Solution<R>& s):r(*new R(s.r)){}

	virtual ~Solution() { delete &r; }

	void setR(const R& rr){ r = rr; }

	const R& getR() const {	return r; }
	R& getR() { return r; }

	virtual bool check() const
	{
	   return true;
	}

	virtual void print() const
	{
		cout << "Solution: "<< r << endl;
	}

	virtual Solution<R>& operator= (const Solution<R>& s)
	{
		if(&s == this) // auto ref check
			return *this;

		r = s.r;

		return *this;
	}

	virtual Solution<R>& clone() const
	{
		return * new Solution<R>(*this);
	}

};

#endif /* OPTFRAME_SOLUTION_HPP_ */
