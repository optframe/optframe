#include "ProblemInstance.h"

using namespace KP2;

ProblemInstance::ProblemInstance() {
	#ifdef PI_DEBUG
	std::cout << "Reading the input" << std::endl;
	#endif
	std::cin >> n >> max_v;
	#ifdef PI_DEBUG
	std::cout << "n: " << n << " max_v: " << max_v << std::endl;
	#endif

	for(int i = 0; i < n; ++i){
		int vi, wi;
		std::cin >> vi >> wi;
		value.push_back(vi);
		weight.push_back(wi);

		#ifdef PI_DEBUG
		std::cout << "Item: " << i << " vi: " << vi << " wi: " << wi << std::endl;
		#endif
	}
}