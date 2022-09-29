// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_EPOPULATION_HPP_
#define OPTFRAME_EPOPULATION_HPP_

#include <vector>

#include "Component.hpp"
#include "Evaluation.hpp"
#include "Evaluator.hpp"
#include "Solution.hpp"

namespace optframe {

//
// EPopulation class is 'final' (WHY??? NOT ANYMORE...)
//
template<XESolution XES> // XES only for evaluation purposes!
class EPopulation : public Component
{
   using S = typename XES::first_type;
   using XEv = typename XES::second_type;

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

   // ============ push_back( S )

   void push_back(const S& cs)
   {
      // copy constructor is required
      p.push_back(new XES(cs, XEv{}));
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

   virtual string id() const override
   {
      return idComponent();
   }

   virtual void print() const
   {
      std::cout << toString() << std::endl;
   }

   virtual std::string toString() const override
   {
      std::stringstream ss;
      ss << "EPopulation(" << p.size() << ")";
      ss << endl;

      for (unsigned i = 0; i < p.size(); i++) {
         //p.at(i)->print();
         ss << *p.at(i) << std::endl;
      }
      return ss.str();
   }

}; // class EPopulation

} // namespace optframe

// compilation tests
// TODO: only add if not including "printable.h"
//static_assert(X2ESolution<EPopulation<std::pair<Solution<double>, Evaluation<double>>>, std::pair<Solution<double>, Evaluation<double>>>);

#endif /* OPTFRAME_POPULATION_HPP_ */
