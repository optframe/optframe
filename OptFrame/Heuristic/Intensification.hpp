// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

#ifndef INTENSIFICATION_HPP_
#define INTENSIFICATION_HPP_

#include "../Heuristic.hpp"

template<class R,class M>
class Intensification : public Heuristic<R,M>
{
public:

	virtual void exec(Solution<R>& _s, double timelimit, double target_f) = 0;

	virtual void addSolution(const Solution<R>&) = 0;
};


#endif /*INTENSIFICATION_HPP_*/
