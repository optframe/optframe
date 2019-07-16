#include "ProblemInstance.h"

#include "../../OptFrame/Util/printable.h"

using namespace KP;
using namespace optframe;

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
    cout << "profit:" << toString(p) << endl;
    cout << "weight:" << toString(w) << endl;
    cout << "W=" << W << endl;
}
	
ProblemInstance::~ProblemInstance()
{
}
