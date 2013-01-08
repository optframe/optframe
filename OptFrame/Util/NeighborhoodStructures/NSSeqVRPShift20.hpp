// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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


#ifndef NSSeqVRPShift20_HPP_
#define NSSeqVRPShift20_HPP_

// Framework includes
#include "../../Move.hpp"
#include "../../NSSeq.hpp"

using namespace std;

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class MoveVRPShift20: public Move<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

private:

	int r1, r2; //routes id's
	int cli;//cli shifted in r1
	int pos;// insertion position in r2

public:

	MoveVRPShift20(int _r1, int _r2, int _cli, int _pos) :
		r1(_r1), r2(_r2), cli(_cli), pos(_pos)
	{
	}

	virtual ~MoveVRPShift20()
	{
	}

	int get_r1()
	{
		return r1;
	}

	int get_r2()
	{
		return r2;
	}

	int get_cli()
	{
		return cli;
	}

	int get_pos()
	{
		return pos;
	}

	bool canBeApplied(const Routes& rep)
	{
		bool numRoutes = rep.size() >= 2;
		return ((r1 >= 0) && (r2 >= 0) && (cli >= 0) && (pos >= 0) && numRoutes);
	}

	MoveVRPShift20<T, ADS, M>& apply(Routes& rep)
	{
		//pegando o cliente
		int c = rep.at(r1).at(cli);
		int c2 = rep.at(r1).at(cli + 1);

		//removendo os clientes de cada rota
		rep.at(r1).erase(rep.at(r1).begin() + cli);
		rep.at(r1).erase(rep.at(r1).begin() + cli);

		//fazendo a inserção
		rep.at(r2).insert(rep.at(r2).begin() + pos, c2);
		rep.at(r2).insert(rep.at(r2).begin() + pos, c);

		return *new MoveVRPShift20<T, ADS, M>( r2, r1, pos, cli);
	}



	virtual bool operator==(const Move<Routes>& _m) const
	{
		const MoveVRPShift20<T, ADS, M>& m = (const MoveVRPShift20<T, ADS, M>&) _m;
		return ((r1 == m.r1) && (r2 == m.r2) && (cli == m.cli) && (pos == m.pos));
	}

	void print() const
	{
		cout << "MoveVRPShift20( ";
		cout << r1 << " , ";
		cout << r2 << " , ";
		cout << cli << " , ";
		cout << pos << " )";
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class NSIteratorVRPShift20: public NSIterator<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

private:

	MoveVRPShift20<T, ADS, M>* m;
	vector<MoveVRPShift20<T, ADS, M>*> moves;
	int index; //index of moves
	const Routes& r;

public:

	NSIteratorVRPShift20(const Routes& _r) :
		r(_r)
	{
		m = NULL;
		index = 0;
	}

	virtual ~NSIteratorVRPShift20()
	{
		/*if (moves.size() > 0) //todo see this with Igor
		 {
		 for (unsigned int i = 0; i < moves.size(); i++)
		 delete moves[i];
		 moves.clear();
		 }*/
	}

	void first()
	{
		for (int r1 = 0; r1 < r.size(); r1++)
		{
			for (int r2 = 0; r2 < r.size(); r2++)
			{
				if (r1 != r2)
				{
					for (int cli = 0; cli < ((int) r.at(r1).size()) - 1; cli++)
					{
						for (int pos = 0; pos <= r.at(r2).size(); pos++)
						{
							moves.push_back(new MoveVRPShift20<T, ADS, M>( r1, r2, cli, pos));
						}
					}
				}
			}
		}
		if (moves.size() > 0)
		{
			m = moves[index];
		}
		else
			m = NULL;
	}

	void next()
	{
		index++;
		if (index < moves.size())
		{
			m = moves[index];
		}
		else
			m = NULL;
	}

	bool isDone()
	{
		return m == NULL;
	}

	MoveVRPShift20<T, ADS, M>& current()
	{
		if (isDone())
		{
			cout << "There isnt any current element!" << endl;
			cout << "NSSeqVRPShift20. Aborting." << endl;
			exit(1);
		}

		return *m;
	}
};

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class NSSeqVRPShift20: public NSSeq<vector<vector<T> > >
{

	typedef vector<vector<T> > Routes;

private:

public:

	NSSeqVRPShift20()
	{
	}

	virtual ~NSSeqVRPShift20()
	{
	}

	virtual MoveVRPShift20<T, ADS, M>& move(const Routes& rep)
	{
		if (rep.size() < 2)
			return *new MoveVRPShift20<T, ADS, M>( -1, -1, -1, -1);

		int r1 = rand() % rep.size();
		if (rep.at(r1).size() < 2)
		{
			return *new MoveVRPShift20<T, ADS, M>( -1, -1, -1, -1);
		}

		int r2;
		do
		{
			r2 = rand() % rep.size();
		} while (r1 == r2);

		int cli = rand() % (rep.at(r1).size() - 1);

		int pos = rand() % (rep.at(r2).size() + 1);
		return *new MoveVRPShift20<T, ADS, M>( r1, r2, cli, pos); // return a random move
	}

	virtual NSIteratorVRPShift20<T, ADS, M>& getIterator(const Routes& r)
	{
		return *new NSIteratorVRPShift20<T, ADS, M>( r);
	}

	virtual void print()
	{
		cout << "NSSeqVRPShift20" << endl;
	}
};

#endif /*NSSeqVRPShift20_HPP_*/

