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

#ifndef OPTFRAME_EPOPULATION_HPP_
#define OPTFRAME_EPOPULATION_HPP_

#include <vector>

#include "Component.hpp"
#include "Evaluation.hpp"
#include "Evaluator.hpp"
#include "Solution.hpp"

namespace optframe {

//
// EPopulation class is 'final'
//
template<XESolution XES> // XES only for evaluation purposes!
class EPopulation final : public Component
{
   using S = typename XES::first_type;
   using XEv = typename XES::first_type;

protected:
   std::vector<XES*> p;

public:
   EPopulation()
   {
   }

   EPopulation(const EPopulation& epop)
   {
      for (unsigned i = 0; i < epop.size(); i++)
         p.push_back(new XES(epop.at(i))); // implicit copy constructor
   }

   // move constructor ('steal' population from dying epop)
   EPopulation(EPopulation&& epop)
     : p{ epop.p }
   {
   }

   virtual ~EPopulation()
   {
      clear();
   }

   unsigned size() const
   {
      return p.size();
   }

   // supports X2ESolution concept standard
   XES& at(unsigned c)
   {
      return (*p.at(c));
   }

   // supports X2ESolution concept standard
   const XES& at(unsigned c) const
   {
      return (*p.at(c));
   }

   void push_back(XES* c)
   {
      if (c) // not null
         p.push_back(c);
   }

   void push_back(const XES& c)
   {
      // copy constructor is required
      p.push_back(new XES(c));
   }

   // chromossome is near dying... take everything and drop the corpse!!
   void push_back(XES&& c)
   {
      p.push_back(new XES(std::move(c)));
   }

   XEv& getFitness(int pos)
   {
      return p.at(pos)->second;
   }

   void setFitness(unsigned pos, const XEv& v)
   {
      p.at(pos)->second = v;
   }

   // clear and kill
   void clear()
   {
      for (unsigned i = 0; i < p.size(); i++)
         delete p.at(i);
      p.clear();
   }

   virtual EPopulation<XES>& operator=(const EPopulation<XES>& epop)
   {
      if (&epop == this) // auto ref check
         return *this;

      clear(); // this is NOT p.clear... pay attention!

      for (unsigned i = 0; i < epop.p.size(); i++)
         p.push_back(new XES(*epop.p[i])); // implicit copy constructor over XES

      return (*this);
   }

   // assignment from dying epop
   virtual EPopulation<XES>& operator=(EPopulation<XES>&& epop)
   {
      clear(); // this is NOT vector p.clear... pay attention!
      p = std::move(epop.p);
      epop.p.clear(); // this is vector clear
      return (*this);
   }

   virtual EPopulation<XES>& clone() const
   {
      return *new EPopulation<XES>(*this);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":EPopulation";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual void print() const
   {
      cout << "EPopulation(" << p.size() << ")";
      cout << endl;

      for (unsigned i = 0; i < p.size(); i++) {
         //p.at(i)->print();
         std::cout << *p.at(i) << std::endl;
      }
   }

}; // class EPopulation

} // namespace optframe

// compilation tests
static_assert(X2ESolution<EPopulation<std::pair<Solution<double>, Evaluation<double>>>, std::pair<Solution<double>, Evaluation<double>>>);

#endif /* OPTFRAME_POPULATION_HPP_ */
