#ifndef TESTMOVE_HPP_
#define TESTMOVE_HPP_

#include "../Move.hpp"

#include <cstdlib>
#include <iostream>

template< class R, class M = OPTFRAME_DEFAULT_MEMORY >
class TestMove : public Move< R, M >
{
private:
   static const unsigned long long MAX_MOVE_IN_MEMORY_ERROR = 100000;
   static unsigned long long MAX_MOVE_IN_MEMORY_WARNING;

   static unsigned long long testmove_objects;
   static unsigned long long testmove_objects_nodecrement;

   unsigned long long testmove_number;

public:
   TestMove () :
      Move< R, M > ()
   {
      testmove_objects++;
      testmove_objects_nodecrement++;
      check();

      testmove_number = testmove_objects_nodecrement;
   }

   virtual ~TestMove ()
   {
      testmove_objects--;
   }

   void check () // check number of TestMove objects in memory
   {
      if (testmove_objects >= MAX_MOVE_IN_MEMORY_WARNING)
      {
         cout << "WARNING: " << TestMove< R, M >::testmove_objects << " TestMove objects in memory!" << endl;
         TestMove< R, M >::MAX_MOVE_IN_MEMORY_WARNING++;
      }

      if (testmove_objects >= MAX_MOVE_IN_MEMORY_ERROR)
      {
         cout << "ERROR: " << TestMove< R, M >::testmove_objects << " TestMove objects in memory!" << endl;
         cout << "MAX_MOVE_IN_MEMORY_ERROR = " << MAX_MOVE_IN_MEMORY_ERROR << endl;
         cout << "aborting...";
         exit(1);
      }
   }

   void print () const
   {
      cout << "TestMove #" << testmove_number << " (" << testmove_objects << " in memory now): \n";
   }

   TestMove< R, M >& operator= (const TestMove< R, M >& m)
   {
      if (&m == this) // auto ref check
      return *this;

      *this = Move< R, M >::operator=(m);

      // do not copy the 'testmove_number'

      return *this;
   }

   Move< R, M >& operator= (const Move< R, M >& m)
   {
      return operator=((const TestMove< R, M >&) m);
   }

   Move< R, M >& clone () const
   {
      Move< R, M >* m = new TestMove< R, M > (*this);
      return (*m);
   }
};

template< class R, class M >
unsigned long long TestMove< R, M >::MAX_MOVE_IN_MEMORY_WARNING = 0.7 * MAX_MOVE_IN_MEMORY_ERROR;

template< class R, class M >
unsigned long long TestMove< R, M >::testmove_objects = 0;

template< class R, class M >
unsigned long long TestMove< R, M >::testmove_objects_nodecrement = 0;

#endif /* TESTMOVE_HPP_ */
