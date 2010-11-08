#ifndef OPTFRAME_TABUSEARCH_HPP_
#define OPTFRAME_TABUSEARCH_HPP_

#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class TabuSearch: public Heuristic<R, M>
{
private:
	NSSeq<R, M>& nsSeq;
	Evaluator<R, M>& evaluator;
	int tlSize;
	int tsMax;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	TabuSearch(Evaluator<R, M>& _ev, NSSeq<R, M>& _nsSeq, int _tlSize, int _tsMax) :
		evaluator(_ev), nsSeq(_nsSeq), tlSize(_tlSize), tsMax(_tsMax)
	{
	}

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = evaluator.evaluate(s.getR());
		exec(s, e, timelimit, target_f);
		delete &e;
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		//cout << "TabuSearch exec(" << target_f << "," << timelimit << ")" << endl;

		long tini = time(NULL);

		Solution<R>* sStar = &s.clone();
		Evaluation<M>* evalSStar = &evaluator.evaluate(*sStar);

		//evalSStar->print();

		int Iter = 0;

		int BestIter = 0;

		const vector<Move<R, M>*> emptyList;
		vector<Move<R, M>*> tabuList;

		long tnow = time(NULL);

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

			Move<R, M>* bestMove = tabuBestMove(s, e, emptyList);

			Solution<R>* s1 = &s.clone();

			Move<R, M>* newTabu = &bestMove->apply(*s1);
			Evaluation<M>* evalS1 = &evaluator.evaluate(*s1);

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

			if (tabuList.size() > tlSize)
			{
				delete tabuList[0];
				tabuList.erase(tabuList.begin());
			}

			//-----------------------------------------------------------
			if (tabuList.size() > tlSize)
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

			tnow = time(NULL);
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
			return;
		}

		fprintf(ftabu, "%d\n", estimative_BTmax);
		fclose(ftabu);
	}

	Move<R, M>* tabuBestMove(Solution<R>& s, Evaluation<M>& e, const vector<Move<R, M>*>& tabuList)
	{
		NSIterator<R, M>& it = nsSeq.getIterator(e.getM(), s.getR());

		it.first();

		if (it.isDone())
		{
			delete &it;
			return NULL;
		}

		Move<R, M>* bestMove = &it.current();

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
				return NULL;
			}
		}

		double bestCost = evaluator.moveCost(e, *bestMove, s);
		it.next();
		while (!it.isDone())
		{
			Move<R, M>* move = &it.current();
			if (move->canBeApplied(s) && !inList(bestMove, tabuList))
			{
				double cost = evaluator.moveCost(e, *move, s);

				if (evaluator.betterThan(cost, bestCost))
				{
					delete bestMove;
					bestMove = move;
					bestCost = cost;
				}
				else
					delete move;
			}
			else
				delete move;
			it.next();
		}

		delete &it;

		return bestMove;
	}

	bool inList(Move<R, M>* m, const vector<Move<R, M>*>& v)
	{
		for (unsigned int i = 0; i < v.size(); i++)
			if ((*m) == (*v[i]))
				return true;
		return false;
	}

};

#endif /*OPTFRAME_TABUSEARCH_HPP_*/
