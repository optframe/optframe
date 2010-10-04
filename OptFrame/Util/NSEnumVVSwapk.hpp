#ifndef OPTFRAME_NSENUMVVSWAPK_HPP_
#define OPTFRAME_NSENUMVVSWAPK_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSEnum.hpp"
#include "NSVector.hpp"

using namespace std;

//============================================================================
//                           VVSwapk Move
//============================================================================

template<class T, class M>
class MoveVVSwapk : public Move<vector<vector<T> >, M>
{
public:
	int k1,k2,v1,p1,v2,p2;

	MoveVVSwapk(int k1, int k2, int v1,int p1,int v2,int p2)
	{
		this->k1 = k1;
		this->k2 = k2;
		this->v1 = v1;
		this->p1 = p1;
		this->v2 = v2;
		this->p2 = p2;
	}

	virtual bool canBeApplied(vector<vector<T> >*)
	{
		return true;
	}

	Move<vector<vector<T> >, M>* apply(vector<vector<T> >* rep)
	{
		pair< pair<int,int> , pair < pair<int,int>,pair<int,int> > > m;
		m.first.first = k1;
		m.first.second = k2;
		m.second.first.first = v1;
		m.second.first.second = p1;
		m.second.second.first = v2;
		m.second.second.second = p2;
		NSVector<T>::swapk_apply(*rep,m);

		return new MoveVVSwapk<T,M>(k1,k2,v2,p2,v1,p1);
	}

	void print()
	{
		cout << "Move Vector Vector Swapk("<< k1 << " " << k2 << " " << v1 << " " << p1 << " " << v2 << " " << p2 <<")"<<endl;
	}

	bool equals(Move<vector<vector<T> >,M>* m) {return equals((MoveVVSwapk<T,M>*)m);}
	bool equals(MoveVVSwapk<T,M>* m)
	{
		return k1==m->k1 && k2==m->k2 && v1==m->v1 && p1==m->p1 && v2==m->v2 && p2==m->p2;
	};
};


//============================================================================
//                  Swap Neighborhood Structure
//============================================================================


template<class T, class M>
class NSEnumVVSwapk: public NSEnum< vector<vector<T> >, M >
{
protected:
	int k1,k2;
	vector< pair< pair<int,int> , pair< pair<int,int>,pair<int,int> > > > * moves;
	//int move_k; // TODO conferir

public:	

	NSEnumVVSwapk(int k1, int k2)
	{
		this->k1 = k1;
		this->k2 = k2;
		moves = new vector< pair< pair<int,int> , pair< pair<int,int>,pair<int,int> > > >;
	}

	virtual void init(Solution<vector<vector<int> > >* s)
	{
		init(s->getR());
	}

	virtual void init(vector<vector<int> >* rep)
	{
		delete moves;
		moves = NSVector<int>::swapk_appliableMoves(*rep,k1,k2);

		this->move_k=0;
	}

	virtual Move<vector<vector<T> >,M>* kmove(int _k)
	{
		if(_k>size())
		{
			cerr << "Neighborhood Swap Error! Move " << _k << " does not exist! Valid Interval from 0 to " << (size()-1) << "." << endl;
			exit(1);

			return NULL;
		}

		return new MoveVVSwapk<T,M>((*moves)[_k].first.first,(*moves)[_k].first.second,
				(*moves)[_k].second.first.first,(*moves)[_k].second.first.second,
				(*moves)[_k].second.second.first,(*moves)[_k].second.second.second);
	}

	virtual Move<vector<vector<T> >,M>* move(vector<vector<T> >* rep)
	{
		//cout << "*";
		int v1;
		do v1 = rand()%(*rep).size(); while ((*rep)[v1].size() < k1);

		int p1 = rand() % ( (*rep)[v1].size() - k1 + 1 );

		int v2;
		do v2 = rand()%(*rep).size(); while ((*rep)[v2].size() < k2 || v1==v2);

		int p2 = rand()%((*rep)[v2].size() - k1 + 1 );

		return new MoveVVSwapk<T,M>(k1,k2,v1,p1,v2,p2);
	};

	int size()
	{
		return moves->size();
	}

	virtual void print()
	{
		cout << "NSEnum Vector Vector Swapk ("<< size() << ")" << endl;
	}

};

#endif /*OPTFRAME_NSENUMVVSWAPK_HPP_*/
