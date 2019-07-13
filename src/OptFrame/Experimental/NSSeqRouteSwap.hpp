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

#ifndef OPTFRAME_NSSEQROUTESWAP_HPP_
#define OPTFRAME_NSSEQROUTESWAP_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSEnum.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class DS >
class MoveVVSwapIntra : public Move<vector<vector<T> >, DS >
{
private:
	int i,j,k;

public:

	MoveVVSwapIntra(int i, int j, int k)
	{
		this->i=i;
		this->j=j;
		this->k=k;
	}

	bool canBeApplied(vector<vector<T> >* rep)
	{
		return (rep->size()>0)&&(j != k);
	}

	Move<vector<vector<T> >, DS >* apply(vector<vector<T> >* rep)
	{
		T aux = (*rep)[i][j];
		(*rep)[i][j] = (*rep)[i][k];
		(*rep)[i][k] = aux;

		return new MoveVVSwapIntra(i,k,j);
	}


	bool equals(Move<vector<vector<T> >, DS >* m)
	{
		MoveVVSwapIntra* m1 = (MoveVVSwapIntra*)m;

		return (m1->i==i) && ((m1->j==j && m1->k==k) || (m1->j==k && m1->k==j));
	}

	void print() const
	{
		cout << "MoveVVSwapIntra( " <<i<<" , ( "<<j<<" , "<<k<<" ) )" << endl;
	}
};



template<class T, class DS >
class NSSeqVVSwapIntra: public NSSeq< vector<vector<T> > , DS >
{
private:
	int m_i, m_j, m_k;

public:	

	NSSeqVVSwapIntra(){}

	Move<vector<vector<T> > , DS >* move(vector<vector<T> >* rep)
	{
		if(rep->size()==0)
			return new MoveVVSwapIntra<T,DS >(0,0,0);

		int i = rand()%rep->size();

		int n = rep->at(i).size();

		if(n==0)
			return new MoveVVSwapIntra<T,DS >(i,0,0);

		int j = rand()%n;
		int k = j;

		if(n>1)
			while(k==j)
				k = rand()%n;

		return new MoveVVSwapIntra<T,DS >(i,j,k);
	}


	void init(vector<vector<T> >* rep) // Initialize neighborhood structure
	{
		m_i = 0;
		m_j = 0;
		m_k = 0;
	}

	Move<vector<vector<T> > , DS >* next(vector<vector<T> >* rep)
	{
		if(m_i >= rep->size())
		{
			cout << "Erro!!" << endl;
			exit(1);
		}

		int n2 = rep->at(m_i).size();

		if(m_k<n2-1)
			m_k++;
		else
			if(++m_j < n2-1)
			{
				m_k=m_j+1;
			}
			else
			{
				m_i++;
				m_j=0;
				m_k=0;
			}

		return new MoveVVSwapIntra<T,DS >(m_i,m_j,m_k);
	}

	bool hasNext(vector<vector<T> >* rep)
	{
		int n1 = rep->size();
		int n2 = rep->at(m_i).size();

		if((m_i>=n1-1) && (m_j>=n2-2) && (m_k>=n2-1))
			return false;
		else
			return true;
	}


	virtual void print()
	{
		cout << "NSSeqVVSwapIntra" << endl;
	}

};


#endif /*OPTFRAME_NSSEQROUTESWAP_HPP_*/
