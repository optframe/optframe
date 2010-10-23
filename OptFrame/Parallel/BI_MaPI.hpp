#ifndef BI_MaPI_HPP_
#define BI_MaPI_HPP_

#include "MapReduce++/MaPI/MaPI.h"

// TODO implement this file

#include "../Heuristic.hpp"
#include "../NSEnum.hpp"
#include "../Evaluator.hpp"


// OptFrame MapReduce Types

//typedef R KeyA;						// initial solution // TODO memory for reevaluation
//typedef int A;						// search subspace index
//typedef int KeyB;						// dummy unique key (integer -1)
//typedef pair<R, double> B;			// best move in each subspace and its cost // TODO //typedef pair<Move<R, M>*, double> B;	// best move in each subspace and its cost
//typedef R C;							// best solution among all subspaces


template<class R, class M>
class MyMaPISerializer : public MaPI_Serializer<R, int, int, pair<R, double> , R>
{
public:

	virtual R KeyA_fromString(string s)
	{
		//printf("\nINIT================\n");
		R r;
		//cout << "# "<< s << endl;
		//int i;
		//cin >> i;
		Scanner scan(s);
		scan.useSeparators("()[], ");

		scan.next(); // vector
		scan.next(); // int size

		while (scan.hasNext()) r.push_back(scan.nextInt());

		//cout << "##" << r << endl;

		//printf("\nFIM================\n");
		return r;
	}
	;
	virtual string KeyA_toString(R o)
	{
		stringstream s;
		s << o;
		return s.str();
	}
	;

	virtual int A_fromString(string s)
	{
		return atoi(s.c_str());

	}
	;
	virtual string A_toString(int o)
	{
		stringstream s;
		s << o;
		return s.str();
	}
	;

	virtual int KeyB_fromString(string s)
	{
		return atoi(s.c_str());
	}
	;
	virtual string KeyB_toString(int o)
	{
		stringstream s;
		s << o;
		return s.str();
	}
	;

	virtual pair<R, double> B_fromString(string s) // TODO
	{
		//printf("\nINIT================\n%s\n",s.c_str());
		//cout << "Initializing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;

		R r;
		double c;

		//cout << endl << "# "<< s << endl;

		Scanner scan(s);
		scan.useSeparators("()[], \t\n");

		scan.next(); // pair
		scan.next(); // vector

//		cout << scan.next(); // pair
//		cout << scan.next(); // -1

//		cout << scan.next(); // pair
//		cout << scan.next(); // vector

		int size = scan.nextInt(); // int size

		//cout << size << "!" << endl;

		for (int i = 0 ; i < size && scan.hasNext() ; i++) r.push_back(scan.nextInt());

		//int i;				cin >> i;

		//cout << "rest" << scan.rest() << endl;

		c = scan.nextDouble();

		//cout << c << endl;

		//cout << "##" << r << " c=" << c << endl;
		//printf("\nFIM================\n");
		return make_pair(r,c);
	}
	;
	virtual string B_toString(pair<R, double> o)
	{
		stringstream s;
		s << o;
		return s.str();
	}
	;

	virtual R C_fromString(string s)
	{
		R r;
		//cout << "# "<< s << endl;
		//int i;
		//cin >> i;
		Scanner scan(s);
		scan.useSeparators("()[], ");

		scan.next(); // vector
		scan.next(); // int size

		while (scan.hasNext()) r.push_back(scan.nextInt());

		//cout << "##" << r << endl;

		return r;
	}
	;
	virtual string C_toString(R o)
	{
		stringstream s;
		s << o;
		return s.str();
	}
	;

};



