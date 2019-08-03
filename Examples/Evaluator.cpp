#include "Evaluator.h"

using namespace KP2;

Evaluator::Evaluator(ProblemInstance& _inst) : inst(_inst){
}

int Evaluator::evaluate(const std::vector<bool>& item){
	#ifdef EV_DEBUG
	std::cout << "Evaluating item: ";
	for_each(item.begin(), item.end(), [](const bool x){ std::cout << x; });
	std::cout << std::endl;
	#endif

	int total_value = 0;
	int total_weight = 0;
	for(int i = 0; i < inst.n; ++i){
		if(item[i]){
			total_value += inst.value[i];
			total_weight += inst.weight[i];
		}
	}
	if(total_weight > inst.max_v) total_value -= BIG_M;

	#ifdef EV_DEBUG
	std::cout << "Evaluation: " << total_value << std::endl;
	#endif

	return total_value;
}

int Evaluator::evaluate(const Solution& sol){
	return evaluate(sol.item);
}

int Evaluator::check_weight(const std::vector<bool>& item){
	int total_weight = 0;
	for(int i = 0; i < inst.n; ++i){
		if(item[i])
			total_weight += inst.weight[i];
	}
	return total_weight;
}