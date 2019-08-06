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

#ifndef OPTFRAME_NSENUMVVSwapkIntra_HPP_
#define OPTFRAME_NSENUMVVSwapkIntra_HPP_

// Framework includes
#include "../NSEnum.hpp"

#include "./Moves/MoveVVSwapkIntra.hpp"
#include "../NSEnumIterator.hpp"

using namespace std;

//============================================================================
//                  Swap Neighborhood Structure
//============================================================================


template<class T, class DS >
class NSEnumVVSwapkIntra: public NSEnum< vector<vector<T> >, DS >
{
protected:
	int k1,k2;
	vector < vector< pair<pair<int,int>,pair<int,int> > > * > moves;
	vector< pair<int,int> > moveindex;

public:	

	using NSEnum<vector<vector<T> >, DS >::move; // prevents name hiding

	NSEnumVVSwapkIntra(int k1,int k2)
	{
		this->k1 = k1;
		this->k2 = k2;
	}

	/*virtual void init(Solution<vector<vector<int> > >* s)
	{
		init(s->getR());
	}

	virtual void init(vector<vector<int> >& rep)
	{
		delete moves;
		moves = NSVector<int>::SwapkIntra_appliableMoves(rep,k);
	}*/

	virtual NSIterator<vector<vector<T> > , DS >& getIterator(const vector<vector<T> >& rep)
	{
		for (int i = 0 ; i < moves.size() ; i++) delete moves[i];
		moves.clear();
		moveindex.clear();

		for (int i = 0 ; i < rep.size() ; i++)
		{
			moves.push_back( NSVector<int>::swapk_appliableMoves(rep[i],k1,k2) ); // TODO
			for (int j = 0 ; j < moves.back()->size() ; j++)
				moveindex.push_back(make_pair(i,j));
		}

		return *new NSEnumIterator<vector<vector<T> > , DS > (*this);
	}

	virtual Move<vector<vector<T> >,DS >& move(unsigned int _k)
	{
		if(_k>size())
		{
			cerr << "Neighborhood Swap Error! Move " << _k << " does not exist! Valid Interval from 0 to " << (size()-1) << "." << endl;
			exit(1);

			//return nullptr;
		}

		int i = moveindex[_k].first;
		int j = moveindex[_k].second;

		int k1 = moves[i]->at(j).first.first;
		int k2 = moves[i]->at(j).first.second;
		int v = i;
		int p1 = moves[i]->at(j).second.first;
		int p2 = moves[i]->at(j).second.second;

		return * new MoveVVSwapkIntra<T,DS >(k1,k2,v,p1,p2);
	}

	virtual Move<vector<vector<T> >,DS >& move(const vector<vector<T> >& rep)//TODO
	{
		//cout << "*";
		int v;
		do v = rand()%rep.size(); while (rep[v].size() < k1 + k2);

		int p1 = rand() % ( rep[v].size() - k1 + 1 );

		int p2 = rand()%(rep[v].size()+1);

		return * new MoveVVSwapkIntra<T,DS >(k1,k2,v,p1,p2);
	};

	virtual unsigned int size()
	{
		return moves.size();
	}

	virtual void print()
	{
		cout << "NSEnum Vector Vector SwapkIntra ("<< size() << ")" << endl;
	}

};

#endif /*OPTFRAME_NSENUMVVSwapkIntra_HPP_*/
