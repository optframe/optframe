#ifndef KP2_PROBLEMINSTANCE_H_
#define KP2_PROBLEMINSTANCE_H_

#include <vector>
#include <random>

namespace KP2
{

struct Item{
	int id;
	int valor;
	int peso;
};

class ProblemInstance{
public:
	int n; //numero de items
	int limite; //limite da mochila
	std::vector<Item> items;
	std::mt19937 mersenne_twister;

	ProblemInstance(int seed = 0);

	~ProblemInstance() = default;
};

}

#endif /*KP2_PROBLEMINSTANCE_H_*/

