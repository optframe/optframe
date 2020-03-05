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

#ifndef OPTFRAME_NSENUMITERATOR_HPP_
#define OPTFRAME_NSENUMITERATOR_HPP_

#include "Move.hpp"

using namespace std;

namespace optframe
{


template<XESolution XES, XEvaluation XEv, XESolution XSH> class NSEnum;

template<XESolution XES, XEvaluation XEv = Evaluation<>, XESolution XSH = XES>
class NSEnumIterator: public NSIterator<XES, XEv, XSH>
{
private:
   NSEnum<XES, XEv, XSH>& ns;
   unsigned int move;
   unsigned int nsSize;

public:

   NSEnumIterator(NSEnum<XES, XEv, XSH>& _ns) :
      ns(_ns)
   {
      move = 0;
      nsSize = _ns.size();
   }

   virtual ~NSEnumIterator()
   {
   }

   void first() override
   {
      move = 0;
   }

   void next() override
   {
      move++;
   }

   bool isDone() override
   {
      return move >= nsSize;
   }

   uptr<Move<XES, XEv>> current() override
   {
      if (isDone())
         throw IteratorOutOfBound(move);
      return ns.indexMove(move);
   }

   uptr<Move<XES, XEv>> at(unsigned int m)
   {
      // TODO: throw exception if m >= size
      return ns.move(m);
   }

   unsigned int size()
   {
      return nsSize;
   }
};

}

#endif // OPTFRAME_NSENUMITERATOR_HPP_
