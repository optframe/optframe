#ifndef OPTFRAME_NSITERATOR_HPP_
#define OPTFRAME_NSITERATOR_HPP_

#include "Move.hpp"

using namespace std;

class IteratorOutOfBound
{
private:
   int id;
public:
   IteratorOutOfBound(int _id) :
      id(_id)
   {
   }

   int getId()
   {
      return id;
   }
};

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class NSIterator
{
public:
   virtual ~NSIterator()
   {
   }

   virtual void first() = 0;
   virtual void next() = 0;
   virtual bool isDone() = 0;
   virtual Move<R, M>& current() = 0;
};


#endif //OPTFRAME_NSITERATOR_HPP_
