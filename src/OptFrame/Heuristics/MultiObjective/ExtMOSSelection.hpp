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

#ifndef OPTFRAME_EXTENDED_MOS_SELECTION_HPP_
#define OPTFRAME_EXTENDED_MOS_SELECTION_HPP_

#include "MOSIndividual.hpp"
#include "../../Component.hpp"

namespace optframe
{

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ExtMOSSelection: Component
{
public:

	ExtMOSSelection()
	{
	}

	virtual ~ExtMOSSelection()
	{
	}

	// select individuals and FREE unused individuals, keep archive updated
	virtual void extSelect(unsigned target_size, vector<MOSIndividual<R>*>& ps, vector<MOSExtIndividual<R, X>*>& p, vector<MOSExtIndividual<R, X>*>& archive) = 0;

	virtual void print() const
	{
		cout << "ExtMOSSelection" << endl;
	}
};

template<class R, class X, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSGAIIExtSelection: public ExtMOSSelection<R, X, ADS, DS>
{
public:

	NSGAIIExtSelection()
	{
	}

	virtual ~NSGAIIExtSelection()
	{
	}

	static bool compare(MOSExtIndividual<R, X>* ind1, MOSExtIndividual<R, X>* ind2)
	{
		return ind1->betterThan(*ind2);
	}

	virtual void extSelect(unsigned target_size, vector<MOSIndividual<R>*>& ps, vector<MOSExtIndividual<R, X>*>& Pop, vector<MOSExtIndividual<R, X>*>& archive)
	{
		int f = 0;
		unsigned count = 0;
		vector<vector<MOSExtIndividual<R, X>*> > F;
		while(count != Pop.size())
		{
			vector<MOSExtIndividual<R, X>*> front;
			for(unsigned i = 0; i < Pop.size(); i++)
				if(Pop.at(i)->fitness == f)
					front.push_back(Pop.at(i));
			F.push_back(front);
			count += front.size();
			f++;
		}

		Pop.clear();

		unsigned i = 0;
		while((Pop.size() + F[i].size()) <= target_size)
		{
			// Pt+1 = Pt+1 U Fi
			Pop.insert(Pop.end(), F[i].begin(), F[i].end());
			F[i].clear();

			i = i + 1;
		}

		if(Pop.size() < target_size)
		{
			sort(F[i].begin(), F[i].end(), compare);

			unsigned missing = target_size - Pop.size();
			for(unsigned j = 0; j < missing; j++)
			{
				Pop.push_back(F[i][j]);
				F[i][j] = nullptr;
			}
		}

		for(i = 0; i < F.size(); i++)
			for(unsigned j = 0; j < F[i].size(); j++)
				if(F[i][j])
					delete F[i][j];

		archive = Pop; // no other archiving
	}

	virtual void free(vector<MOSExtIndividual<R, X>*>& Pop,vector<MOSExtIndividual<R, X>*>& archive)
	{
		archive = Pop;
		Pop.clear();
	}

};

}

#endif /*OPTFRAME_EXTENDED_MOS_SELECTION_HPP_*/
