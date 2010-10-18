#ifndef MRNS_HPP_
#define MRNS_HPP_

#include <vector>
#include <omp.h>

#include "Evaluator.hpp"
#include "NSSeq.hpp"
#include "NSEnum.hpp"
#include "Util/MapReduce++/MapMP/MapMP.h"

// =============================
// MapReduce Neighborhood Structure (Enum)
// Abstract Class
// =============================

using namespace std;

template <class R, class M>
class MapMP_Mapper_NS : public MapMP_Mapper< NSEnum<R,M>* , pair<int,int> , NSEnum<R,M>* , Move<R,M>*, Move<R,M>* > {
protected:
	Solution<R>* base; 
	Evaluator<R,M>* av;
	vector<Move<R,M>*> * blacklist;
public:
	MapMP_Mapper_NS(MapReduce< NSEnum<R,M>* , pair<int,int> , NSEnum<R,M>* , Move<R,M>*, Move<R,M>* > * mr,
					Solution<R>* _s, Evaluator<R,M>* _av, vector<Move<R,M>*> * _blacklist)
		:MapMP_Mapper<  NSEnum<R,M>* , pair<int,int> , NSEnum<R,M>* , Move<R,M>*, Move<R,M>*  >(mr),
			base(_s),av(_av),blacklist(_blacklist){};

	virtual multimap< NSEnum<R,M>* , Move<R,M>* > map( pair< NSEnum<R,M>* , pair<int,int> > a ) 
	{ 
		//cout << "\tMapping..\n";

		Solution<R>* s = base->clone();

		multimap< NSEnum<R,M>* , Move<R,M>* > m;

		//vector< Move<R,M>* > * moves = a.first;
		NSEnum<R,M>* ns = a.first;
		
		if (a.second.first >= ns->size()) // verifica existência de algum vizinho
		{printf("Aviso (MRNS - mapper): %i %i\n",a.second.first,ns->size()); return m;}

		Move<R,M>* melhorMov = ns->kmove(a.second.first);//moves->at(a.second.first);
		double melhorCusto = av->custoMovimento(melhorMov,s);

		for (int i = a.second.first ; i < ns->size() ; i += a.second.second)
		{
			Move<R,M>* mov = ns->kmove(i);//moves->at(i);

			if(mov->canBeApplied(s) && !(ns->inList(mov,*blacklist)))
			{
				double custoMov = av->custoMovimento(mov,s);
	
				if(av->betterThan(custoMov,melhorCusto))
				{
					delete melhorMov;
					melhorMov = mov;
					melhorCusto = custoMov;
				}
				else
					delete mov;
			}
			else
				delete mov;
		}

		delete s;

		//cout << "\tOk\n" << melhorCusto << endl; 
		m.insert(pair< NSEnum<R,M>* , Move<R,M>* >(a.first,melhorMov));
		return m; 
/**/	};
};

template <class R, class M>
class MapMP_Reducer_NS : public MapMP_Reducer< NSEnum<R,M>* , pair<int,int> , NSEnum<R,M>* , Move<R,M>*, Move<R,M>* > {
protected:
	Solution<R>* s; 
	//NSEnum<R,M>* ns;
	Evaluator<R,M>* av;
public:
	MapMP_Reducer_NS(MapReduce<  NSEnum<R,M>* , pair<int,int> , NSEnum<R,M>* , Move<R,M>*, Move<R,M>*  > * mr,
					Solution<R>* _s, Evaluator<R,M>* _av)
		:MapMP_Reducer<  NSEnum<R,M>* , pair<int,int> , NSEnum<R,M>* , Move<R,M>*, Move<R,M>*  >(mr),s(_s),av(_av){};
	virtual pair< NSEnum<R,M>* , Move<R,M>* > reduce( pair < NSEnum<R,M>* , vector< Move<R,M>* > > bs ) 
	{
		pair< NSEnum<R,M>* , Move<R,M>* > m;
		//cout << "\tReducing..\n"; 

		if (bs.second.size()<1){printf("Aviso (MRNS - reduce): %i\n",bs.second.size()); return m;} // verifica existência de algum vizinho

		Move<R,M>* melhorMov = bs.second.at(0);
		double melhorCusto = av->custoMovimento(melhorMov,s);

		for (int i = 0 ; i < bs.second.size() ; i++)
		{
			Move<R,M>* mov = bs.second.at(i);

			double custoMov = av->custoMovimento(mov,s);
	
			if(av->betterThan(custoMov,melhorCusto))
			{
				melhorMov = mov;
				melhorCusto = custoMov;
			}
		}

		for (int i = 0 ; i < bs.second.size() ; i++) if (bs.second[i]!=melhorMov) delete bs.second[i];

		//cout << "\tOk\n" << melhorCusto << endl;
		return pair< NSEnum<R,M>* , Move<R,M>* >(bs.first,melhorMov); 
/**/	};
};

