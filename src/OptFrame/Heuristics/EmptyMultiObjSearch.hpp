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

#ifndef OPTFRAME_EMPTY_MULTI_OBJ_SEARCH_HPP_
#define OPTFRAME_EMPTY_MULTI_OBJ_SEARCH_HPP_

#include <math.h>
#include <vector>

#include "../MultiObjSearch.hpp"

namespace optframe
{

template<XSolution S, XEvaluation XEv = Evaluation<>>
class EmptyMultiObjSearch: public MultiObjSearch<S, XEv>
{
public:

	EmptyMultiObjSearch()
	{
	}

	virtual ~EmptyMultiObjSearch()
	{
	}

	Pareto<S, XEv>* search(MOSC& mosc, Pareto<S, XEv>* _pf = nullptr) override
	{
		cout << "WARNING: RETURNING A EmptyMultiObjSearch!" << endl;
		return nullptr;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << MultiObjSearch<S, XEv>::idComponent() << "empty";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_EMPTY_MULTI_OBJ_SEARCH_HPP_*/
