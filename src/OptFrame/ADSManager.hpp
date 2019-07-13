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

#ifndef OPTFRAME_ADSMANAGER_HPP_
#define OPTFRAME_ADSMANAGER_HPP_

#include <cstdlib>
#include <iostream>

#include "Component.hpp"
#include "Solution.hpp"

using namespace std;

namespace optframe
{

template<class R, class ADS>
class ADSManager: public Component
{

public:
	ADSManager()
	{
	}

	virtual ~ADSManager()
	{

	}

	virtual void initializeADS(const R& rep, ADS& _ads) = 0;

	virtual void initializeADSNeighStructure(const R& rep, ADS& _ads) = 0;

	virtual void setNeighLocalOptimum(const R& rep, ADS& _ads, string str) = 0;

	virtual void setNeighLocalOptimum(Solution<R, ADS>& s, string str)
	{
		setNeighLocalOptimum(s.getR(), s.getADS(), str);
	}

	virtual bool compareADS(const ADS& _ads1, const ADS& _ads2) = 0;

	virtual void printADS(const ADS& _ads) = 0;

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (Component::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << "ADSManager";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "ADSManager";
		return ss.str();
	}

};

}

#endif /* OPTFRAME_ADSMANAGER_HPP_ */
