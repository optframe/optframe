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

#ifndef OPTFRAME_MULTI_NS_HPP_
#define OPTFRAME_MULTI_NS_HPP_

#include "../Move.hpp"
#include "../NS.hpp"
#include "../Solution.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiMove: public Move<R, ADS, DS>
{
public:
	vector<Move<R, ADS, DS>*> vm;

	MultiMove(vector<Move<R, ADS, DS>*>& _vm) :
			vm(_vm)
	{
	}

	virtual ~MultiMove()
	{
		for(unsigned i = 0; i < vm.size(); i++)
			delete vm[i];
		vm.clear();
	}

	virtual bool canBeApplied(const R& r, const ADS& ads)
	{
		for(unsigned i = 0; i < vm.size(); i++)
			if(!vm[i]->canBeApplied(r, ads))
				return false;
		// MAY STILL BE UNAPPLIABLE!! SOLVE THIS WITH RETURN nullptr IN APPLY?? TODO
		// OR... return an UNKNOWN state here (may be pointer, but better a tri-state element).
		return true;
	}

	virtual Move<R, ADS, DS>& apply(R& r, ADS& ads)
	{
		vector<Move<R, ADS, DS>*> vrev;
		for(unsigned i = 0; i < vm.size(); i++)
			vrev.push_back(&vm[i]->apply(r, ads));
		return *new MultiMove<R, ADS, DS>(vrev);
	}

	virtual bool operator==(const Move<R, ADS, DS>& m) const
	{
		cout << "STUB IN MultiMove: TODO IMPLEMENT operator==" << endl;
		return true; // STUB
	}

	static string idComponent()
	{
		return "OptFrame:Move:MultiMove";
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual void print() const
	{
		cout << "MultiMove(" << vm.size() << "):";
		for(unsigned i = 0; i < vm.size(); i++)
			vm[i]->print();
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MultiNS: public NS<R, ADS, DS>
{
public:

	RandGen& rg;
	NS<R, ADS, DS>& ns;
	unsigned kMin, kMax;

	MultiNS(RandGen& _rg, NS<R, ADS, DS>& _ns, unsigned _kMin, unsigned _kMax) :
			rg(_rg), ns(_ns), kMin(_kMin), kMax(_kMax)
	{
	}

	virtual ~MultiNS()
	{
	}

protected:
	virtual Move<R, ADS, DS>& move(const R& r, const ADS& ads)
	{
		vector<Move<R, ADS, DS>*> vm;
		unsigned k = rg.rand(kMin, kMax);
                //cout << "K=" << k << endl;
		for(unsigned i = 0; i < k; i++)
			vm.push_back(&ns.move(r, ads));
                //cout << "created move!" << endl;
		return *new MultiMove<R, ADS, DS>(vm);
	}

public:
	static string idComponent()
	{
		return "OptFrame:NS:MultiNS";
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (NS<R, ADS, DS>::compatible(s));
	}
};

}

#endif /*OPTFRAME_MULTI_NS_HPP_*/
