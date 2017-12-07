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

#ifndef OPTFRAME_POPULATION_HPP_
#define OPTFRAME_POPULATION_HPP_

#include <vector>

#include "Solution.hpp"
#include "Evaluation.hpp"
#include "Evaluator.hpp"
#include "Component.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS>
class Population: public Component
{
protected:
	typedef Solution<R, ADS> chromossome;
	typedef vector<chromossome*> population;
	typedef vector<vector<Evaluation> > populationFitness;

	population p;
	populationFitness pFitness;
public:

	vector<double> fitness;

	Population()
	{
	}

	Population(const Population& pop)
	{
		for (unsigned i = 0; i < pop.size(); i++)
		{
			p.push_back(&pop.at(i).clone());
			fitness.push_back(0); // TODO: fix
			vector<Evaluation> a;
			pFitness.push_back(a);
		}
	}

	virtual ~Population()
	{
		clear();
	}

	unsigned size() const
	{
		return p.size();
	}

	chromossome& at(unsigned c)
	{
		return (*p.at(c));
	}

	const chromossome& at(unsigned c) const
	{
		return (*p.at(c));
	}

	void insert(unsigned pos, chromossome& c)
	{
		p.insert(p.begin() + pos, new chromossome(c));
		fitness.insert(fitness.begin() + pos, 0.0);
		vector<Evaluation> a;
		pFitness.insert(pFitness.begin() + pos, a);
	}

	void push_back(chromossome* c)
	{
		if (c) // not null
		{
			p.push_back(c);
			fitness.push_back(0);
			vector<Evaluation> a;
			pFitness.push_back(a);
		}
	}

	void push_back(const chromossome& c)
	{
		p.push_back(&c.clone());
		fitness.push_back(0);
		vector<Evaluation> a;
		pFitness.push_back(a);
	}

	void push_back(const chromossome& c, vector<Evaluation> chromossomeFitness)
	{
		p.push_back(&c.clone());
		fitness.push_back(0);
		pFitness.push_back(chromossomeFitness);
	}

	chromossome& remove(unsigned pos)
	{
		chromossome& c = *p.at(pos);
		p.erase(p.begin() + pos);
		fitness.erase(fitness.begin() + pos);
		pFitness.erase(pFitness.begin() + pos);
		return c;
	}

	vector<double> getFitness(int pos) const
	{
		return pFitness[pos];
	}

	double getSingleFitness(int pos) const
	{
		return fitness[pos];
	}

	void setFitness(unsigned i, double v)
	{
		fitness[i] = v;
	}

	void add(const Population<R, ADS>& pop)
	{
		for (unsigned i = 0; i < pop.size(); i++)
		{
			const chromossome& s = pop.at(i);
			push_back(s);
		}
	}

	// clear and kill
	void clear()
	{
		for (unsigned i = 0; i < p.size(); i++)
			delete p.at(i);

		p.clear();
		fitness.clear();
		pFitness.clear();

	}

	void clearNoKill()
	{
		p.clear();
		fitness.clear();
		pFitness.clear();
	}

	bool empty()
	{
		return p.empty();
	}

	// operates for Single Obj Populations
	// TODO: divide class in SOPop and MOPop
	void sort(bool isMin)
	{
		// basic selection sort
		for(int i=0; i<int(p.size())-1; i++)
		{
			int best = i;
			for(int j=i+1; j<int(p.size()); j++)
				if((isMin && fitness[j] < fitness[best]) || (!isMin && fitness[j] > fitness[best] ))
					best = j;
			// swap best
			if(best != i)
			{
				Solution<R>* si = p[i];
				p[i] = p[best];
				p[best] = si;

				double di = fitness[i];
				fitness[i] = fitness[best];
				fitness[best] = di;
			}
		}
	}

	virtual Population<R, ADS>& operator=(const Population<R, ADS>& p)
	{
		if (&p == this) // auto ref check
			return *this;

		unsigned sizePop = this->p.size();

		fitness = p.fitness;

		pFitness = p.pFitness;

		for (unsigned i = 0; i < sizePop; i++)
		{
			if (this->p.at(i)) // If no nullptr pointing.
			{
				delete this->p.at(i);
			}
		}

		this->p.clear();

		sizePop = p.size();

		for (unsigned i = 0; i < sizePop; i++)
		{
			if (&p.at(i)) // If no nullptr pointing.
			{
				this->p.push_back(new chromossome(p.at(i)));
			}
			else
			{
				this->p.push_back(nullptr);
			}
		}

		return (*this);
	}

	virtual Population<R, ADS>& clone() const
	{
		return *new Population<R, ADS>(*this);
	}

	static string idComponent()
	{
		stringstream ss;
		ss << Component::idComponent() << ":Population";
		return ss.str();
	}

	virtual string id() const
	{
		return idComponent();
	}

	virtual void print() const
	{
		cout << "Population(" << p.size() << ")";
		cout << endl;

		for (unsigned i = 0; i < p.size(); i++)
		{
			p.at(i)->print();
		}
	}

	chromossome& cloneBestChromossome(Evaluator<R, ADS>& eval)
	{
		vector<pair<Solution<R, ADS>, double> > v;

		for (int i = 0; i < p.size(); i++)
		{
			Evaluation& e = eval.evaluate(p[i]);
			v.push_back(make_pair(*p[i], e.evaluation()));
			delete &e;
		}

		int bestC = 0;
		for (int i = 0; i < (v.size() - 1); i++)
			if (eval.betterThan(v[i + 1].second, v[i].second))
				bestC = i + 1;

		return v[bestC].first;
	}

};

}

#endif /* OPTFRAME_POPULATION_HPP_ */
