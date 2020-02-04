#include <iostream>
#include <iomanip>

#include "Evaluator.h"

#ifndef NDEBUG
#define EV_DEBUG 1 //uncomment this to debug this code
#endif

#ifdef EV_DEBUG
#define DEBUG
#else
#define DEBUG while(false)
#endif

using namespace KP2;

MyEvaluator::MyEvaluator(ProblemInstance& _pKP2) : pKP2(_pKP2){
}

optframe::Evaluation<> MyEvaluator::evaluate(const RepKP2& rep){
    constexpr double penalty = 10000.0;

	DEBUG{
		std::cerr << "Starting KP2 evaluator with ";
		for(const bool& b : rep)
			std::cerr << b;
		std::cerr << std::endl;
	}

    double fo = 0.0; // Evaluation<> Function Value
    double peso_total = 0;

    for(int i = 0; i < pKP2.n; ++i){ //para cada item na instancia 
    	if(rep[i]){ //se rep[i] for verdadeiro, entao o item está na mochila
    		fo += pKP2.items[i].valor; //adicionar o valor do item na funcao objetivo
    		peso_total += pKP2.items[i].peso; //adicionar o peso do item no total
    		DEBUG std::cerr << "item " << i << " is on the bag, it has value/weight " << pKP2.items[i].valor << " / " << pKP2.items[i].peso << " actual bag value/weight: " << fo << " / " << peso_total << std::endl;
    	}
    }

    if(peso_total > pKP2.limite){
        fo = fo/penalty;
        DEBUG std::cerr << "Solution has been penalized for surpassing the limit! " << std::endl;               
    } //penalizar sol!!

    DEBUG std::cerr << "Solution evaluated: " << fo << std::endl;

    return optframe::Evaluation(fo);
}


bool MyEvaluator::isMinimization() const{
	return false;
}
