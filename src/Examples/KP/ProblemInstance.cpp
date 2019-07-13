#include "ProblemInstance.h"

using namespace KP;

ProblemInstance::ProblemInstance(Scanner& scanner)
{
    N = scanner.nextInt();
    p = vector<double>(N);
    for(int i=0; i<N; i++)
       p[i] = scanner.nextDouble();
    w = vector<double>(N);
    for(int i=0; i<N; i++)
       w[i] = scanner.nextDouble();
    W = scanner.nextDouble();

    cout << "N=" << N << endl;
    cout << "profit:" << p << endl;
    cout << "weight:" << w << endl;
    cout << "W=" << W << endl;
}
	
ProblemInstance::~ProblemInstance()
{
}
