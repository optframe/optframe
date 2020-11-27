
#include "TSP-brkga.hpp" // implementation of TSP
#include <iostream>

#include <OptFrame/Heuristics/EvolutionaryAlgorithms/BRKGA.hpp>

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
using namespace TSP_brkga;

int
main()
{
   srand(0); // using system random (weak... just an example!)

   // load data into problem context 'pTSP'
   Scanner scanner{ "3\n1 10 10\n2 20 20\n3 30 30\n" };
   pTSP.load(scanner);
   std::cout << pTSP.dist << std::endl;

   BRKGA<ESolutionTSP::first_type, ESolutionTSP::second_type, double, ESolutionTSP> brkga(
     eprk, pTSP.n, 1000, 30, 0.4, 0.3, 0.6);

   std::cout << "FINISHED" << std::endl;
   return 0;
}