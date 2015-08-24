#ifndef OPM_NSENUMTP_HPP_
#define OPM_NSENUMTP_HPP_

// Framework includes
#include "../../OptFrame/NSEnum.hpp"

// Own includes
#include "ProblemInstance.hpp"

#include "Solution.h"

using namespace std;
namespace POLAD
{

class MoveTP: public Move<RepOPM>
{
private:
	int f, c1, c2;
	OPMProblemInstance& opm;

public:

	using Move<RepOPM>::apply; // prevents name hiding
	using Move<RepOPM>::canBeApplied; // prevents name hiding

	MoveTP(int _f, int _c1, int _c2, OPMProblemInstance& _opm) :
		f(_f), c1(_c1), c2(_c2), opm(_opm)
	{
	}

	virtual ~MoveTP()
	{
	}

	bool canBeApplied(const RepOPM& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return ((c1 != c2) && (rep.second(f, c1) > 0) && (rep.first.at(f) > -1) && (opm.isCompatible(c2, rep.first.at(f))));
	}

	Move<RepOPM>* apply(RepOPM& rep, OPTFRAME_DEFAULT_ADS&)
	{
		rep.second(f, c1)--;
		rep.second(f, c2)++;

		if (rep.first[f] < 0)
		{
			cout << "Erro CARREGS!" << endl;
			this->print();
		}

		if (rep.second(f, c1) < 0)
		{
			cout << "Erro VIAGENS NEG!" << endl;
			this->print();

		}

		if (!opm.isCompatible(c2, rep.first[f]))
		{
			cout << "Erro Incompatibilidade!" << endl;
			this->print();

		}

		return new MoveTP(f, c2, c1, opm);
	}

	virtual bool operator==(const Move<RepOPM>& _m) const
	{
		const MoveTP& m = (const MoveTP&) _m;
		return (f == m.f) && (c1 == m.c1) && (c2 == m.c2);
	}

	void print() const
	{
		cout << "MoveTP(mine " << f << " : from " << c1 << " to " << c2 << ")" << endl;
	}
};

class NSEnumTP: public NSEnum<RepOPM>
{
private:
	OPMProblemInstance& opm;
public:

	using NSSeq<RepOPM>::move; // prevents name hiding

	NSEnumTP(OPMProblemInstance& _opm, RandGen& _rg) :
		NSEnum<RepOPM>(_rg), opm(_opm)
	{
	}

	virtual ~NSEnumTP()
	{
	}

	Move<RepOPM>& move(unsigned int k)
	{
		if (k > size())
		{
			cerr << "Neighborhood TP Error! Move " << k << " doesnt exist! Valid Interval from 0 to " << (size() - 1) << "." << endl;
			exit(1);

		}

		int nc = opm.getNumCaminhoes();

		int f, c1, c2, nc2;
		nc2 = nc * nc;
		f = k / nc2;
		c2 = k % nc2 / nc;
		c1 = k % nc;

		return *new MoveTP(f, c1, c2, opm);
	}

	unsigned int size() const
	{
		return opm.getNumFrentes() * opm.getNumCaminhoes() * opm.getNumCaminhoes();
	}

	void print()
	{
		cout << "NSEnum TP (" << size() << ")" << endl;
	}
};

}

#endif /*OPM_NSENUMTP_HPP_*/

