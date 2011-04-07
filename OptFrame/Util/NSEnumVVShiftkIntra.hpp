#ifndef OPTFRAME_NSENUMVVShiftkIntra_HPP_
#define OPTFRAME_NSENUMVVShiftkIntra_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSEnum.hpp"
#include "NSVector.hpp"

using namespace std;

//============================================================================
//                           VVShiftkIntra Move
//============================================================================

template<class T, class M>
class MoveVVShiftkIntra : public Move<vector<vector<T> >, M>
{
public:
	int k,v,p1,p2;

	MoveVVShiftkIntra(int k,int v,int p1,int p2)
	{
		this->k = k;
		this->v = v;
		this->p1 = p1;
		this->p2 = p2;
	}

	virtual bool canBeApplied(const vector<vector<T> >&)
	{
		return true;
	}

	virtual Move<vector<vector<T> >, M>& apply(vector<vector<T> >& rep)
	{
		pair<int,pair<int,int> > m;
		m.first = k;
		m.second.first = p1;
		m.second.second = p2;
		NSVector<T>::shiftk_apply(rep[v],m);

		return * new MoveVVShiftkIntra<T,M>(k,v,p2,p1);
	}

	virtual Move<vector<vector<T> >, M>& apply(M& m, vector<vector<T> > & r)
	{
		if (!m.empty())
		{
			m[v].first = -1;
			m[v].second.first = p1;
			m[v].second.second = r[v].size()-1;
		} else
		{
			//e->setMemory(new MemVRP(r.size(),make_pair(-1,make_pair(0,r.size()-1))));
			m = MemVRPTW(r.size(),make_pair(-1,make_pair(0,r.size()-1)));
		}

		return apply(r);
	}

	virtual void print() const
	{
		cout << "Move VRP ShiftkIntra("<< k << " " << v << " " << p1 << " " << p2 <<")"<<endl;
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
class NSEnumVVShiftkIntra: public NSEnum< vector<vector<T> >, M >
{
protected:
	int k;
	vector < vector< pair<int,pair<int,int> > > * > moves;
	vector< pair<int,int> > moveindex;

public:	

	using NSEnum<RepVRPTW, MemVRPTW>::move; // prevents name hiding

	NSEnumVVShiftkIntra(int k)
	{
		this->k = k;
	}

	/*virtual void init(Solution<vector<vector<int> > >* s)
	{
		init(s->getR());
	}

	virtual void init(vector<vector<int> >& rep)
	{
		delete moves;
		moves = NSVector<int>::ShiftkIntra_appliableMoves(rep,k);
	}*/

	virtual NSIterator<vector<vector<T> > , M>& getIterator(const vector<vector<T> >& rep)
	{
		for (int i = 0 ; i < moves.size() ; i++) delete moves[i];
		moves.clear();
		moveindex.clear();

		for (int i = 0 ; i < rep.size() ; i++)
		{
			moves.push_back( NSVector<int>::shiftk_appliableMoves(rep[i],k) );
			for (int j = 0 ; j < moves.back()->size() ; j++)
				moveindex.push_back(make_pair(i,j));
		}

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

		int i = moveindex[_k].first;
		int j = moveindex[_k].second;

		int k = moves[i]->at(j).first;
		int v = i;
		int p1 = moves[i]->at(j).second.first;
		int p2 = moves[i]->at(j).second.second;

		return * new MoveVVShiftkIntra<T,M>(k,v,p1,p2);
	}

	virtual Move<vector<vector<T> >,M>& move(const vector<vector<T> >& rep)
	{
		//cout << "*";
		int v;
		do v = rand()%rep.size(); while (rep[v].size() < k);

		int p1 = rand() % ( rep[v].size() - k + 1 );

		int p2 = rand()%(rep[v].size()+1);

		return * new MoveVVShiftkIntra<T,M>(k,v,p1,p2);
	};

	virtual unsigned int size()
	{
		return moves.size();
	}

	virtual void print()
	{
		cout << "NSEnum Vector Vector ShiftkIntra ("<< size() << ")" << endl;
	}

};

#endif /*OPTFRAME_NSENUMVVShiftkIntra_HPP_*/