template<class R, class M>
class MRNS  : public NSEnum<R,M>
{
private:
	NSEnum<R,M>* ns;
	int numberOfThreads;
public:

	MRNS(int _numberOfThreads, NSEnum<R,M>* _ns):numberOfThreads(_numberOfThreads),ns(_ns){ omp_set_num_threads(numberOfThreads); };

	virtual Move<R,M>* kmove(int k) 
	{
		return ns->kmove(k);
	};

	virtual int size() 
	{
		return ns->size();
	};

	// ========================================================================================
	// Metodos herdados de NSSeq
	// ========================================================================================

	//virtual Move<R,M>* bestMove(Evaluator<R,M>* av, vector<Move<R,M>*> blacklist, Solution<R>* s);
	//virtual Move<R,M>* bestMove(Evaluator<R,M>* av, Solution<R>* s);

	//virtual Solution<R>* bestImprovement(Evaluator<R,M>* av, Solution<R>* base);

	// ========================================================================================
	// Metodos herdados de NSSeq
	// ========================================================================================

	//virtual Move<R,M>* firstMove(Evaluator<R,M>* av, vector<Move<R,M>*> blacklist, Solution<R>* s);
	//virtual Move<R,M>* firstMove(Evaluator<R,M>* av, Solution<R>* s);

	//virtual Solution<R>* firstImprovement(Evaluator<R,M>* av, Solution<R>* base);

	// ========================================================================================
	// ========================================================================================

	virtual void print() { ns->print(); };

	// Implementação usando MapReduce
	virtual Move<R,M>* bestMove(Evaluator<R,M>* av, vector<Move<R,M>*> blacklist, Solution<R>* s)
	{
		// Inicializa a estrutura de vizinhanca
		ns->init(s);

		multimap< NSEnum<R,M>* , pair<int,int> > input;
	
		int NP = numberOfThreads;

		for (int i = 0 ; i < NP ; i++) // Ex: roda 2 processos de mapeamento e redução em um processador dual core
		{
			input.insert(pair< NSEnum<R,M>* , pair<int,int> >(ns,pair<int,int>(i,NP)));
		}

		MapMP_MapReduce< NSEnum<R,M>* , pair<int,int> , NSEnum<R,M>* , Move<R,M>*, Move<R,M>* > mapReduce;
		MapMP_Mapper_NS<R,M> mapper(&mapReduce,s,av,&blacklist);
		MapMP_Reducer_NS<R,M> reducer(&mapReduce,s,av);

		map< NSEnum<R,M>* , Move<R,M>* > output = mapReduce.run(mapper,reducer,input);
		Move<R,M>* bestMov = output[ns];
		return bestMov;
	};

//	virtual Move<R,M>* firstMove(Evaluator<R,M>* av, vector<Move<R,M>*> blacklist, Solution<R>* s) // TODO
//	{
		//...
//	};
	
};

#endif /*MRNS_HPP_*/

