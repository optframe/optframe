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

#ifndef OPTFRAME_UPDATEADS_HPP_
#define OPTFRAME_UPDATEADS_HPP_

#include <cstdlib>
#include <iostream>

#include "OptFrameComponent.hpp"

using namespace std;

template<class R, class ADS>
class UpdateADS: public OptFrameComponent
{

public:
	UpdateADS()
	{
	}

	virtual ~UpdateADS()
	{

	}

	virtual void initializeADS(const R& rep, ADS& _ads) = 0;

	virtual void initializeADSNeighStatus(const R& rep, ADS& _ads) = 0;

	virtual void updateADSNeighStatus(const R& rep, ADS& _ads, string str) = 0;

	virtual void updateADSNeighStatus(Solution<R, ADS>& s, string str)
	{
		updateADSNeighStatus(s.getR(), s.getADS(), str);
	}

	virtual bool compareADS(const ADS& _ads1, const ADS& _ads2) = 0;

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (OptFrameComponent::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << OptFrameComponent::idComponent() << "UpdateADS";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "Update ADS ";
		return ss.str();
	}

};

#endif /* OPTFRAME_UPDATEADS_HPP_ */
