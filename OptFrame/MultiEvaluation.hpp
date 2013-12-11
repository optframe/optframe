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

using namespace std;

namespace optframe
{

template<class DS = OPTFRAME_DEFAULT_DS>
class MultiEvaluation: public Component //nevector<Evaluation<DS>*>
{
protected:
	vector<Evaluation<DS>*> vev;

public:

	MultiEvaluation(Evaluation<DS>& ev)
	{
		vev.push_back(&ev);
	}

	MultiEvaluation(const vector<Evaluation<DS>*>& _vev) :
			vev(_vev)
	{
	}

	MultiEvaluation(const MultiEvaluation<DS>& mev)
	{
		vev = mev.vev;
	}

	virtual ~MultiEvaluation()
	{
	}

	void addEvaluation(Evaluation<DS>& ev)
	{
		vev.push_back(&ev);
	}

	unsigned size()
	{
		return vev.size();
	}

	void erase(unsigned index)
	{
		if (vev.size() > 1)
			vev.erase(vev.begin() + index);
		else
		{
			cerr << "MultiEvaluation::erase error: not enough elements do delete (" << vev.size() << ")";
			cerr << endl;
			exit(1); // TODO: throw exception
		}
	}

	Evaluation<DS>& at(unsigned index)
	{
		return *vev.at(index);
	}

	const Evaluation<DS>& at(unsigned index) const
	{
		return *vev.at(index);
	}

	Evaluation<DS>& operator[](unsigned index)
	{
		return vev[index];
	}

	const Evaluation<DS>& operator[](unsigned index) const
	{
		return vev[index];
	}


	const vector<Evaluation<DS>*>& getVector() const
	{
		return vev;
	}

	virtual MultiEvaluation<DS>& operator=(const MultiEvaluation<DS>& mev)
	{
		if (&mev == this) // auto ref check
			return *this;

		//this->v = mev.v;
		this->vev = mev.vev;

		return *this;
	}

	virtual MultiEvaluation<DS>& clone() const
	{
		return *new MultiEvaluation<DS>(*this);
	}

};

}

#endif /*OPTFRAME_MULTI_EVALUATION_HPP_*/
