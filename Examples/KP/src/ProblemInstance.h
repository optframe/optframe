#ifndef KP_PROBLEMINSTANCE_H_
#define KP_PROBLEMINSTANCE_H_

#include <iostream>
#include <vector>

#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;
using namespace scannerpp;

namespace KP {
class ProblemInstance
{
public:
   int N;            // number of items
   vector<double> w; // weight of each item
   vector<double> p; // profit of each item
   double W;         // knapsack capacity

   ProblemInstance(Scanner& scanner);
   virtual ~ProblemInstance();
};
}

#endif /*KP_PROBLEMINSTANCE_H_*/
