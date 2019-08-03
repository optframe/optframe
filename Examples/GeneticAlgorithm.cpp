#include "GeneticAlgorithm.h"

using namespace KP2;

GeneticAlgorithm::GeneticAlgorithm(ProblemInstance& _inst, Evaluator& _eval, int _seed) : inst(_inst), eval(_eval), seed(_seed), mt(_seed){
	#ifdef GA_DEBUG
	std::cout << "Creating GA with seed: " << seed << std::endl;
	#endif
}

Solution GeneticAlgorithm::solver(int max_gen, int max_pop, double alpha, double omega){
	#ifdef GA_DEBUG
	std::cout << "Stating GA with max_gen: " << max_gen << " max_pop: " << max_pop << " alpha: " << alpha << " omega: " << omega << std::endl;
	#endif


	Population pop = initial_pop(max_pop);
	evaluate(pop);

	#ifdef PRINT
	auto next_gen_it = next_gen.begin();
	if(*next_gen_it == 0){
		std::cout << "Generation 0 population: " << std::endl;
		std::cout << pop << std::endl << "----------" << std::endl;
		++next_gen_it;
	}
	#endif

	for(int gen = 1; gen <= max_gen; ++gen){
		#ifdef GA_DEBUG
		std::cout << "Generation: " << gen << std::endl;
		#endif

		pop = select(pop, alpha);
		reproduct(pop, max_pop);
		mutate(pop, omega);
		evaluate(pop);

		#ifdef PRINT
		if(*next_gen_it == gen){
			std::cout << "Generation " << gen << " population: " << std::endl;
			std::cout << pop << std::endl << "----------" << std::endl;
			++next_gen_it;
		}
		#endif
	}

	Solution best_individual(std::move(pop.individuals[0].chromosome));
	return best_individual;
}

Population GeneticAlgorithm::initial_pop(int max_pop){
	#ifdef GA_DEBUG
	std::cout << "Creating inital population" << std::endl;
	#endif

	std::uniform_int_distribution<int> dist01(0, 1);
	Population pop;

	for(int i = 0; i < max_pop; ++i){
		Individual ind;
		for(int j = 0; j < inst.n; ++j)
			ind.chromosome.push_back((bool) dist01(mt));

		#ifdef GA_DEBUG
		std::cout << "Individual " << i << ": " << ind << std::endl;
		#endif

		pop.individuals.push_back(std::move(ind));
	}

	#ifdef GA_DEBUG
	std::cout << "Pop size: " << max_pop << " " << pop.individuals.size() << std::endl;
	#endif

	return pop;
}

void GeneticAlgorithm::evaluate(Population& pop){
	#ifdef GA_DEBUG
	std::cout << "Evaluating population" << std::endl;
	#endif

	std::vector<Individual>& indis = pop.individuals;
	for(int i = 0; i < pop.individuals.size(); ++i){
		Individual& ind = indis[i];
		if(!ind.evaluated){
			ind.fitness = eval.evaluate(ind.chromosome);
			ind.evaluated = true;
			#ifdef PRINT 
			ind.weight = eval.check_weight(ind.chromosome);
			#endif
		}
	}

	#ifdef GA_DEBUG
	std::cout << "Ranking population" << std::endl;
	#endif
	sort(indis.begin(), indis.end(), [](const Individual& a, const Individual& b){ return a.fitness > b.fitness; } );
	
	#ifdef GA_DEBUG
	std::cout << "Sorted population: " << std::endl;
	std::cout << pop << std::endl;
	#endif
}

Population GeneticAlgorithm::select(Population& pop, double alpha){
	#ifdef GA_DEBUG
	std::cout << "Starting select operation" << std::endl;
	#endif

	int size = ceil(pop.individuals.size()*alpha);
	Population new_pop;

	#ifdef GA_DEBUG
	std::cout << "Maintaining " << size << " most fit individuals" << std::endl;
	#endif

	for(int i = 0; i < size; ++i)
		new_pop.individuals.push_back(std::move(pop.individuals[i]));

	#ifdef GA_DEBUG
	std::cout << "New pop: " << std::endl;
	std::cout << new_pop << std::endl;
	#endif

	return new_pop;
}

