#include "KP2.h"

using namespace KP2;

int main(int argc, char **argv){
	int seed = (argc > 2) ? atoi(argv[1]) : 0;
	ProblemInstance inst; //read input
	Evaluator eval(inst);
	GeneticAlgorithm ga(inst, eval, seed);

	#ifdef PRINT
	for(int i = 2; i < argc; ++i) ga.next_gen.push_back(atoi(argv[i]));
		ga.next_gen.push_back(-1);
	#endif

	Solution sol = ga.solver();
	std::cout << sol << std::endl << eval.evaluate(sol) << std::endl;

	return 0;
}