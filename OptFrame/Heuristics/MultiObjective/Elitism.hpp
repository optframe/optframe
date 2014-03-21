// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OPTFRAME_ELITISM_HPP_
#define OPTFRAME_ELITISM_HPP_

#include "MOSIndividual.hpp"
#include "../../Component.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Elitism: Component
{
public:

	Elitism()
	{
	}

	virtual ~Elitism()
	{
	}

	// select individuals and keep archive updated
	virtual void select(unsigned target_size, MOSPopulation<R, ADS, DS>& p, MOSPopulation<R, ADS, DS>& archive) = 0;

	// free individuals from population and keep archive updated
	virtual void free(MOSPopulation<R, ADS, DS>& p, MOSPopulation<R, ADS, DS>& archive) = 0;

	virtual void print() const
	{
		cout << "Elitism" << endl;
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NSGAIISelection: public Elitism<R, ADS, DS>
{
public:

	NSGAIISelection()
	{
	}

	virtual ~NSGAIISelection()
	{
	}

	static bool compare(MOSIndividual<R, ADS, DS>* ind1, MOSIndividual<R, ADS, DS>* ind2)
	{
		return ind1->betterThan(*ind2);
	}

	virtual void select(unsigned target_size, MOSPopulation<R, ADS, DS>& Pop, MOSPopulation<R, ADS, DS>& archive)
	{
		int f = 0;
		unsigned count = 0;
		vector<vector<MOSIndividual<R, ADS, DS>*> > F;
		while(count != Pop.P.size())
		{
			vector<MOSIndividual<R, ADS, DS>*> front;
			for(unsigned i = 0; i < Pop.P.size(); i++)
				if(Pop.P[i]->fitness == f)
					front.push_back(Pop.P[i]);
			F.push_back(front);
			count += front.size();
			f++;
		}

		Pop.P.clear();

		unsigned i = 0;
		while((Pop.P.size() + F[i].size()) <= target_size)
		{
			// Pt+1 = Pt+1 U Fi
			Pop.P.insert(Pop.P.end(), F[i].begin(), F[i].end());
			F[i].clear();

			i = i + 1;
		}

		if(Pop.P.size() < target_size)
		{
			sort(F[i].begin(), F[i].end(), compare);

			unsigned missing = target_size - Pop.P.size();
			for(unsigned j = 0; j < missing; j++)
			{
				Pop.P.push_back(F[i][j]);
				F[i][j] = NULL;
			}
		}

		for(i = 0; i < F.size(); i++)
			for(unsigned j = 0; j < F[i].size(); j++)
				if(F[i][j])
					delete F[i][j];

		archive.P = Pop.P; // no other archiving
	}

	virtual void free(MOSPopulation<R, ADS, DS>& Pop, MOSPopulation<R, ADS, DS>& archive)
	{
		archive.P = Pop.P;
		Pop.P.clear();
	}

};

}

#endif /*OPTFRAME_ELITISM_HPP_*/
