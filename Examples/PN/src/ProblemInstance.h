#ifndef PN_PROBLEMINSTANCE_H_
#define PN_PROBLEMINSTANCE_H_

#include <iostream>

#include <OptFrame/Component.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>

using namespace std;
using namespace scannerpp;
using namespace optframe;

namespace PN {

class ProblemInstance : public Component
{
private:
   // Your private vars

public:
   int size;
   vector<double> nums;
   ProblemInstance(Scanner& scanner);

   virtual ~ProblemInstance();

   virtual std::string toString() const
   {
      return "PROBLEM_PN";
   }
};

}

#endif /*PN_PROBLEMINSTANCE_H_*/
