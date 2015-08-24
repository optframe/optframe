#ifndef OPM_NSENUMPO_HPP_
#define OPM_NSENUMPO_HPP_

// Framework includes
#include "../../OptFrame/NSEnum.hpp"

// Own includes
#include "ProblemInstance.hpp"
#include "Solution.h"

using namespace std;
namespace POLAD
{
class MovePO: public Move<RepOPM>
{
private:
	int f;

	vector<int>* mem;

	MovePO(int f, vector<int>* _mem)
	{
		this->f = f;
		this->mem = _mem;
	}

public:

	using Move<RepOPM>::apply; // prevents name hiding
	using Move<RepOPM>::canBeApplied; // prevents name hiding

	MovePO(int f)
	{
		this->f = f;
		this->mem = NULL;
	}

	virtual ~MovePO()
	{
		if (mem)
			delete mem;
	}

	bool canBeApplied(const RepOPM& rep, const OPTFRAME_DEFAULT_ADS&)
	{
		return true;
	}

	Move<RepOPM>* apply(RepOPM& rep, OPTFRAME_DEFAULT_ADS&)
	{
		vector<int>* anterior;

		if (mem == NULL)
		{
			anterior = new vector<int> (rep.second.getNumCols());

			for (unsigned int c = 0; c < rep.second.getNumCols(); c++)
			{
				(*anterior)[c] = rep.second(f, c);
				rep.second(f, c) = 0;
			}
		}
		else
		{
			anterior = NULL;

			for (unsigned int c = 0; c < rep.second.getNumCols(); c++)
				rep.second(f, c) = (*mem)[c];
		}

		return new MovePO(f, anterior);
	}

	virtual bool operator==(const Move<RepOPM>& _m) const
	{
		const MovePO& m = (const MovePO&) _m;
		return (f == m.f);
	}

	void print() const
	{
		cout << "MovePO(" << f << ") ";
		if (mem == NULL)
			cout << "no memory." << endl;
		else
			cout << "memory: " << (*mem) << endl;
	}
};
class NSEnumPO: public NSEnum<RepOPM>
{
private:
	OPMProblemInstance& opm;
public:

	using NSSeq<RepOPM>::move; // prevents name hiding

	NSEnumPO(OPMProblemInstance& _opm, RandGen& _rg) :
		NSEnum<RepOPM>(_rg), opm(_opm)
	{
	}

	virtual ~NSEnumPO()
	{
	}

	Move<RepOPM>& move(unsigned int m)
	{
		if (m > size())
		{
			cerr << "Neighborhood PO Error! Move " << m << " doesnt exist! Valid Interval from 0 to " << (size() - 1) << "." << endl;
			exit(1);

		}

		return *new MovePO(m);
	}

	unsigned int size() const
	{
		return opm.getNumFrentes();
	}

	void print()
	{
		cout << "NSEnum PO (" << size() << ")\n";
	}
};

}
#endif /*OPM_NSENUMPO_HPP_*/

