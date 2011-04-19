#ifndef OPTFRAME_NSENUMVVSwapK_HPP_
#define OPTFRAME_NSENUMVVSwapK_HPP_

#include <algorithm>

// Framework includes
#include "../NSEnum.hpp"

#include "./Moves/MoveVVSwapk.hpp"
#include "../NSEnumIterator.hpp"

using namespace std;

//============================================================================
//                  Swap Neighborhood Structure
//============================================================================


template<class T, class M>
class NSEnumVVSwapk: public NSEnum< vector<vector<T> >, M >
{
protected:
	int k1,k2;
	vector< pair< pair<int,int> , pair< pair<int,int>,pair<int,int> > > > * moves;

public:	

	using NSEnum<vector<vector<T> >, M>::move; // prevents name hiding

	NSEnumVVSwapk(int k1, int k2)
	{
		this->k1 = k1;
		this->k2 = k2;
		moves = new vector< pair< pair<int,int> , pair< pair<int,int>,pair<int,int> > > >;
	}

	/*virtual void init(Solution<vector<vector<int> > >* s)
	{
		init(s->getR());
	}

	virtual void init(vector<vector<int> >& rep)
	{
		delete moves;
		moves = NSVector<int>::Swapk_appliableMoves(rep,k);
	}*/

	virtual NSIterator<vector<vector<T> > , M>& getIterator(const vector<vector<T> >& rep)
	{
		delete moves;
		moves = NSVector<int>::swapk_appliableMoves(rep,k1,k2);
		random_shuffle(moves->begin(),moves->end());

		return *new NSEnumIterator<vector<vector<T> > , M> (*this);
	}

	virtual Move<vector<vector<T> >,M>& move(unsigned int _k)
	{
		if(_k>size())
		{
			cerr << "Neighborhood Swap Error! Move " << _k << " does not exist! Valid Interval from 0 to " << (size()-1) << "." << endl;
			exit(1);

			//return NULL;
		}

		return * new MoveVVSwapk<T,M>((*moves)[_k].first.first,(*moves)[_k].first.second,
				(*moves)[_k].second.first.first,(*moves)[_k].second.first.second,
				(*moves)[_k].second.second.first,(*moves)[_k].second.second.second);
	}

	virtual Move<vector<vector<T> >,M>& move(const vector<vector<T> >& rep)
	{
		//cout << "*";
		int v1;
		do v1 = rand()%rep.size(); while (rep[v1].size() < k1);

		int p1 = rand() % ( rep[v1].size() - k1 + 1 );

		int v2;
		do v2 = rand()%rep.size(); while (rep[v2].size() < k2 || v1==v2);

		int p2 = rand()%(rep[v2].size() - k2 + 1 );

		return * new MoveVVSwapk<T,M>(k1,k2,v1,p1,v2,p2);
	};

	virtual unsigned int size()
	{
		return moves->size();
	}

	virtual void print()
	{
		cout << "NSEnum Vector Vector Swapk ("<< size() << ")" << endl;
	}

};

#endif /*OPTFRAME_NSENUMVVSwapK_HPP_*/
