#ifndef OPM_NSENUMST_HPP_
#define OPM_NSENUMST_HPP_

// Framework includes
#include "../../OptFrame/NSEnum.hpp"

// Own includes
#include "ProblemInstance.hpp"

#include "Solution.h"

using namespace std;
namespace POLAD
{

class MoveST: public Move<RepOPM>
{
private:
	int f1, c1, f2, c2;
	OPMProblemInstance& opm;

public:

	using Move<RepOPM>::apply; // prevents name hiding
	using Move<RepOPM>::canBeApplied; // prevents name hiding

	MoveST(int _f1, int _c1, int _f2, int _c2, OPMProblemInstance& _opm) :
		f1(_f1), c1(_c1), f2(_f2), c2(_c2), opm(_opm)

	{
	}

	virtual ~MoveST()
	{
	}

	bool canBeApplied(const RepOPM& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return (f1 != f2) && (c1 != c2) && (rep.second(f1, c1) > 0) && (rep.first.at(f2) > -1) && (opm.isCompatible(c2, rep.first.at(f2)));
	}

	Move<RepOPM>* apply(RepOPM& rep, OPTFRAME_DEFAULT_ADS&)
	{
		rep.second(f1, c1)--;
		rep.second(f2, c2)++;

		return new MoveST(f2, c2, f1, c1, opm);
	}

	virtual bool operator==(const Move<RepOPM>& _m) const
	{
		const MoveST& m = (const MoveST&) _m;
		return (f1 == m.f1) && (f2 == m.f2) && (c1 == m.c1) && (c2 == m.c2);
	}

	void print() const
	{
		cout << "MoveST( from " << f1 << "," << c1 << " to " << f2 << "," << c2 << ")" << endl;
	}
};

class NSEnumST: public NSEnum<RepOPM>
{
private:
	OPMProblemInstance& opm;
public:

	using NSSeq<RepOPM>::move; // prevents name hiding

	NSEnumST(OPMProblemInstance& _opm, RandGen& _rg) :
		NSEnum<RepOPM>(_rg), opm(_opm)
	{
	}

	virtual ~NSEnumST()
	{
	}

	Move<RepOPM>& move(unsigned int k)
	{
		if (k > size())
		{
			cerr << "Neighborhood ST Error! Move " << k << " doesnt exist! Valid Interval from 0 to " << (size() - 1) << "." << endl;
			exit(1);

		}

		int max_i = opm.getNumFrentes();
		int max_j = opm.getNumCaminhoes();

		int a, b, c, d;
		int m = k;

		// ========================================
		// Estrutura: max_i * max_j * max_i * max_j
		// ========================================

		a = floor((double) m / (max_j * max_i * max_j));

		m = m % (max_j * max_i * max_j);

		b = floor((double) m / (max_i * max_j));

		m = m % (max_i * max_j);

		c = floor((double) m / (max_j));

		m = m % (max_j);

		d = m;

		return *new MoveST(a, b, c, d, opm);
	}

	unsigned int size() const
	{
		return opm.getNumFrentes() * opm.getNumCaminhoes() * opm.getNumFrentes() * opm.getNumCaminhoes();
	}

	void print()
	{
		cout << "NSEnum ST (" << size() << ")" << endl;
	}
};

}
#endif /*OPM_NSENUMST_HPP_*/

