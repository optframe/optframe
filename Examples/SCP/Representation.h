#ifndef SCP_REPRESENTATION_H_
#define SCP_REPRESENTATION_H_

#include <vector>
#include "../../OptFrame/Util/printable.h"

#include "../../OptFrame/SetInspired/Set.h"
#include "../../OptFrame/SetInspired/Subset.h"
#include "../../OptFrame/SetInspired/SetUnion.h"
#include "../../OptFrame/SetInspired/Permutation.h"

#include "../../OptFrame/SetInspired/Adapter.hpp"

using namespace std;

struct RepSCP
{
	Set& set;       // set of all elements
	Subset& sub;    // subset of chosen elements
	Permutation& p; // permutation of the subset
};

class SCPSetAdapter: public SetAdapter<RepSCP>
{
	Set& adapt(RepSCP& r)
	{
		return r.set;
	}

	const Set& adapt(const RepSCP& r)
	{
		return r.set;
	}
};

class SCPSubsetAdapter: public SubsetAdapter<RepSCP>
{
	Subset& adapt(RepSCP& r)
	{
		return r.sub;
	}

	const Subset& adapt(const RepSCP& r)
	{
		return r.sub;
	}
};

class SCPPermAdapter: public PermAdapter<RepSCP>
{
	Permutation& adapt(RepSCP& r)
	{
		return r.p;
	}

	const Permutation& adapt(const RepSCP& r)
	{
		return r.p;
	}
};

//#include <ostream>
//ostream & operator<< (ostream & s, RepSCP){}

#endif /*SCP_REPRESENTATION_H_*/

