#include "../../Examples/SCP/ConstructiveConstructiveTest.h"

using namespace SCP;

ConstructiveConstructiveTest::ConstructiveConstructiveTest(ProblemInstance& _pSCP): 
        pSCP(_pSCP)
{
}
	
ConstructiveConstructiveTest::~ConstructiveConstructiveTest()
{
}

Solution<RepSCP, MY_ADS>& ConstructiveConstructiveTest::generateSolution()
{
    RepSCP newRep;

    return * new TestSolution<RepSCP, MY_ADS>(newRep);			
}
