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

	virtual void buildADS(const R& _r, ADS& _ads)
	{

	}

	virtual void updateADSNeighStatus(ADS& _ads, int k)
	{

	}


	static string idComponent()
	{
		return "OptFrame:UpdateADS";
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
