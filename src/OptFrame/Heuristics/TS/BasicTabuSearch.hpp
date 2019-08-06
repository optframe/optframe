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

#ifndef OPTFRAME_TABUSEARCH_HPP_
#define OPTFRAME_TABUSEARCH_HPP_

#include "../../SingleObjSearch.hpp"
#include "../../NSEnum.hpp"
#include "../../Evaluator.hpp"

#include "TS.h"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
class BasicTabuSearch: public SingleObjSearch<R, ADS, S>, public TS
{
private:
	Evaluator<R, ADS, S>& evaluator;
	Constructive<R, ADS, S>& constructive;
	NSSeq<R, ADS, S>& nsSeq;
	int tlSize;
	int tsMax;

public:

	BasicTabuSearch(Evaluator<R, ADS, S>& _ev, Constructive<R, ADS, S>& _constructive, NSSeq<R, ADS, S>& _nsSeq, int _tlSize, int _tsMax) :
			evaluator(_ev), constructive(_constructive), nsSeq(_nsSeq), tlSize(_tlSize), tsMax(_tsMax)
	{
	}

	virtual ~BasicTabuSearch()
	{
	}

	pair<S&, Evaluation&>* search(double timelimit = 100000000, double target_f = 0, const S* _s = nullptr, const Evaluation* _e = nullptr)
	{
		//cout << "BasicTabuSearch exec(" << target_f << "," << timelimit << ")" << endl;

		long tini = time(nullptr);

		S& s = constructive.generateSolution();
		Evaluation& e = evaluator.evaluate(s);

		S* sStar = &s.clone();
		Evaluation* evalSStar = &evaluator.evaluate(*sStar);

		//evalSStar->print();

		int Iter = 0;

		int BestIter = 0;

		const vector<Move<R, ADS, S>*> emptyList;
		vector<Move<R, ADS, S>*> tabuList;

		long tnow = time(nullptr);

		int estimative_BTmax = 0;

		while (Iter - BestIter <= tsMax && ((tnow - tini) < timelimit))
		{
			Iter = Iter + 1;

			if ((Iter - BestIter) > estimative_BTmax)
				estimative_BTmax = (Iter - BestIter);

			//cout << "Iter " << Iter << " (" << (Iter - BestIter - 1) << " without improvement)" << endl;

			// ==================
			// First: aspiration
			// ==================

			Move<R, ADS, S>* bestMove = tabuBestMove(s, e, emptyList);

			S* s1 = &s.clone();

			Move<R, ADS, S>* newTabu = &bestMove->apply(*s1);
			Evaluation* evalS1 = &evaluator.evaluate(*s1);

			if (evaluator.betterThan(*evalS1, *evalSStar))
			{
				// Better than global!

				for (unsigned int i = 0; i < tabuList.size(); i++)
					if ((*tabuList[i]) == (*bestMove))
					{
						delete tabuList[i];
						tabuList.erase(tabuList.begin() + i);
						break;
					}
			}
			else
			{
				delete s1;
				delete evalS1;
				delete newTabu;
				delete bestMove;

				bestMove = tabuBestMove(s, e, tabuList);
				s1 = &s.clone();
				newTabu = &bestMove->apply(*s1);
				evalS1 = &evaluator.evaluate(*s1);

				delete bestMove;
			}

			// =====================================================
			//    |T| <= tsSize
			// =====================================================

			tabuList.push_back(newTabu);

			if (((int) tabuList.size()) > tlSize)
			{
				delete tabuList[0];
				tabuList.erase(tabuList.begin());
			}

			//-----------------------------------------------------------
			if (((int) tabuList.size()) > tlSize)
				throw string("ERROR on Tabu Search! more elements than expected...");
			//-----------------------------------------------------------

			// =====================================================
			//        's' <- 's1';
			// =====================================================
			s = *s1;
			e = *evalS1;

			delete s1;
			delete evalS1;

			// =====================================================
			//        's' better than 's*' (?)
			// =====================================================

			if (evaluator.betterThan(e, *evalSStar))
			{
				delete sStar;
				delete evalSStar;

				sStar = &s.clone();
				evalSStar = &evaluator.evaluate(*sStar);

				BestIter = Iter;

				//cout << "Improvement on " << BestIter << ": fo=" << evalSStar->evaluation() << endl;
			}

			tnow = time(nullptr);
		}

		while (tabuList.size() > 0)
		{
			delete tabuList[0];
			tabuList.erase(tabuList.begin());
		}

		// ===========
		// 's' <- 's*'
		// ===========

		s = *sStar;
		e = *evalSStar;

		delete sStar;
		delete evalSStar;

		FILE* ftabu = fopen("tabu.txt", "a");
		if (!ftabu)
		{
			cout << "Error creating file 'tabu.txt'" << endl;
		}
		else
		{
			fprintf(ftabu, "%d\n", estimative_BTmax);
			fclose(ftabu);
		}

		return new pair<S&, Evaluation&>(s, e);
	}

