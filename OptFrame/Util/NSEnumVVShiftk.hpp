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

	virtual bool canBeApplied(const vector<vector<T> >&)
	{
		return true;
	}

	virtual Move<vector<vector<T> >, M>& apply(vector<vector<T> >& rep)
	{
		pair<int,pair < pair<int,int> , pair<int,int> > > m;
		m.first = k;
		m.second.first.first = v1;
		m.second.first.second = p1;
		m.second.second.first = v2;
		m.second.second.second = p2;
		NSVector<T>::shiftk_apply(rep,m);

		return * new MoveVVShiftk<T,M>(k,v2,p2,v1,p1);
	}

	virtual Move<vector<vector<T> >, M>& apply(M& m, vector<vector<T> > & r)
	{
		if (!m.empty())
		{
			m[v1].first = m[v2].first = -1;
			m[v1].second.first = p1;
			m[v1].second.second = r[v1].size()-1;
			m[v2].second.first = p2;
			m[v2].second.second = r[v2].size()-1;
		} else
		{
			//e->setMemory(new MemVRP(r.size(),make_pair(-1,make_pair(0,r.size()-1))));
			m = MemVRPTW(r.size(),make_pair(-1,make_pair(0,r.size()-1)));
		}

		return apply(r);
	}

	virtual void print() const
	{
		cout << "Move Vector Vector Shiftk("<< k << " " << v1 << " " << p1 << " " << v2 << " " << p2 <<")"<<endl;
	}

	virtual bool operator==(const Move<vector<vector<T> >,M>& m) const
	{
		return false; //TODO
	}
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

public:	

	using NSEnum<RepVRPTW, MemVRPTW>::move; // prevents name hiding

	NSEnumVVShiftk(int k)
	{
		this->k = k;
		moves = new vector< pair<int, pair< pair<int,int>,pair<int,int> > > >;
	}

	/*virtual void init(Solution<vector<vector<int> > >* s)
	{
		init(s->getR());
	}

	virtual void init(vector<vector<int> >& rep)
	{
		delete moves;
		moves = NSVector<int>::shiftk_appliableMoves(rep,k);
	}*/

	virtual NSIterator<vector<vector<T> > , M>& getIterator(const vector<vector<T> >& rep)
	{
		delete moves;
		moves = NSVector<int>::shiftk_appliableMoves(rep,k);
		random_shuffle(moves->begin(),moves->end());

		return *new NSEnumIterator<vector<vector<T> > , M> (*this);
	}

	virtual Move<vector<vector<T> >,M>& move(unsigned int _k)
	{
		if(_k>size())
		{
			cerr << "Neighborhood Shift Error! Move " << _k << " does not exist! Valid Interval from 0 to " << (size()-1) << "." << endl;
			exit(1);

			//return NULL;
		}

		return * new MoveVVShiftk<T,M>((*moves)[_k].first,
				(*moves)[_k].second.first.first,(*moves)[_k].second.first.second,
				(*moves)[_k].second.second.first,(*moves)[_k].second.second.second);
	}

	virtual Move<vector<vector<T> >,M>& move(const vector<vector<T> >& rep)
	{
		//cout << "*";
		int v1;
		do v1 = rand()%rep.size(); while (rep[v1].size() < k);

		int p1 = rand() % ( rep[v1].size() - k + 1 );

		int v2;
		do v2 = rand()%rep.size(); while (v1==v2);

		int p2 = rand()%(rep[v2].size()+1);

		return * new MoveVVShiftk<T,M>(k,v1,p1,v2,p2);
	};

	virtual unsigned int size()
	{
		return moves->size();
	}

	virtual void print()
	{
		cout << "NSEnum Vector Vector Shiftk ("<< size() << ")" << endl;
	}

};

#endif /*OPTFRAME_NSENUMVVSHIFTK_HPP_*/
