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

#ifndef OPTFRAME_POPULATION_HPP_
#define OPTFRAME_POPULATION_HPP_

#include "Solution.hpp"
#include "Evaluation.hpp"
#include "Evaluator.hpp"
#include <vector>

#include "Component.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class Population: public Component
{
protected:
	typedef Solution<R, ADS> chromossome;
	typedef vector<chromossome*> population;

	population p;

public:

	Population()
	{
	}

	Population(const Population& pop)
	{
		for(unsigned i = 0; i < pop.size(); i++)
			p.push_back(&pop.at(i).clone());
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
	}

	void push_back(chromossome* c)
	{
		if(c) // not null
			p.push_back(c);
	}

	void push_back(const chromossome& c)
	{
		p.push_back(&c.clone());
	}

	chromossome& remove(unsigned pos)
	{
		chromossome& c = *p.at(pos);
		p.erase(p.begin() + pos);
		return c;
	}

	void add(const Population<R, ADS, DS>& pop)
	{
		for(unsigned i = 0; i < pop.size(); i++)
		{
			const chromossome& s = pop.at(i);
			push_back(s);
		}
	}

	// clear and kill
	void clear()
	{
		for(unsigned i = 0; i < p.size(); i++)
			delete p.at(i);

		p.clear();
	}

	void clearNoKill()
	{
		p.clear();
	}

	bool empty()
	{
		return p.empty();
	}

	virtual Population<R, ADS>& operator=(const Population<R, ADS>& p)
	{
		if(&p == this) // auto ref check
			return *this;

		unsigned sizePop = this->p.size();

		for(unsigned i = 0; i < sizePop; i++)
		{
			if(this->p.at(i)) // If no NULL pointing.
			{
				delete this->p.at(i);
			}
		}

		this->p.clear();

		sizePop = p.size();

		for(unsigned i = 0; i < sizePop; i++)
		{
			if(&p.at(i)) // If no NULL pointing.
			{
				this->p.push_back(new chromossome(p.at(i)));
			}
			else
			{
				this->p.push_back(NULL);
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

		for(unsigned i = 0; i < p.size(); i++)
		{
			p.at(i)->print();
		}
	}

	chromossome& cloneBestChromossome(Evaluator<R, ADS, DS>& eval)
	{
		vector<pair<Solution<R, ADS>, double> > v;

		for(int i = 0; i < p.size(); i++)
		{
			Evaluation<DS>& e = eval.evaluate(p[i]);
			v.push_back(make_pair(*p[i], e.evaluation()));
			delete &e;
		}

		int bestC = 0;
		for(int i = 0; i < (v.size() - 1); i++)
			if(eval.betterThan(v[i + 1].second, v[i].second))
				bestC = i + 1;

		return v[bestC].first;
	}

};

}

#endif /* OPTFRAME_POPULATION_HPP_ */