void GeneticAlgorithm::reproduct(Population& pop, int max_pop){
	int p1, p2;
	int sum = 0, viable = 0;
	int cut;
	std::uniform_int_distribution<int> dist_cut(1, inst.n-1);


	#ifdef GA_DEBUG
	std::cout << "Starting reproduct operation: filling the population with " << max_pop-pop.individuals.size() << " offsprings" << std::endl;
	#endif

	for(int i = 0; i < pop.individuals.size(); ++i){
		if(pop.individuals[i].fitness >= 0){
			sum += pop.individuals[i].fitness;
			++viable;
		}
		else break; //pop is sorted
	}

	#ifdef GA_DEBUG
	std::cout << "Number of viable individuals: " << viable << " their accumulated fitness: " << sum << std::endl;
	#endif

	if(viable < 5){ //this is a special case; if this happens you should consider a higher max_pop (or higher alpha). If your parameters are high and this still happens, then the instance is very hard
		#ifdef GA_DEBUG
		std::cout << "Special case: using random operator to select parents" << std::endl;
		#endif

		std::uniform_int_distribution<int> dist(0, pop.individuals.size());

		for(int i = pop.individuals.size(); i < max_pop; i+=2){
			p1 = p2 = dist(mt);
			while(p1 == p2) p2 = dist(mt);

			Individual& parent1 = pop.individuals[p1];
			Individual& parent2 = pop.individuals[p2];
			cut = dist_cut(mt);
			Individual child1, child2;

			int k = 0;
			for(; k < cut; ++k){
				child1.chromosome.push_back(parent1.chromosome[k]);
				child2.chromosome.push_back(parent2.chromosome[k]);
			}
			for(;k < inst.n; ++k){
				child1.chromosome.push_back(parent2.chromosome[k]);
				child2.chromosome.push_back(parent1.chromosome[k]);
			}

			#ifdef GA_DEBUG
			std::cout << "Parent1: " << parent1 << std::endl;
			std::cout << "Parent2: " << parent2 << std::endl;
			std::cout << "Cut: " << cut << std::endl;
			std::cout << "Offspring1: " << child1 << std::endl;
			if(pop.individuals.size() < max_pop) std::cout << "Offspring2: " << child2 << std::endl;
			#endif

			pop.individuals.push_back(std::move(child1));
			if(pop.individuals.size() < max_pop) pop.individuals.push_back(std::move(child2));

			#ifdef GA_DEBUG
			std::cout << "New pop size: " << pop.individuals.size();
			#endif
		}
	}
	else{ //normal case, roulette
		#ifdef GA_DEBUG
		std::cout << "Normal case: using roulette operator to select parents" << std::endl;
		#endif

		std::uniform_int_distribution<int> dist_roulette(1, sum);
		int roll;
		for(int i = pop.individuals.size(); i < max_pop; i+=2){
			roll = dist_roulette(mt);
			p2 = p1 = 0;
			while(roll > pop.individuals[p1].fitness){
				roll -= pop.individuals[p1].fitness;
				++p1;
			}

			do{
				roll = dist_roulette(mt);
				p2 = 0;
				while(roll > pop.individuals[p2].fitness){
					roll -= pop.individuals[p2].fitness;
					++p2;
				}
			}while(p1 == p2);

			Individual& parent1 = pop.individuals[p1];
			Individual& parent2 = pop.individuals[p2];
			cut = dist_cut(mt);
			Individual child1, child2;

			int k = 0;
			for(; k < cut; ++k){
				child1.chromosome.push_back(parent1.chromosome[k]);
				child2.chromosome.push_back(parent2.chromosome[k]);
			}
			for(;k < inst.n; ++k){
				child1.chromosome.push_back(parent2.chromosome[k]);
				child2.chromosome.push_back(parent1.chromosome[k]);
			}

			#ifdef GA_DEBUG
			std::cout << "Parent1: " << parent1 << std::endl;
			std::cout << "Parent2: " << parent2 << std::endl;
			std::cout << "Cut: " << cut << std::endl;
			std::cout << "Offspring1: " << child1 << std::endl;
			if(pop.individuals.size() < max_pop) std::cout << "Offspring2: " << child2 << std::endl;
			#endif

			pop.individuals.push_back(std::move(child1));
			if(pop.individuals.size() < max_pop) pop.individuals.push_back(std::move(child2));

			#ifdef GA_DEBUG
			std::cout << "New pop size: " << pop.individuals.size() << std::endl;
			#endif
		}
	}

	#ifdef GA_DEBUG
	std::cout << "Population after reproduction: " << std::endl;
	std::cout << pop << std::endl;
	#endif
}

void GeneticAlgorithm::mutate(Population& pop, double omega){
	int mutants = ceil(pop.individuals.size()*omega);
	std::uniform_int_distribution<int> dist_who(0, pop.individuals.size()-1);
	std::uniform_int_distribution<int> dist_where(0, inst.n-1);

	#ifdef GA_DEBUG
	std::cout << "Starting mutate operation: will introduce " << mutants << " mutants into the population" << std::endl;
	#endif

	for(int i = 0; i < mutants; ++i){
		int who = dist_who(mt);
		int where = dist_where(mt);

		#ifdef GA_DEBUG
		std::cout << "Individual: " << pop.individuals[who] << " will be mutated at pos " << where << std::endl;
		#endif

		pop.individuals[who].chromosome[where] = !pop.individuals[who].chromosome[where];
		pop.individuals[who].evaluated = false; //may be optmized

		#ifdef GA_DEBUG
		std::cout << "After mutating: " << pop.individuals[who] << std::endl;
		#endif
	}
}

std::ostream& operator<<(std::ostream& os, const Individual& ind){
	for_each(ind.chromosome.begin(), ind.chromosome.end(), [&os](const bool x){ os << x; });

	#ifdef GA_DEBUG
	os << " -- " << ind.fitness << " -- " << ind.evaluated;
	#elif defined(PRINT)
	os << " --\t" << ind.fitness << "\t--\t" << ind.weight;
	#else
	os << " -- " << ind.fitness;
	#endif

	return os;
}

std::ostream& operator<<(std::ostream& os, const Population& pop){
	#ifdef PRINT
	int sum = 0, viable = 0;
	#endif
	for(int i = 0; i < pop.individuals.size(); ++i){
		os << pop.individuals[i] << std::endl;
		#ifdef PRINT
		if(pop.individuals[i].fitness > -1){
			sum += pop.individuals[i].fitness;
			++viable;
		}
		#endif
	}
	#ifdef GA_DEBUG
	os << "Pop size: " << pop.individuals.size();
	#elif defined(PRINT)
	os << "Avg (viable only): " << sum/viable << std::endl;
	#endif
	return os;
}

