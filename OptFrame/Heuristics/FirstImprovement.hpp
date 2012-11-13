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

#ifndef OPTFRAME_FI_HPP_
#define OPTFRAME_FI_HPP_

#include "../LocalSearch.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class FirstImprovement: public LocalSearch<R, ADS, M>
{
private:
	Evaluator<R, ADS, M>& eval;
	NSSeq<R, ADS, M>& nsSeq;

public:

	FirstImprovement(Evaluator<R, ADS, M>& _eval, NSSeq<R, ADS, M>& _nsSeq) :
		eval(_eval), nsSeq(_nsSeq)
	{
	}

	virtual ~FirstImprovement()
	{
	}

	virtual void exec(Solution<R, ADS>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = eval.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R, ADS>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		NSIterator<R, ADS, M>& it = nsSeq.getIterator(e.getEM(), s.getR());

		it.first();

		if (it.isDone())
		{
			delete &it;
			return;
		}

		do
		{
			Move<R, ADS, M>* move = &it.current();

			if (move->canBeApplied(s) && eval.betterThan(eval.moveCost(e, *move, s), 0))
			{
				delete &move->apply(e, s);
				delete move;

				delete &it;

				eval.evaluate(e, s); // updates 'e'
				return;
			}

			delete move;

			it.next();
		} while (!it.isDone());

		delete &it;
	}

	virtual bool compatible(string s)
	{
		return (s == idComponent()) || (LocalSearch<R, ADS, M>::compatible(s));
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearch<R, ADS, M>::idComponent() << "fi";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class FirstImprovementBuilder : public LocalSearchBuilder<R, ADS, M>
{
public:
	virtual ~FirstImprovementBuilder()
	{
	}

	virtual LocalSearch<R, ADS, M>* build(Scanner& scanner, HeuristicFactory<R, ADS, M>& hf, string family = "")
	{
		Evaluator<R, ADS, M>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		NSSeq<R, ADS, M>* nsseq;
		hf.assign(nsseq, scanner.nextInt(), scanner.next()); // reads backwards!

		return new FirstImprovement<R, ADS, M>(*eval, *nsseq);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, M>::idComponent(), "evaluation function"));
		params.push_back(make_pair(NSSeq<R, ADS, M>::idComponent(), "neighborhood structure"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == FirstImprovement<R, ADS, M>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << LocalSearchBuilder<R, ADS, M>::idComponent() << "FI";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

#endif /*OPTFRAME_FI_HPP_*/
