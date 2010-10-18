#ifndef OPTFRAME_NSENUMVVSHIFTK_HPP_
#define OPTFRAME_NSENUMVVSHIFTK_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSEnum.hpp"
#include "NSVector.hpp"

using namespace std;

//============================================================================
//                           VVShiftk Move
//============================================================================

template<class T, class M>
class MoveVVShiftk : public Move<vector<vector<T> >, M>
{
public:
	int k,v1,p1,v2,p2;

	MoveVVShiftk(int k,int v1,int p1,int v2,int p2)
	{
		this->k = k;
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
		pair<int,pair < pair<int,int> , pair<int,int> > > m;
		m.first = k;
		m.second.first.first = v1;
		m.second.first.second = p1;
		m.second.second.first = v2;
		m.second.second.second = p2;
		NSVector<T>::shiftk_apply(*rep,m);

		return new MoveVVShiftk<T,M>(k,v2,p2,v1,p1);
	}

	void print()
	{
		cout << "Move Vector Vector Shiftk("<< k << " " << v1 << " " << p1 << " " << v2 << " " << p2 <<")"<<endl;
	}

	bool equals(Move<vector<vector<T> >,M>* m) {return equals((MoveVVShiftk<T,M>*)m);}
	bool equals(MoveVVShiftk<T,M>* m)
	{
		return k==m->k && v1==m->v1 && p1==m->p1 && v2==m->v2 && p2==m->p2;
	};
};


//============================================================================
//                  Shift Neighborhood Structure
//============================================================================


template<class T, class M>
class NSEnumVVShiftk: public NSEnum< vector<vector<T> >, M >
{
protected:
	int k;
	vector< pair<int, pair< pair<int,int>,pair<int,int> > > > * moves;
	//int move_k; // TODO conferir

public:	

	NSEnumVVShiftk(int k)
	{
		this->k = k;
		moves = new vector< pair<int, pair< pair<int,int>,pair<int,int> > > >;
	}

	virtual void init(Solution<vector<vector<int> > >* s)
	{
		init(s->getRepresentation());
	}

	virtual void init(vector<vector<int> >* rep)
	{
		delete moves;
		moves = NSVector<int>::shiftk_appliableMoves(*rep,k);

		this->move_k=0;
	}

	virtual Move<vector<vector<T> >,M>* kmove(int _k)
	{
		if(_k>size())
		{
			cerr << "Neighborhood Shift Error! Move " << _k << " does not exist! Valid Interval from 0 to " << (size()-1) << "." << endl;
			exit(1);

			return NULL;
		}

		return new MoveVVShiftk<T,M>((*moves)[_k].first,
				(*moves)[_k].second.first.first,(*moves)[_k].second.first.second,
				(*moves)[_k].second.second.first,(*moves)[_k].second.second.second);
	}

	virtual Move<vector<vector<T> >,M>* move(vector<vector<T> >* rep)
	{
		//cout << "*";
		int v1;
		do v1 = rand()%(*rep).size(); while ((*rep)[v1].size() < k);

		int p1 = rand() % ( (*rep)[v1].size() - k + 1 );

		int v2;
		do v2 = rand()%(*rep).size(); while (v1==v2);

		int p2 = rand()%((*rep)[v2].size()+1);

		return new MoveVVShiftk<T,M>(k,v1,p1,v2,p2);
	};

	int size()
	{
		return moves->size();
	}

	virtual void print()
	{
		cout << "NSEnum Vector Vector Shiftk ("<< size() << ")" << endl;
	}

};

#endif /*OPTFRAME_NSENUMVVSHIFTK_HPP_*/
