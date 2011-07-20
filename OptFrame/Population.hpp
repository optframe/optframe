// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class Population
{
protected:
   typedef Solution<R> chromossome;
   typedef vector<chromossome*> population;
   //typedef vector<const chromossome*> constPopulation;

   population p;

public:

   Population()
   {
   }

   Population(chromossome& c)
   {
      p.push_back(c);
   }

   Population(Population& pop) :
      p(pop.p)
   {
   }

   Population(const Population& pop) :
      p(pop.p)
   {
   }

   virtual ~Population()
   {
      unsigned sizePop = p.size();

      for (unsigned i = 0; i < sizePop; i++)
      {
         delete p.at(i);
      }
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

   void push_back(chromossome& c)
   {
      p.push_back(new chromossome(c));
   }

   chromossome& remove(unsigned pos)
   {
      chromossome *c = new chromossome(*p.at(pos));
      p.erase(p.begin() + pos);
      return *c;
   }

   void clear()
   {
      p.clear();
   }

   bool empty()
   {
      return p.empty();
   }

   virtual Population<R>& operator=(const Population<R>& p)
   {
      if (&p == this) // auto ref check
         return *this;

      unsigned sizePop = this->p.size();

      for (unsigned i = 0; i < sizePop; i++)
      {
         if (this->p.at(i)) // If no NULL pointing.
         {
            delete this->p.at(i);
         }
      }

      this->p.clear();

      sizePop = p.size();

      for (unsigned i = 0; i < sizePop; i++)
      {
         if (&p.at(i)) // If no NULL pointing.
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

   virtual Population<R>& clone() const
   {
      return *new Population<R> (*this);
   }

   virtual void print() const
   {
      cout << "Population's printing:" << endl;

      for (unsigned int i = 0; i < p.size(); i++)
      {
         p.at(i)->print();
      }
   }

	chromossome& cloneBestChromossome(Evaluator<R, M>& eval)
	{
		vector<pair<Solution<R> , double> > v;

		for (int i = 0; i < p.size(); i++)
		{
			Evaluation<M>& e = eval.evaluate(p[i]);
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

#endif /* OPTFRAME_POPULATION_HPP_ */