template<class R, class M>
class MyMaPIMapper: public MaPI_Mapper<R, int, int, pair<R, double> , R>
{
private:
	Evaluator<R, M>& eval;
	NSEnum<R, M>& nsEnum;
	int NP;
public:
	MyMaPIMapper(MaPI_MapReduce<R, int, int, pair<R, double> , R> * mr, MaPI_Serializer<R, int, int, pair<R, double> , R> * s, Evaluator<R, M>& _eval, NSEnum<R, M>& _nsEnum, int _NP) :
		MaPI_Mapper<R, int, int, pair<R, double> , R> (mr,s), eval(_eval), nsEnum(_nsEnum), NP(_NP)
	{
	}

	virtual vector<pair<int, pair<R, double> > > map(pair<R, int> a)
	{
		//cout << "[MyMaPIMapper::map] begin " << a.second << endl;
		vector<pair<int, pair<R, double> > > m;


		Solution<R> s(a.first);


		OffsetBestImprovementEnum<R,M> bi(eval, nsEnum, a.second, NP-1);
		bi.exec(s,0,0);

		Evaluation<M> e = eval.evaluate(s);

		R mapped = R(s.getR());

		m.push_back( make_pair(-1, make_pair( mapped , e.evaluation() ) ) );
		//cout << "[MyMaPIMapper::map] end " << a.second << " " << m << endl;

		return m;
	}
};

template<class R, class M>
class MyMaPIReducer: public MaPI_Reducer<R, int, int, pair<R, double> , R>
{
private:
	Evaluator<R, M>& eval;
public:
	MyMaPIReducer(MaPI_MapReduce<R, int, int, pair<R, double> , R> * mr, MaPI_Serializer<R, int, int, pair<R, double> , R> * s,Evaluator<R, M>& _eval) :
		MaPI_Reducer<R, int, int, pair<R, double> , R> (mr,s),eval(_eval)
	{
	}

	virtual pair<int, R> reduce(
			pair<int, vector<pair<R, double> > > bs)
	{
		//cout << "[MyMaPIReducer::reduce] begin " << bs.first << endl;

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

		//cout << "[MyMaPIReducer::reduce] end " << bs.first << endl;

		return pair<int, R> (bs.first, reduced);
	}
};


template<class R, class M>
class BestImprovement_MaPI: public Heuristic<R, M>
{
private:
	Evaluator<R, M>& eval;
	NSEnum<R, M>& nsEnum;

	MyMaPISerializer<R, M> &serializer;
	MaPI_MapReduce<R, int, int, pair<R, double> , R> &mapReduce;
	MyMaPIMapper<R, M> &mapper;
	MyMaPIReducer<R, M> &reducer;

public:

	using Heuristic<R, M>::exec; // prevents name hiding

	BestImprovement_MaPI(
			MyMaPISerializer<R, M> &_serializer,
			MaPI_MapReduce<R, int, int, pair<R, double> , R> &_mapReduce,
			MyMaPIMapper<R, M> &_mapper,
			MyMaPIReducer<R, M> &_reducer,
			Evaluator<R, M>& _eval, NSEnum<R, M>& _nsEnum) :
		serializer(_serializer),mapReduce(_mapReduce),mapper(_mapper),reducer(_reducer),
		eval(_eval), nsEnum(_nsEnum)
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
		//cout << "MaPI test" << endl;

		vector< pair<R,int> > input;
		for (int i = 0 ; i < mapReduce.getMPISize()-1 ; i++) input.push_back(pair<R,int>(s.getR(),i));

//		MyMaPISerializer serializer;
//		MaPI_MapReduce<R, int, int, pair<R, double> , R> mapReduce;
//		MyMaPIMapper<R,M> mapper(&mapReduce,&serializer,eval,nsEnum,NP-1);
//		MyMaPIReducer<R,M> reducer(&mapReduce,&serializer,eval);
//		mapReduce.initServers(argc,argv);

		vector< pair<int,R> > output = mapReduce.run(mapper,reducer,input);
		//cout << output << endl;

		if (output.size()>0)
			s.setR(output[0].second);
		//else { cout << "ERROR" << endl; exit(1); }

		eval.evaluate(e, s); // updates 'e'
	}

};

#endif /*BI_MaPI_HPP_*/
