#include <algorithm>
#include <functional>
#include <iostream>

#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>

using namespace std;
using namespace optframe;
using namespace scannerpp;

namespace TSP_fcore {

class ProblemInstance
{
private:
   int n;
   Matrix<double> dist;

public:
   ProblemInstance(Scanner& scanner)
   {
      n = *scanner.nextInt();
      dist = Matrix<double>(n, n);
      //cout << n << endl;
      vector<double> xvalues(n);
      vector<double> yvalues(n);
      //
      for (int i = 0; i < n; i++) {
         scanner.next();
         xvalues[i] = *scanner.nextDouble(); // x
         yvalues[i] = *scanner.nextDouble(); // y
      }
      //
      for (int i = 0; i < n; i++)
         for (int j = 0; j < n; j++)
            dist(i, j) = distance(xvalues.at(i), yvalues.at(i), xvalues.at(j), yvalues.at(j));
   }
   //
   double distance(double x1, double y1, double x2, double y2)
   {
      return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
   }
};

//
using MyXSolution = Solution<std::vector<int>>;
using MyXESolution = std::pair<MyXSolution, Evaluation<int>>;
//

// this works when defining in global scope (same as 'class')
using MoveSwap = FMove<
  std::pair<int, int>,
  MyXESolution,
  [](const std::pair<int, int>& myData, MyXESolution& xes) -> op<std::pair<int, int>> {
     return make_optional(std::pair<int, int>(myData.second, myData.first));
  }>;

} // TSP_fcore

using namespace TSP_fcore;

int
main()
{
   Scanner scanner{ "3\n1 10\ 10\n2 20 20\n3 30 30\n" };
   ProblemInstance pTSP{ scanner };

   // swap 0 with 1
   MoveSwap move{ make_pair(0, 1) };

   std::cout << "FINISHED" << std::endl;
   return 0;
}