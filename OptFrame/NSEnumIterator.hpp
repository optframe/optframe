#ifndef OPTFRAME_NSENUMITERATOR_HPP_
#define OPTFRAME_NSENUMITERATOR_HPP_

#include "Move.hpp"

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY> class NSEnum;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class NSEnumIterator: public NSIterator<R, M>
{
private:
   NSEnum<R, M>& ns;
   unsigned int move;
   unsigned int nsSize;

public:

   NSEnumIterator(NSEnum<R, M>& _ns) :
      ns(_ns)
   {
      move = 0;
      nsSize = _ns.size();
   }

   virtual ~NSEnumIterator()
   {
   }

   void first()
   {
      move = 0;
   }

   void next()
   {
      move++;
   }

   bool isDone()
   {
      return move >= nsSize;
   }

   Move<R, M>& current()
   {
      if (isDone())
         throw IteratorOutOfBound(move);
      return ns.move(move);
   }

   Move<R, M>& at(unsigned int m)
   {
      // TODO: throw exception if m >= size
      return ns.move(m);
   }

   unsigned int size()
   {
      return nsSize;
   }
};

#endif // OPTFRAME_NSENUMITERATOR_HPP_