	Move<R, ADS, S>* tabuBestMove(S& s, Evaluation& e, const vector<Move<R, ADS, S>*>& tabuList)
	{
		NSIterator<R, ADS, S>& it = nsSeq.getIterator(s.getR(), s.getADS());

		it.first();

		if (it.isDone())
		{
			delete &it;
			return nullptr;
		}

		Move<R, ADS, S>* bestMove = &it.current();

		while (!bestMove->canBeApplied(s) || inList(bestMove, tabuList))
		{
			delete bestMove;
			it.next();

			if (!it.isDone())
			{
				bestMove = &it.current();
			}
			else
			{
				delete &it;
				return nullptr;
			}
		}

		MoveCost* bestCost = &evaluator.moveCost(e, *bestMove, s);
		it.next();
		while (!it.isDone())
		{
			Move<R, ADS, S>* move = &it.current();
			if (move->canBeApplied(s) && !inList(bestMove, tabuList))
			{
				MoveCost* cost = &evaluator.moveCost(e, *move, s);

				if (evaluator.betterThan(*cost, *bestCost))
				{
					delete bestMove;
					delete bestCost;
					bestMove = move;
					bestCost = cost;
				}
				else
				{
					delete move;
					delete cost;
				}
			}
			else
				delete move;
			it.next();
		}

		delete &it;

		delete bestCost;

		return bestMove;
	}

	bool inList(Move<R, ADS, S>* m, const vector<Move<R, ADS, S>*>& v)
	{
		for (unsigned int i = 0; i < v.size(); i++)
			if ((*m) == (*v[i]))
				return true;
		return false;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<R, ADS, S>::idComponent() << "TS:BasicTabuSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, BaseSolution<R,ADS> S = CopySolution<R,ADS>>
class BasicTabuSearchBuilder: public TS, public SingleObjSearchBuilder<R, ADS, S>
{
public:
	virtual ~BasicTabuSearchBuilder()
	{
	}

	virtual SingleObjSearch<R, ADS, S>* build(Scanner& scanner, HeuristicFactory<R, ADS, S>& hf, string family = "")
	{
		Evaluator<R, ADS, S>* eval;
		hf.assign(eval, scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<R, ADS, S>* constructive;
		hf.assign(constructive, scanner.nextInt(), scanner.next()); // reads backwards!

		NSSeq<R, ADS, S>* nsseq;
		hf.assign(nsseq, scanner.nextInt(), scanner.next()); // reads backwards!

		if (!scanner.hasNext())
			return nullptr;

		int tl = scanner.nextInt();

		if (!scanner.hasNext())
			return nullptr;

		int tsMax = scanner.nextInt();

		return new BasicTabuSearch<R, ADS, S>(*eval, *constructive, *nsseq, tl, tsMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<R, ADS, S>::idComponent(), "evaluation function"));
		params.push_back(make_pair(Constructive<R, ADS, S>::idComponent(), "constructive heuristic"));
		params.push_back(make_pair(NSSeq<R, ADS, S>::idComponent(), "neighborhood structure"));
		params.push_back(make_pair("OptFrame:int", "tabu list size"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicTabuSearchBuilder<R, ADS, S>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<R, ADS, S>::idComponent() << ":" << TS::family() << ":BasicTabuSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_TABUSEARCH_HPP_*/
