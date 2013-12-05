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

#ifndef OPTFRAME_MULTI_EVALUATION_HPP_
#define OPTFRAME_MULTI_EVALUATION_HPP_

#include "Evaluation.hpp"
#include "nevector.hpp"

using namespace std;

namespace optframe
{

template<class DS>
class MultiEvaluation: public nevector<Evaluation<DS>*>
{
	MultiEvaluation(const MultiEvaluation<DS>& mev) :
			nevector<DS>::v(mev.v)
	{
	}

	virtual ~MultiEvaluation()
	{
	}

	virtual MultiEvaluation<DS>& operator=(const MultiEvaluation<DS>& mev)
	{
		if (&mev == this) // auto ref check
			return *this;

		nevector<DS>::v = mev.v;

		return *this;
	}

	virtual MultiEvaluation<DS>& clone() const
	{
		return *new MultiEvaluation<DS>(*this);
	}

};

}

#endif /*OPTFRAME_MULTI_EVALUATION_HPP_*/
