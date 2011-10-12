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

#ifndef OPTFRAME_EVALUATOR_HPP_
#define OPTFRAME_EVALUATOR_HPP_

#include "Solution.hpp"
#include "Evaluation.hpp"
#include "Move.hpp"

#include <iostream>

#include "OptFrameComponent.hpp"

//#define EPSILON 0.0001

using namespace std;

//! \english The Evaluator class is responsible for the attribution of objective values for each Solution \endenglish \portuguese A classe Evaluator é responsável pela atribuição de valores objetivo para cada Solution \endportuguese

/*!
  \english
  The Evaluator class is responsible for the attribution of objective values for each Solution
  This is done by the method evaluate().
  \endenglish

  \portuguese
  A classe Evaluator é responsável pela atribuição de valores objetivo para cada Solution.
  Isto é feito através do método evaluate().
  \endportuguese
*/

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class Evaluator : public OptFrameComponent
{
public:

	virtual ~Evaluator()
	{
	}

	Evaluation<M>& evaluate(const Solution<R, ADS>& s)
	{
		return evaluate(s.getR());
	}

	virtual Evaluation<M>& evaluate(const R& r) = 0;

	void evaluate(Evaluation<M>& e, const Solution<R, ADS>& s)
	{
		evaluate(e, s.getR());
	}

	virtual void evaluate(Evaluation<M>& e, const R& r)
	{
		Evaluation<M>& e1 = evaluate(r);
		e = e1;
		delete &e1;
	}

	// Apply movement considering a previous evaluation => Faster.
	// Update evaluation 'e'
	Move<R, ADS, M>& applyMove(Evaluation<M>& e, Move<R, ADS, M>& m, Solution<R, ADS>& s)
	{
		Move<R, ADS, M>& rev = m.apply(e, s);
		evaluate(e, s);
		return rev;
	}

	// Apply movement without considering a previous evaluation => Slower.
	// Return new evaluation 'e'
	pair<Move<R, ADS, M>&, Evaluation<M>&>& applyMove(Move<R, ADS, M>& m, Solution<R, ADS>& s)
	{
		Move<R, ADS, M>& rev = m.apply(s);
		return *new pair<Move<R, ADS, M>&, Evaluation<M>&> (rev, evaluate(s));
	}

	// Movement cost based on reevaluation of 'e'
	double moveCost(Evaluation<M>& e, Move<R, ADS, M>& m, Solution<R, ADS>& s)
	{
		Move<R, ADS, M>& rev = applyMove(e, m, s);
		double e_end = e.evaluation();

		Move<R, ADS, M>& ini = applyMove(e, rev, s);
		double e_ini = e.evaluation();

		// Difference: new - original
		double diff = e_end - e_ini;

		delete &rev;
		delete &ini;

		return diff;
	}

	// Movement cost based on complete evaluation
	double moveCost(Move<R, ADS, M>& m, Solution<R, ADS>& s)
	{
		pair<Move<R, ADS, M>&, Evaluation<M>&>& rev = applyMove(m, s);

		pair<Move<R, ADS, M>&, Evaluation<M>&>& ini = applyMove(rev.first, s);

		// Difference: new - original
		double diff = rev.second.evaluation() - ini.second.evaluation();

		delete &rev.first;
		delete &rev.second;
		delete &ini.first;
		delete &ini.second;

		delete &rev;
		delete &ini;

		return diff;
	}

	// true if 's1' is better than 's2'
	bool betterThan(const Solution<R, ADS>& s1, const Solution<R, ADS>& s2)
	{
		Evaluation<M>& e1 = evaluate(s1);
		Evaluation<M>& e2 = evaluate(s2);

		double f1 = e1.evaluation();
		double f2 = e2.evaluation();

		delete &e1;
		delete &e2;

		return betterThan(f1, f2);
	}

	// true if 'e1' is better than 'e2'
	bool betterThan(const Evaluation<M>& e1, const Evaluation<M>& e2)
	{
		return betterThan(e1.evaluation(), e2.evaluation());
	}

	//! abstract method betterThan: true when a < b for minimization problems; and true when a > b for maximization problems.
	/*!
	 betterThan is the method in OptFrame used to guide the search methods into the solution space.
	 with betterThan the search methods are able to compare good and poor solutions, in one of the two directions: minimization and maximization.
	 It must be implemented by the final user in one of these ways:
	 - for minimization problems, returns a < b;
	 - for maximization problems, returns a > b.
	 */
	virtual bool betterThan(double a, double b) = 0;

   static string idComponent()
   {
      return "OptFrame:ev";
   }

   virtual string id() const
   {
      return idComponent();
   }

};

#endif /*OPTFRAME_EVALUATOR_HPP_*/
