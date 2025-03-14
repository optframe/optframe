#include "ProblemInstance.h"

#include <OptFrame/printable/printable.hpp>

using namespace KP;
using namespace optframe;

ProblemInstance::ProblemInstance(Scanner& scanner)
{
   N = *scanner.nextInt();
   p = vector<double>(N);
   for (int i = 0; i < N; i++)
      p[i] = *scanner.nextDouble();
   w = vector<double>(N);
   for (int i = 0; i < N; i++)
      w[i] = *scanner.nextDouble();
   W = *scanner.nextDouble();

   std::cout << "N=" << N << std::endl;
   std::cout << "profit:" << toString(p) << std::endl;
   std::cout << "weight:" << toString(w) << std::endl;
   std::cout << "W=" << W << std::endl;
}

ProblemInstance::~ProblemInstance()
{
}
