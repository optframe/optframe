#ifndef OPTFRAME_NSSEQVVSWAPINTRA_HPP_
#define OPTFRAME_NSSEQVVSWAPINTRA_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSEnum.hpp"

using namespace std;

// Working structure: vector<vector<T> >

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class MoveVVSwapIntra: public Move<vector<vector<T> > , M>
{
	typedef vector<vector<T> > Routes;

private:
	int i, j, k;

public:

	MoveVVSwapIntra(int i, int j, int k)
	{
		this->i = i;
		this->j = j;
		this->k = k;
	}

	virtual ~MoveVVSwapIntra()
	{
	}

	bool canBeApplied(const Routes& rep)
	{
		return (j != k);
	}

	Move<Routes, M>& apply(Routes& rep)
	{
		T aux = rep[i][j];
		rep[i][j] = rep[i][k];
		rep[i][k] = aux;

		return *new MoveVVSwapIntra(i, k, j);
	}

	virtual bool operator==(const Move<Routes, M>& _m) const
	{
		const MoveVVSwapIntra& m = (const MoveVVSwapIntra&) _m;
		return (m.i == i) && ((m.j == j && m.k == k) || (m.j == k && m.k == j));
	}

	void print()
	{
		cout << "MoveVVSwapIntra( " << i << " , ( " << j << " , " << k
				<< " ) )" << endl;
	}
};

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class NSIteratorVVSwapIntra: public NSIterator<vector<vector<T> > , M>
{
	typedef vector<vector<T> > Routes;
	const Routes& routes;

	int m_i, m_j, m_k;
public:

	NSIteratorVVSwapIntra(const Routes& r) :
		routes(r)
	{
	}

	virtual ~NSIteratorVVSwapIntra()
	{
	}

	void first()
	{
		// initialize vars
		m_i = 0;
		m_j = 0;
		m_k = 0;

		// go to a valid move
		next();
	}

	void next()
	{
		int n2 = routes.at(m_i).size();

		if (m_k < n2 - 1)
			m_k++;
		else if (++m_j < n2 - 1)
		{
			m_k = m_j + 1;
		}
		else
		{
			m_i++;
			m_j = 0;
			m_k = 0;
		}
	}

	bool isDone()
	{
		int n1 = routes.size();
		int n2 = routes.at(m_i).size();

		if ((m_i >= n1 - 1) && (m_j >= n2 - 2) && (m_k >= n2 - 1))
			return true;
		else
			return false;
	}

	Move<Routes, M>& current()
	{
		return *new MoveVVSwapIntra<T, M> (m_i, m_j, m_k);
	}
};

template<class T, class M = OPTFRAME_DEFAULT_MEMORY>
class NSSeqVVSwapIntra: public NSSeq<vector<vector<T> > , M>
{
	typedef vector<vector<T> > Routes;

public:

	NSSeqVVSwapIntra()
	{
	}

	virtual ~NSSeqVVSwapIntra()
	{
	}

	Move<Routes, M>& move(const Routes& rep)
	{
		int i = rand() % rep.size();

		int n = rep.at(i).size();

		int j = rand() % n;
		int k = j;

		if (n > 1)
			while (k == j)
				k = rand() % n;

		return *new MoveVVSwapIntra<T, M> (i, j, k);
	}

	virtual NSIterator<Routes, M>& getIterator(const Routes& r)
	{
		return *new NSIteratorVVSwapIntra<T, M> (r);
	}

	virtual void print()
	{
		cout << "NSSeqVVSwapIntra" << endl;
	}

};

#endif /*OPTFRAME_NSSEQVVSWAPINTRA_HPP_*/
