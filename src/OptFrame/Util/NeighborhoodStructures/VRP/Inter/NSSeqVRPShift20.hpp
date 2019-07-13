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


#ifndef NSSeqVRPShift20_HPP_
#define NSSeqVRPShift20_HPP_

// Framework includes
#include "../../../../Move.hpp"
#include "../../../../NSSeq.hpp"

using namespace std;

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class MoveVRPShift20: public Move<vector<vector<T> > , ADS, DS>
{

	typedef vector<vector<T> > Routes;

private:

	int r1, r2; //routes id's
	int cli;//cli shifted in r1
	int pos;// insertion position in r2

	OPTFRAME_DEFAULT_PROBLEM* problem;

public:

	MoveVRPShift20(int _r1, int _r2, int _cli, int _pos, OPTFRAME_DEFAULT_PROBLEM* _problem = nullptr) :
		r1(_r1), r2(_r2), cli(_cli), pos(_pos), problem(_problem)
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

	virtual bool canBeApplied(const Routes& rep, const ADS&)
	{
		bool numRoutes = rep.size() >= 2;
		return ((r1 >= 0) && (r2 >= 0) && (cli >= 0) && (pos >= 0) && numRoutes);
	}

	virtual Move<Routes, ADS, DS>& apply(Routes& rep, ADS&)
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

		return *new MoveVRPShift20<T, ADS, DS> (r2, r1, pos, cli);
	}

	virtual bool operator==(const Move<Routes>& _m) const
	{
		const MoveVRPShift20<T, ADS, DS>& m = (const MoveVRPShift20<T, ADS, DS>&) _m;
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

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS, class MOVE = MoveVRPShift20<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM>
class NSIteratorVRPShift20: public NSIterator<vector<vector<T> > , ADS, DS>
{

	typedef vector<vector<T> > Routes;

private:

	MoveVRPShift20<T, ADS, DS>* m;
	vector<MoveVRPShift20<T, ADS, DS>*> moves;
	int index; //index of moves
	const Routes& r;

	P* p; // has to be the last
public:

	NSIteratorVRPShift20(const Routes& _r, P* _p = nullptr) :
		r(_r), p(_p)
	{
		m = nullptr;
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

	virtual void first()
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
							moves.push_back(new MOVE(r1, r2, cli, pos, p));
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
			m = nullptr;
	}

	virtual void next()
	{
		index++;
		if (index < moves.size())
		{
			m = moves[index];
		}
		else
			m = nullptr;
	}

	virtual bool isDone()
	{
		return m == nullptr;
	}

	virtual Move<Routes, ADS, DS>& current()
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

template<class T, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS, class MOVE = MoveVRPShift20<T, ADS, DS>, class P = OPTFRAME_DEFAULT_PROBLEM, class NSITERATOR = NSIteratorVRPShift20<T, ADS, DS, MOVE, P> >
class NSSeqVRPShift20: public NSSeq<vector<vector<T> > , ADS, DS>
{

	typedef vector<vector<T> > Routes;

private:
	P* p; // has to be the last

public:

	NSSeqVRPShift20(P* _p = nullptr) :
		p(_p)
	{
	}

	virtual ~NSSeqVRPShift20()
	{
	}

	virtual Move<Routes, ADS, DS>& move(const Routes& rep, const ADS&)
	{
		if (rep.size() < 2)
			return *new MOVE(-1, -1, -1, -1, p);

		int r1 = rand() % rep.size();
		if (rep.at(r1).size() < 2)
		{
			return *new MOVE(-1, -1, -1, -1, p);
		}

		int r2;
		do
		{
			r2 = rand() % rep.size();
		} while (r1 == r2);

		int cli = rand() % (rep.at(r1).size() - 1);

		int pos = rand() % (rep.at(r2).size() + 1);
		return *new MOVE(r1, r2, cli, pos, p); // return a random move
	}

	virtual NSIteratorVRPShift20<T, ADS, DS, MOVE, P>& getIterator(const Routes& r, const ADS&)
	{
		return *new NSITERATOR (r, p);
	}

	virtual void print()
	{
		cout << "NSSeqVRPShift20 with move: " << MOVE::idComponent();
	}
};

#endif /*NSSeqVRPShift20_HPP_*/

