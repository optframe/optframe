//#include <OptFCore/FCore.hpp> // included src

#include<iostream>

#include <OptFrame/myconcepts.h>
#include <OptFrame/Evaluation.hpp>

// simple test for package loading (on bazel)


int main()
{
   std::cout << "A" << std::endl;
   //
   optframe::Evaluation<int> ev;
   //
   return 0;
}
