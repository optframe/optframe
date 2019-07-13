#ifndef OPM_NSENUMLD_HPP_
#define OPM_NSENUMLD_HPP_

// Framework includes
#include "../../OptFrame/NSEnum.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Solution.h"

using namespace std;
namespace POLAD
{

class MoveLD: public Move<RepOPM>
{
private:
	int f1, f2;

public:

	using Move<RepOPM>::apply; // prevents name hiding
	using Move<RepOPM>::canBeApplied; // prevents name hiding

	MoveLD(int _f1, int _f2) :
		f1(_f1), f2(_f2)
	{
	}

	virtual ~MoveLD()
	{
	}

	bool canBeApplied(const RepOPM& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return f1<f2;
	}

	Move<RepOPM>* apply(RepOPM& rep, OPTFRAME_DEFAULT_ADS&)
	{
		int nc = rep.second.getNumCols();
		int l1;

		for (int c = 0; c < nc; c++)
		{
			l1 = rep.second(f1, c);
			rep.second(f1, c) = rep.second(f2, c);
			rep.second(f2, c) = l1;
		}

		int car1 = rep.first[f1];
		rep.first[f1] = rep.first[f2];
		rep.first[f2] = car1;

		return new MoveLD(f2, f1);
	}

	virtual bool operator==(const Move<RepOPM>& _m) const
	{
		const MoveLD& m = (const MoveLD&) _m;
		return (f1 == m.f1) && (f2 == m.f2);
	}

	void print() const
	{
		cout << "MoveLD(" << f1 << " , " << f2 << ")" << endl;
	}
};

class NSEnumLD: public NSEnum<RepOPM>
{
private:
	OPMProblemInstance& opm;
public:

	using NSSeq<RepOPM>::move; // prevents name hiding

	NSEnumLD(OPMProblemInstance& _opm, RandGen& _rg) :
			NSEnum<RepOPM>(_rg), opm(_opm)
{
	}

	virtual ~NSEnumLD()
	{
	}

	Move<RepOPM>& move(unsigned int m)
	{
		int num_f = opm.getNumFrentes();

		/* Implementação O(1) */

		if (m < num_f * num_f)
		{
			int i = floor((double) m / num_f);
			int j = m % num_f;
			return *new MoveLD(i, j);
		}
		else
		{
			cerr << "Neighborhood LD Error! Move " << m << " doesnt exist! Valid Interval from 0 to " << (size() - 1) << "." << endl;
			exit(1);
		}
	}

	unsigned int size() const
	{
		return opm.getNumFrentes() * (opm.getNumFrentes() - 1) / 2;
	}

	void print()
	{
		cout << "NSEnum LD (" << size() << ")" << endl;
	}
};

}
#endif /*OPM_NSENUMLD_HPP_*/
