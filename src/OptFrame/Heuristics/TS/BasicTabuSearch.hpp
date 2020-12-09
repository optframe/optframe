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

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class BasicTabuSearch: public SingleObjSearch<XES>, public TS
{
private:
	Evaluator<XES, XEv>& evaluator;
	InitialSearch<XES, XEv>& constructive;
	NSSeq<XES, XEv, XSH>& nsSeq;
	int tlSize;
	int tsMax;

public:

	BasicTabuSearch(Evaluator<XES, XEv>& _ev, InitialSearch<XES, XEv>& _constructive, NSSeq<XES, XEv, XSH>& _nsSeq, int _tlSize, int _tsMax) :
			evaluator(_ev), constructive(_constructive), nsSeq(_nsSeq), tlSize(_tlSize), tsMax(_tsMax)
	{
	}

	virtual ~BasicTabuSearch()
	{
	}

	//pair<S&, Evaluation<>&>* search(double timelimit = 100000000, double target_f = 0, const S* _s = nullptr, const Evaluation<>* _e = nullptr) override
   SearchStatus search(op<XES>& star, const StopCriteria<XEv>& stop) override
	{
		//cout << "BasicTabuSearch exec(" << target_f << "," << timelimit << ")" << endl;

		long tini = time(nullptr);
      long timelimit = stop.timelimit;

		XSolution& s = constructive.generateSolution();
		Evaluation<>& e = evaluator.evaluate(s);

		XSolution* sStar = &s.clone();
		Evaluation<>* evalSStar = &evaluator.evaluate(*sStar);

		//evalSStar->print();

		int Iter = 0;

		int BestIter = 0;

		const vector<Move<XES, XEv>*> emptyList;
		vector<Move<XES, XEv>*> tabuList;

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

			Move<XES, XEv>* bestMove = tabuBestMove(s, e, emptyList);

			XSolution* s1 = &s.clone();

			Move<XES, XEv>* newTabu = &bestMove->apply(*s1);
			Evaluation<>* evalS1 = &evaluator.evaluate(*s1);

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

		//return new pair<S&, Evaluation<>&>(s, e);
      //return STATUS
	}

	Move<XES, XEv>* tabuBestMove(XES& se, const vector<Move<XES, XEv>*>& tabuList)
	{
      XSolution& s = se.first;
      Evaluation<>& e = se.second;

		NSIterator<XES, XEv>& it = nsSeq.getIterator(s.getR(), s.getADS());

		it.first();

		if (it.isDone())
		{
			delete &it;
			return nullptr;
		}

		Move<XES, XEv>* bestMove = &it.current();

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

		MoveCost<>* bestCost = &evaluator.moveCost(*bestMove, se);
		it.next();
		while (!it.isDone())
		{
			Move<XES, XEv>* move = &it.current();
			if (move->canBeApplied(s) && !inList(bestMove, tabuList))
			{
				MoveCost<>* cost = &evaluator.moveCost(*move, se);

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

	bool inList(Move<XES, XEv>* m, const vector<Move<XES, XEv>*>& v)
	{
		for (unsigned int i = 0; i < v.size(); i++)
			if ((*m) == (*v[i]))
				return true;
		return false;
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearch<XES>::idComponent() << "TS:BasicTabuSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

};

template<XSolution S, XEvaluation XEv = Evaluation<>, XESolution XES = pair<S, XEv>, X2ESolution<XES> X2ES = MultiESolution<XES>, XSearch<XES> XSH = std::pair<S, XEv>>
class BasicTabuSearchBuilder: public TS, public SingleObjSearchBuilder<S, XEv, XES>
{
public:
	virtual ~BasicTabuSearchBuilder()
	{
	}

	virtual SingleObjSearch<XES>* build(Scanner& scanner, HeuristicFactory<S, XEv, XES, X2ES>& hf, string family = "")
	{
		Evaluator<XES, XEv>* eval;
		hf.assign(eval, *scanner.nextInt(), scanner.next()); // reads backwards!

		Constructive<S>* constructive;
		hf.assign(constructive, *scanner.nextInt(), scanner.next()); // reads backwards!

		NSSeq<XES, XEv, XSH>* nsseq;
		hf.assign(nsseq, *scanner.nextInt(), scanner.next()); // reads backwards!

		if (!scanner.hasNext())
			return nullptr;

		int tl = *scanner.nextInt();

		if (!scanner.hasNext())
			return nullptr;

		int tsMax = *scanner.nextInt();

		return new BasicTabuSearch<S, XEv, XSH>(*eval, *constructive, *nsseq, tl, tsMax);
	}

	virtual vector<pair<string, string> > parameters()
	{
		vector<pair<string, string> > params;
		params.push_back(make_pair(Evaluator<XES, XEv>::idComponent(), "evaluation function"));
		//params.push_back(make_pair(Constructive<S>::idComponent(), "constructive heuristic"));
params.push_back(make_pair(InitialSearch<XES, XEv>::idComponent(), "constructive heuristic"));
		params.push_back(make_pair(NSSeq<XES, XEv, XSH>::idComponent(), "neighborhood structure"));
		params.push_back(make_pair("OptFrame:int", "tabu list size"));
		params.push_back(make_pair("OptFrame:int", "max number of iterations"));

		return params;
	}

	virtual bool canBuild(string component)
	{
		return component == BasicTabuSearchBuilder<S, XEv>::idComponent();
	}

	static string idComponent()
	{
		stringstream ss;
		ss << SingleObjSearchBuilder<S, XEv>::idComponent() << ":" << TS::family() << ":BasicTabuSearch";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}
};

}

#endif /*OPTFRAME_TABUSEARCH_HPP_*/
