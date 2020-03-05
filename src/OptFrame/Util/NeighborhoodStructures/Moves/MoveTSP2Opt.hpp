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

#ifndef OPTFRAME_MOVETSP2OPT_HPP_
#define OPTFRAME_MOVETSP2OPT_HPP_

// Framework includes
#include "../../../Move.hpp"

#include "../BaseSolutionTSP.hpp"


//using namespace std;
//using namespace optframe; (?????????????????) Don't use namespace declarations in headers

// Working structure: vector<T>

namespace optframe
{

//template<class T, class ADS = OPTFRAME_DEFAULT_ADS>
template<class T, class ADS = OPTFRAME_DEFAULT_ADS, XBaseSolution<vector<T>,ADS> S = CopySolution<vector<T>,ADS>, XEvaluation XEv = Evaluation<>>
class MoveTSP2Opt: public Move<S, XEv>
{
	typedef vector<T> Route;

protected:
	int p1, p2; // position 1 and position 2, respectively

	OPTFRAME_DEFAULT_PROBLEM* problem;

public:

	MoveTSP2Opt(int _p1, int _p2, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
			p1(_p1), p2(_p2), problem(_problem)
	{
	}

	virtual ~MoveTSP2Opt()
	{
	}

	int get_p1() const
	{
		return p1;
	}

	int get_p2() const
	{
		return p2;
	}

	virtual bool canBeApplied(const S& s) override
	{
      const Route& rep = s.getR();
		bool all_positive = (p1 >= 0) && (p2 >= 0);
		bool less = (p1 < p2);
		return all_positive && (rep.size() >= 2) && less;
	}

	virtual uptr<Move<S, XEv>> apply(S& s) override
	{
      Route& rep = s.getR();
		reverse(rep.begin() + p1, rep.begin() + p2);

		// r1->r1, r2->r2, e1->i1, e2->i2, n1->n2, n2->n1, i1->e1, i2->e2
		return uptr<Move<S, XEv>>(new MoveTSP2Opt(p1, p2));
	}

	virtual bool operator==(const Move<XES, XEv>& _m) const
			{
		const MoveTSP2Opt& m1 = (const MoveTSP2Opt&) _m;
		return ((m1.p1 == p1) && (m1.p2 == p2));
	}

	static string idComponent()
	{
		string idComp = Move<S, XEv>::idComponent();
		idComp.append("MoveTSP2Opt");
		return idComp;
	}

	virtual void print() const
	{
		cout << "MoveTSP2Opt( ";
		cout << " edge " << p1 << " <=>  edge " << p2 << " )";
		cout << endl;
	}
};

} // namespace optframe

#endif /*OPTFRAME_MOVETSP2OPT_HPP_*/
