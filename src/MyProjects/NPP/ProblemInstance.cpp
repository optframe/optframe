#include "ProblemInstance.h"

using namespace NPP;

ProblemInstance::ProblemInstance(Scanner& scanner)
{
    this->size = scanner.nextInt();
    for (int i = 0; i < this->size; ++i) {
        this->numbers.push_back(scanner.nextInt());
    }
}
	
ProblemInstance::~ProblemInstance()
{
}