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

#include "Solution.hpp"

#include "Component.hpp"
//#include "Action.hpp"

using namespace std;

namespace optframe
{

// Local Optimum Status
enum LOS
{
    los_yes, los_no, los_unknown, los_partial
};

// exemplo: mochila NSSeq de incrementos (+1).
// Comeca 'unknown'.
// Se nao pode mais incrementar nada vira 'yes'.
// Se perturba um pouquinho vira 'no' (pq sabe como voltar).

// 'partial' indicates that the solution is partially in a local optimum (maybe the word semi-local optimum is better...)
// it may happen for example in a VRP, where a route is in LO while others are not.

// TODO: unused?
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

	virtual void firstValid(const Solution<R, ADS>& s)
	{
		first();

		while(!isDone())
		{
			// TODO: verify if it's not null!
			Move<R, ADS>& m = *current();
			if(m.canBeAppliedToSolution(s))
			{
				delete &m;
				break;
			}

			delete &m;
			next();
		}
	}

	virtual void next() = 0;

	virtual void nextValid(const Solution<R, ADS>& s)
	{
		next();

		while(!isDone())
		{
			// TODO: verify if it's not null!
			Move<R, ADS>& m = *current();
			if(m.canBeAppliedToSolution(s))
			{
				delete &m;
				break;
			}

			delete &m;
			next();
		}
	}

	virtual bool isDone() = 0;
	virtual Move<R, ADS>* current() = 0;

    // INSERT LOCAL OPTIMUM INFORMATION IN SOLUTION (IN ADS? USER DECIDES.)
    virtual void setLOS(LOS status, Solution<R, ADS>& s)
    {
    }

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
