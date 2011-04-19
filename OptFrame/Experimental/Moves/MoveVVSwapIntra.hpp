#ifndef OPTFRAME_MOVEVVSWAPINTRA_HPP_
#define OPTFRAME_MOVEVVSWAPINTRA_HPP_

// Framework includes
#include "../../Move.hpp"

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

	void print() const
	{
		cout << "MoveVVSwapIntra( " << i << " , ( " << j << " , " << k << " ) )" << endl;
	}
};

#endif /*OPTFRAME_MOVEVVSWAPINTRA_HPP_*/
