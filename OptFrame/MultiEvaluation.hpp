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

#ifndef OPTFRAME_MULTI_EVALUATION_HPP_
#define OPTFRAME_MULTI_EVALUATION_HPP_

#include "Evaluation.hpp"

using namespace std;

namespace optframe
{

class MultiEvaluation: public Component
{
protected:
	vector<Evaluation*> vev;

public:

	MultiEvaluation()
	{
	}

	MultiEvaluation(Evaluation* ev)
	{
		vev.push_back(ev);
	}

	MultiEvaluation(const Evaluation& ev)
	{
		vev.push_back(&ev.clone());
	}

	MultiEvaluation(const vector<double>& vd)
	{
		for(unsigned i = 0; i < vd.size(); i++)
			vev.push_back(new Evaluation(vd[i]));
	}

	MultiEvaluation(const vector<Evaluation*>& _vev)
	{
		for(unsigned i = 0; i < _vev.size(); i++)
			vev.push_back(&_vev[i]->clone());
	}

	MultiEvaluation(const MultiEvaluation& mev)
	{
		for(unsigned i = 0; i < mev.vev.size(); i++)
			vev.push_back(&mev.vev[i]->clone());
	}

	virtual ~MultiEvaluation()
	{
		for(unsigned i = 0; i < vev.size(); i++)
			delete vev[i];
		vev.clear();
	}

	void addEvaluation(const Evaluation& ev)
	{
		vev.push_back(&ev.clone());
	}

	void addEvaluation(Evaluation* ev)
	{
		vev.push_back(ev);
	}


	unsigned size() const
	{
		return vev.size();
	}

	void erase(unsigned index)
	{
		delete vev.at(index);
		vev.at(index) = NULL;
		vev.erase(vev.begin() + index);
	}

	Evaluation& at(unsigned index)
	{
		return *vev.at(index);
	}

	const Evaluation& at(unsigned index) const
	{
		return *vev.at(index);
	}

	Evaluation& operator[](unsigned index)
	{
		return *vev[index];
	}

	const Evaluation& operator[](unsigned index) const
	{
		return *vev[index];
	}

	const vector<Evaluation*>& getVector() const
	{
		return vev;
	}

	vector<Evaluation*> getCloneVector() const
	{
		vector<Evaluation*> v_e;
		for(unsigned i=0; i<vev.size(); i++)
			v_e.push_back(&vev[i]->clone());
		return v_e;
	}

	bool sameValues(const MultiEvaluation& mev)
	{
		if(vev.size() != mev.vev.size())
			return false;

		for(unsigned i = 0; i < vev.size(); i++)
			if(vev[i]->evaluation() != mev.vev[i]->evaluation())
				return false;
		return true;
	}


	virtual MultiEvaluation& operator=(const MultiEvaluation& mev)
	{
		if(&mev == this) // auto ref check
			return *this;

		for(unsigned i = 0; i < mev.vev.size(); i++)
			this->vev.push_back(&mev.vev[i]->clone());

		return *this;
	}

	virtual MultiEvaluation& clone() const
	{
		return *new MultiEvaluation(*this);
	}

	void clearNoKill()
	{
		this->vev.clear();
	}

	void clear()
	{
		for(unsigned i=0; i<vev.size(); i++)
			delete vev[i];
		this->vev.clear();
	}

	virtual void print() const
	{
		cout << toString() << endl;
	}

	virtual string toString() const
	{
		stringstream ss;
		ss << "MultiEvaluation (" << vev.size() << "):";
		for(unsigned i = 0; i < vev.size(); i++)
			ss << vev[i]->toString() << endl;
		return ss.str();
	}

};

}

#endif /*OPTFRAME_MULTI_EVALUATION_HPP_*/
