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

#ifndef OPTFRAME_ELITISM_HPP_
#define OPTFRAME_ELITISM_HPP_

#include "MOSIndividual.hpp"
#include "../../Component.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Elitism: Component
{
public:

	Elitism()
	{
	}

	virtual ~Elitism()
	{
	}

	// keep archive updated
	virtual void updateArchive(const vector<const MOSIndividual<R, ADS, DS>*>& P, vector<MOSIndividual<R, ADS, DS>*>& archive) = 0;

	virtual void print() const
	{
		cout << "Elitism" << endl;
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NoArchiving: public Elitism<R, ADS, DS>
{
public:

	NoArchiving()
	{
	}

	virtual ~NoArchiving()
	{
	}

	void updateArchive(const vector<const MOSIndividual<R, ADS, DS>*>& P, vector<MOSIndividual<R, ADS, DS>*>& archive)
	{
		// DO ANYTHING! NSGA-II STYLE
	}

	virtual void print() const
	{
		cout << "NoArchiving" << endl;
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BoundedArchiving: public Elitism<R, ADS, DS>
{
protected:
	unsigned limit;
	MultiDirection<DS>& mDir;
public:

	BoundedArchiving(unsigned _limit, MultiDirection<DS>& _mDir) :
			limit(_limit), mDir(_mDir)
	{
	}

	virtual ~BoundedArchiving()
	{
	}

	virtual void updateArchive(const vector<const MOSIndividual<R, ADS, DS>*>& P, vector<MOSIndividual<R, ADS, DS>*>& archive)
	{
		// USE DOMINANCE TO KEEP ARCHIVING SUBJECT TO LIMIT
	}

	virtual void print() const
	{
		cout << "BoundedArchiving(" << limit << ")" << endl;
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class UnboundedArchiving: public Elitism<R, ADS, DS>
{
protected:
	MultiDirection<DS>& mDir;
public:

	UnboundedArchiving(MultiDirection<DS>& _mDir) :
			mDir(_mDir)
	{
	}

	virtual ~UnboundedArchiving()
	{
	}

	virtual void updateArchive(const vector<const MOSIndividual<R, ADS, DS>*>& P, vector<MOSIndividual<R, ADS, DS>*>& archive)
	{
		// USE DOMINANCE TO KEEP ARCHIVING
	}

	virtual void print() const
	{
		cout << "UnboundedArchiving" << endl;
	}
};

}

#endif /*OPTFRAME_ELITISM_HPP_*/
