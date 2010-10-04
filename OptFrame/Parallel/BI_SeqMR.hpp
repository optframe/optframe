#ifndef BI_SeqMR_HPP_
#define BI_SeqMR_HPP_

#include "MapReduce++/SeqMR/SeqMR.h"


#include "../Heuristic.hpp"
#include "../NSSeq.hpp"
#include "../Evaluator.hpp"


// OptFrame MapReduce Types

//typedef R KeyA;						// initial solution // TODO memory for reevaluation
//typedef int A;						// search subspace index
//typedef int KeyB;						// dummy unique key (integer -1)
//typedef pair<R, double> B;			// best move in each subspace and its cost // TODO //typedef pair<Move<R, M>*, double> B;	// best move in each subspace and its cost
//typedef R C;							// best solution among all subspaces


template<class R, class M>
class MyMapper: public SeqMR_Mapper<R, int, int, pair<R, double> , R>
{
private:
	Evaluator<R, M>& eval;
	NSSeq<R, M>& nsSeq;
	int NP;
public:
	MyMapper(MapReduce<R, int, int, pair<R, double> , R> * mr, Evaluator<R, M>& _eval, NSSeq<R, M>& _nsSeq, int _NP) :
		SeqMR_Mapper<R, int, int, pair<R, double> , R> (mr), eval(_eval), nsSeq(_nsSeq), NP(_NP)
	{
	}

	virtual vector<pair<int, pair<R, double> > > map(pair<R, int> a)
	{
		//cout << "[MyMapper::map] begin " << a.second << endl;
		vector<pair<int, pair<R, double> > > m;


		Solution<R> s(a.first);


		OffsetBestImprovement<R,M> bi(eval, nsSeq, a.second, NP-1);
		bi.exec(s,0,0);

		Evaluation<M> e = eval.evaluate(s);

		R mapped = R(s.getR());

		m.push_back( make_pair(-1, make_pair( mapped , e.evaluation() ) ) );
		//cout << "[MyMapper::map] end " << a.second << " " << m << endl;

		return m;
	}
};

template<class R, class M>
class MyReducer: public SeqMR_Reducer<R, int, int, pair<R, double> , R>
{
private:
	Evaluator<R, M>& eval;
public:
	MyReducer(MapReduce<R, int, int, pair<R, double> , R> * mr,Evaluator<R, M>& _eval) :
		SeqMR_Reducer<R, int, int, pair<R, double> , R> (mr),eval(_eval)
	{
	}

	virtual pair<int, R> reduce(
			pair<int, vector<pair<R, double> > > bs)
	{
		//cout << "[MyReducer::reduce] begin " << bs.first << endl;

		int bestIndex = 0;
		double bestCost = bs.second[bestIndex].second;

		for (unsigned i = 0 ; i < bs.second.size() ; i++)
		{
			if ( eval.betterThan(bs.second[i].second,bestCost) )
			{
				bestIndex = i;
				bestCost = bs.second[i].second;
			}
		}

		R reduced = bs.second[bestIndex].first;

		//cout << "[MyReducer::reduce] end " << bs.first << endl;

		return pair<int, R> (bs.first, reduced);
	}
};


template<class R, class M>
class BestImprovement_SeqMR: public Heuristic<R, M>
{
private:
	Evaluator<R, M>& eval;
	NSSeq<R, M>& nsSeq;
	int NP;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	BestImprovement_SeqMR(Evaluator<R, M>& _eval, NSSeq<R, M>& _nsSeq, int _NP) :
		eval(_eval), nsSeq(_nsSeq),NP(_NP)
	{
	}

	virtual void exec(Solution<R>& s, double timelimit, double target_f)
	{
		Evaluation<M>& e = eval.evaluate(s.getR());

		exec(s, e, timelimit, target_f);

		delete &e;
	}

	virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
	{
		//cout << "SeqMR test" << endl;

		vector< pair<R,int> > input;
		for (int i = 0 ; i < NP-1 ; i++) input.push_back(pair<R,int>(s.getR(),i));

		SeqMR_MapReduce<R, int, int, pair<R, double> , R> mapReduce;
		MyMapper<R,M> mapper(&mapReduce,eval,nsSeq,NP-1);
		MyReducer<R,M> reducer(&mapReduce,eval);

		vector< pair<int,R> > output = mapReduce.run(mapper,reducer,input);
		cout << output << endl;

		s.setR(output[0].second);

		eval.evaluate(e, s); // updates 'e'
	}

};


#endif /*BI_SeqMR_HPP_*/
