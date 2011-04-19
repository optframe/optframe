#ifndef OPTFRAME_ITERATORVVSWAPINTRA_HPP_
#define OPTFRAME_ITERATORVVSWAPINTRA_HPP_

// Framework includes
#include "../../NSIterator.hpp"

#include "../Moves/MoveVVSwapIntra.hpp"

using namespace std;

// Working structure: vector<vector<T> >

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

#endif /*OPTFRAME_ITERATORVVSWAPINTRA_HPP_*/
