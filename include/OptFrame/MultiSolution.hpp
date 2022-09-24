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

#ifndef OPTFRAME_MULTISOLUTION_HPP_
#define OPTFRAME_MULTISOLUTION_HPP_

#include <vector>

#include "Component.hpp"
#include "Solution.hpp"

namespace optframe {

template<XSolution S>
class MultiSolution : public Component
{
protected:
   vector<S*> p;

public:
   MultiSolution()
   {
   }

   MultiSolution(const MultiSolution& pop)
   {
      for (unsigned i = 0; i < pop.size(); i++)
         p.push_back(&pop.at(i).clone());
   }

   virtual ~MultiSolution()
   {
      clear();
   }

   unsigned size() const
   {
      return p.size();
   }

   S& at(unsigned c)
   {
      return (*p.at(c));
   }

   const S& at(unsigned c) const
   {
      return (*p.at(c));
   }

   void insert(unsigned pos, S& c)
   {
      p.insert(p.begin() + pos, new S(c));
   }

   void push_back(S* c)
   {
      if (c) // not null
         p.push_back(c);
   }

   void push_back(const S& c)
   {
      p.push_back(&c.clone());
   }

   S& remove(unsigned pos)
   {
      S& c = *p.at(pos);
      p.erase(p.begin() + pos);
      return c;
   }

   void add(const MultiSolution<S>& pop)
   {
      for (unsigned i = 0; i < pop.size(); i++) {
         const S& s = pop.at(i);
         push_back(s);
      }
   }

   // clear and kill
   void clear()
   {
      for (unsigned i = 0; i < p.size(); i++)
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

   virtual MultiSolution<S>& operator=(const MultiSolution<S>& p)
   {
      if (&p == this) // auto ref check
         return *this;

      unsigned sizePop = this->p.size();

      for (unsigned i = 0; i < sizePop; i++) {
         if (this->p.at(i)) // If no nullptr pointing.
         {
            delete this->p.at(i);
         }
      }

      this->p.clear();

      sizePop = p.size();

      for (unsigned i = 0; i < sizePop; i++) {
         if (&p.at(i)) // If no nullptr pointing.
         {
            this->p.push_back(new S(p.at(i)));
         } else {
            this->p.push_back(nullptr);
         }
      }

      return (*this);
   }

   virtual MultiSolution<S>& clone() const
   {
      return *new MultiSolution<S>(*this);
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":MultiSolution";
      return ss.str();
   }

   virtual string id() const override
   {
      return idComponent();
   }

   virtual void print() const
   {
      cout << "MultiSolution(" << p.size() << ")";
      cout << endl;

      for (unsigned i = 0; i < p.size(); i++) {
         p.at(i)->print();
      }
   }
};

}

#endif /* OPTFRAME_MULTISOLUTION_HPP_ */
