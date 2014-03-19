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

#ifndef OPTFRAME_FITNESS_ASSIGNMENT_HPP_
#define OPTFRAME_FITNESS_ASSIGNMENT_HPP_

#include "MOSIndividual.hpp"
#include "../../Component.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class FitnessAssignment: Component
{
public:

	FitnessAssignment()
	{
	}

	virtual ~FitnessAssignment()
	{
	}

	virtual void assignFitness(vector<MOSIndividual<R, ADS, DS>*>& p) = 0;

	virtual void print() const
	{
		cout << "FitnessAssignment" << endl;
	}

	virtual string id() const
	{
		return "FitnessAssignment";
	}
};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class BiObjNonDominatedSort: public FitnessAssignment<R, ADS, DS>
{
public:
	vector<Direction<DS>*> vDir;

	BiObjNonDominatedSort(vector<Direction<DS>*>& _vDir) :
			vDir(_vDir)
	{
		assert(vDir.size() == 2);
	}

	virtual ~BiObjNonDominatedSort()
	{
	}

	static bool sortByFirst(const MOSIndividual<R, ADS, DS>* ind1, const MOSIndividual<R, ADS, DS>* ind2)
	{
		return ind1->mev.at(0).evaluation() < ind2->mev.at(0).evaluation();
	}

	static bool sortBySecond(const MOSIndividual<R, ADS, DS>* ind1, const MOSIndividual<R, ADS, DS>* ind2)
	{
		return ind1->mev.at(1).evaluation() < ind2->mev.at(1).evaluation();
	}

	virtual void assignFitness(vector<MOSIndividual<R, ADS, DS>*>& P)
	{
		// ASSUMES UNIQUE ELEMENTS

		sort(P.begin(), P.end(), sortBySecond); // any sort is good!
		stable_sort(P.begin(), P.end(), sortByFirst); // necessary to be stable!!
		// (otherwise, when first objective is equal then the second may be downgraded in future and destroy dominance)

		for(unsigned i = 0; i < P.size(); i++)
			P[i]->fitness = 10000000; // INF

		for(unsigned i = 0; i < P.size(); i++)
		{
			//cout << "i:" << i << " ";
			//P[i]->print();
		}

		unsigned count_x = 0;
		int rank = 0;
		while(count_x != P.size())
		{
			// find first
			unsigned j = 0;
			for(j = 0; j < P.size(); j++)
				if(P[j]->fitness == 10000000)
					break;
			P[j]->fitness = rank;
			//cout << "rank: " << rank << " to ";
			//P[j]->print();
			int max_obj2 = P[j]->mev.at(1).evaluation();
			int min_obj2 = max_obj2;
			//cout << "max_obj2: " << max_obj2 << " min_obj2: " << min_obj2 << endl;
			count_x++;
			vector<int> m;
			m.push_back(j);

			for(unsigned r = j + 1; r < P.size(); r++)
				if(P[r]->fitness == 10000000)
				{
					if(P[r]->mev.at(1).evaluation() > max_obj2)
						continue; // discard element (is dominated!)

					if(P[r]->mev.at(1).evaluation() < min_obj2) // add element (improves the best obj2!)
					{
						//cout << "r:" << r << " ";
						P[r]->fitness = rank;
						//cout << "rank: " << rank << " to ";
						//P[r]->print();
						//cout << "min_obj2: " << min_obj2 << " => ";
						min_obj2 = P[r]->mev.at(1).evaluation();
						//cout << min_obj2 << endl;
						count_x++;
						m.push_back(r);
						continue;
					}

					// otherwise, check on list
					bool nonDom = true;
					for(unsigned z = 0; z < m.size(); z++)
						if(P[m[z]]->mev.at(1).evaluation() <= P[r]->mev.at(1).evaluation()) // <= is enough, because distance is better (and values are unique)
						{
							//cout << "r: " << r << " dominated by " << m[z] << " m=" << m << endl;
							nonDom = false;
							break;
						}

					if(nonDom)
					{
						//cout << "*r:" << r << " ";
						P[r]->fitness = rank;
						//cout << "rank: " << rank << " to ";
						//P[r]->print();
						//cout << "m=" << m << endl;
						count_x++;
						m.push_back(r);
					}
					else
					{
						//cout << "r: " << r << " not selected! [" << P[r]->mev.at(0).evaluation() << ";" << P[r]->mev.at(1).evaluation() << "]" << endl;
					}
				}

			//cout << "FINISHED RANK " << rank << " WITH " << m << endl << endl;
			m.clear();

			rank++;
		}

		// SELECTION SCHEME
		/*
		 vector<vector<IndividualNSGAII<R>*>*> fronts;
		 unsigned count_check = 0;
		 for(int r = 0; r < rank; r++)
		 {
		 fronts.push_back(new vector<IndividualNSGAII<R>*>);
		 for(unsigned i = 0; i < Ps.size(); i++)
		 if(Ps[i]->fitness == r)
		 {
		 fronts[r]->push_back(Ps[i]);
		 count_check++;
		 }
		 }

		 if(count_check != Ps.size())
		 {
		 cout << "ERROR in nonDominatedSort! count_check=" << count_check << " |Ps|=" << Ps.size() << endl;
		 exit(1);
		 }

		 return fronts;
		 */
	}

};

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class NonDominatedSort: public FitnessAssignment<R, ADS, DS>
{
public:

	vector<Direction<DS>*> vDir;

	NonDominatedSort(vector<Direction<DS>*>& _vDir) :
			vDir(_vDir)
	{
	}

	virtual ~NonDominatedSort()
	{
	}

	virtual void assignFitness(vector<MOSIndividual<R, ADS, DS>*>& P)
	{
		ParetoDominance<R, ADS, DS> pDominance(vDir);

		if(Component::information)
			cout << this->id() << "::fastNonDominatedSort |P|=" << P.size() << " begin" << endl;

		vector<vector<int>*> F;
		F.push_back(new vector<int>);
		vector<vector<int> > S(P.size());
		vector<int> n(P.size());

		// for each 'p' in 'P'
		double sum_dom_time = 0.0;
		int count_dom_times = 0;
		for(unsigned p = 0; p < P.size(); p++)
		{
			S[p].clear(); // Sp = {}
			n[p] = 0;     // np = 0

			// for each 'q' in 'P'
			for(unsigned q = 0; q < P.size(); q++)
				if(p != q)
				{
					pair<bool, bool> v = pDominance.birelation(P.at(p)->mev, P.at(q)->mev);

					// if (p << q)
					if(v.first)
						S[p].push_back(q); // Sp = Sp U {q}
					// else if (q << p)
					else if(v.second)
						n[p]++; // np = np + 1
				}

			// if n_p = 0, p belongs to the first front
			if(n[p] == 0)
			{
				P[p]->fitness = 0; // p rank = 1
				F[0]->push_back(p);  // F_1 = F_1 U {p}
			}
		}

		int i = 0; // i=1
		while(F[i]->size() != 0) // while Fi != {}
		{
			if(Component::debug)
				cout << "fastNonDominatedSort start i=" << i << " |F[i]|=" << F[i]->size() << endl;
			vector<int>* Q = new vector<int>;  // Q = {}

			//for each 'p' in 'Fi'
			for(unsigned k = 0; k < F[i]->size(); k++)
			{
				int p = F[i]->at(k);

				// for each 'q' in 'Sp'
				for(unsigned j = 0; j < S[p].size(); j++)
				{
					int q = S[p][j];

					// nq = nq - 1
					n[q]--;

					// if nq = 0
					if(n[q] == 0)
					{
						P[q]->fitness = i + 1;  // q rank = i+1
						Q->push_back(q);    // Q = Q U {q}
					}
				}

			}

			i++; // i = i + 1
			F.push_back(NULL);
			F[i] = Q; // Fi = Q
			if(Component::debug)
				cout << "fastNonDominatedSort i=" << i << " |Q|=" << Q->size() << endl;
		}

		// finished, P is updated
	}
};

}

#endif /*OPTFRAME_FITNESS_ASSIGNMENT_HPP_*/
