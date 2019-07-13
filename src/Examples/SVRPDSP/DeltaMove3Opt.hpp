#ifndef SVRPDSP_DELTA_MOVE_3OPT_HPP_
#define SVRPDSP_DELTA_MOVE_3OPT_HPP_

#include "../../OptFrame/Util/NeighborhoodStructures/Moves/MoveTSP3Opt.hpp"

#include <cmath>

#include "MySolution.hpp"

#include "Delta.h"

using namespace std;

namespace SVRPDSP
{

class DeltaMove3Opt: public MoveTSP3Opt<int, AdsSVRPDSP, DeltaSVRPDSP>
{
	typedef MoveTSP3Opt<int, AdsSVRPDSP, DeltaSVRPDSP> super;

private:
	ProblemInstance* svrpdsp;

	static bool verbose;

public:
	using super::apply;
	using super::canBeApplied;

	DeltaMove3Opt(const super& s, ProblemInstance* _svrpdsp) :
			super(s.i, s.j, s.k, s.mode, _svrpdsp), svrpdsp(_svrpdsp)
	{
		if (!svrpdsp)
		{
			cout << "Error: svrpdsp problem is NULL!" << endl;
			print();
			exit(1);
		}
	}

	DeltaMove3Opt(int i, int j, int k, Mode3Opt mode, ProblemInstance* _svrpdsp) :
			super(i, j, k, mode, _svrpdsp), svrpdsp(_svrpdsp)
	{
		if (!svrpdsp)
		{
			cout << "Error: svrpdsp problem is NULL!" << endl;
			print();
			exit(1);
		}
	}

	virtual ~DeltaMove3Opt()
	{
	}

	virtual bool canBeApplied(const RepSVRPDSP& rep, const AdsSVRPDSP& ads)
	{
		if (!super::canBeApplied(rep, ads))
			return false;

		if (i >= ads.zero)
			return false;

		if ((i >= j) || (j >= k))
			return false;

		if (k >= ads.zero)
		{
			for (unsigned z = i; z < ads.zero; z++)
				if (rep[z] <= svrpdsp->n)
					return false;
		}

		return true;
	}

	virtual Move<RepSVRPDSP, AdsSVRPDSP, DeltaSVRPDSP>& apply(RepSVRPDSP& rep, AdsSVRPDSP& ads)
	{
		// apply move
		Move<RepSVRPDSP, AdsSVRPDSP, DeltaSVRPDSP>& rev = super::apply(rep, ads);
		MySolution::syncADS(*svrpdsp, rep, ads); // syncronize ADS

		super& sRev = (super&) rev;
		DeltaMove3Opt* myMove = new DeltaMove3Opt(sRev, svrpdsp);
		delete &sRev;

		return *myMove;
	}


	pair<double, double>* cost(const Evaluation<DeltaSVRPDSP>& e, const RepSVRPDSP& rep, const AdsSVRPDSP& ads)
	{
		return NULL;
	}


	static string idComponent()
	{
		string idComp = super::idComponent();
		idComp.append(":DeltaMove3Opt");
		return idComp;
	}

	virtual bool operator==(const Move<RepSVRPDSP, AdsSVRPDSP, DeltaSVRPDSP>& _m) const
	{
		const DeltaMove3Opt& m1 = (const DeltaMove3Opt&) _m;
		return ((m1.i == i) && (m1.j == j) && (m1.k == k) && (m1.mode == mode));
	}

	virtual void print() const
	{
		cout << "DeltaMove3Opt: ";
		super::print();
	}
};

bool DeltaMove3Opt::verbose = false;

}

#endif /*SVRPDSP_DELTA_MOVE_3OPT_HPP_*/
