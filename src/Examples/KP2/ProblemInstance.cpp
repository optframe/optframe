#include <iostream>
#include <chrono>

#include "ProblemInstance.h"

#ifndef NDEBUG
#define PI_DEBUG 1 //uncomment this to debug this code
#endif

#ifdef PI_DEBUG 
#define DEBUG
#else
#define DEBUG while(false)
#endif

using namespace KP2;

ProblemInstance::ProblemInstance(int seed){
	DEBUG std::cerr << "Reading instance from std::cin " << std::endl;

	std::cin >> n >> limite;
	for(int i = 0; i < n; ++i){
		Item item;
		std::cin >> item.valor >> item.peso;
		item.id = i;
		items.emplace_back(item);
	}

	DEBUG std::cerr << "Setting seed: " << seed << std::endl;
	mersenne_twister.seed(!seed ? seed : std::chrono::steady_clock::now().time_since_epoch().count());

	DEBUG{
		std::cerr << "Problem Instance sucessfully finished: " << std::endl;
		std::cerr << "Number of items: " << n << " Limit: " << limite << " Item list as value/weight: " <<  std::endl;

		for(const Item& item : items)
			std::cerr << item.valor << " " << item.peso << std::endl;
		std::cerr << std::endl;
	}	
}