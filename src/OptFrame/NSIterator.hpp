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

#ifndef OPTFRAME_NSITERATOR_HPP_
#define OPTFRAME_NSITERATOR_HPP_

#include "Move.hpp"

#include "Solution.hpp"

#include "Component.hpp"
//#include "Action.hpp"

using namespace std;

namespace optframe {

// Local Optimum Status
enum LOS
{
   los_yes,
   los_no,
   los_unknown,
   los_partial
};

// exemplo: mochila NSSeq de incrementos (+1).
// Comeca 'unknown'.
// Se nao pode mais incrementar nada vira 'yes'.
// Se perturba um pouquinho vira 'no' (pq sabe como voltar).

// 'partial' indicates that the solution is partially in a local optimum (maybe the word semi-local optimum is better...)
// it may happen for example in a VRP, where a route is in LO while others are not.

// TODO: unused?
class IteratorOutOfBound
{
private:
   int id;

public:
   IteratorOutOfBound(int _id)
     : id(_id)
   {
   }

   int getId()
   {
      return id;
   }
};

template<XESolution XES, XEvaluation XEv = typename XES::second_type, XESolution XSH = XES>
class NSIterator : public Component
{
public:
   virtual ~NSIterator()
   {
   }

   virtual void first() = 0;

   virtual void firstValid(const XES& se)
   {
      first();

      while (!isDone()) {
         uptr<Move<XES, XEv>> m = current();

         if (m && m->canBeApplied(se))
            break;

         next();
      }
   }

   virtual void next() = 0;

   virtual void nextValid(const XES& se)
   {
      next();

      while (!isDone()) {
         uptr<Move<XES, XEv>> m = current();

         if (m && m->canBeApplied(se))
            break;

         next();
      }
   }

   virtual bool isDone() = 0;
   virtual uptr<Move<XES, XEv>> current() = 0;

   // INSERT LOCAL OPTIMUM INFORMATION IN SOLUTION (IN ADS? USER DECIDES.)
   virtual void setLOS(LOS status, XES& s)
   {
   }

   static string idComponent()
   {
      stringstream ss;
      ss << Component::idComponent() << ":NSIterator";
      return ss.str();
   }

   virtual string id() const
   {
      return idComponent();
   }
};

}

#endif //OPTFRAME_NSITERATOR_HPP_
