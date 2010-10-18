#ifndef OPTFRAME_POPULATION_HPP_
#define OPTFRAME_POPULATION_HPP_

#include "Solution.hpp"
#include <vector>

template<class R>
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
      chromossome *c = new chromossome(p.at(pos));
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

      for (int i = 0; i < p.size(); i++)
      {
         p.at(i)->print();
      }
   }

};

#endif /* OPTFRAME_POPULATION_HPP_ */
