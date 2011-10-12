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

#ifndef OPTFRAME_ILSLPerturbation_HPP_
#define OPTFRAME_ILSLPerturbation_HPP_

#include <math.h>
#include <vector>

#include "../NS.hpp"
#include "../RandGen.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ILSLPerturbation : public OptFrameComponent
{
public:
	virtual void perturb(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f, int level) = 0;

   static string idComponent()
   {
      return "OptFrame:ILS:ilsl_pert";
   }

   virtual string id() const
   {
      return idComponent();
   }
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ILSLPerturbationLPlus2: public ILSLPerturbation<R, ADS, M>
{
private:
	vector<NS<R, ADS, M>*> ns;
	Evaluator<R, ADS, M>& evaluator;
	int pMax;
	RandGen& rg;

public:
	ILSLPerturbationLPlus2(Evaluator<R, ADS, M>& e, int _pMax, NS<R, ADS, M>& _ns, RandGen& _rg) :
		evaluator(e), pMax(_pMax), rg(_rg)
	{
		ns.push_back(&_ns);
	}

	void add_ns(NS<R, ADS, M>& _ns)
	{
		ns.push_back(&_ns);
	}

	void perturb(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f, int level)
	{
		int f = 0; // number of failures
		int a = 0; // number of appliable moves

		level += 2; // level 0 applies 2 moves

		while ((a < level) && (f < pMax))
		{
			int x = rg.rand(ns.size());

			Move<R, ADS, M>& m = ns[x]->move(s);

			if (m.canBeApplied(e, s))
			{
				a++;
				delete &m.apply(e, s);
			}
			else
				f++;

			delete &m;
		}

		if (f == pMax)
			cout << "ILS Warning: perturbation had no effect in " << pMax << " tries!" << endl;

		evaluator.evaluate(e, s); // updates 'e'
	}

   static string idComponent()
   {
      return "OptFrame:ILS:ilsl_pert_lp2";
   }

   virtual string id() const
   {
      return idComponent();
   }
};

#endif /*OPTFRAME_ILSLPerturbation_HPP_*/
