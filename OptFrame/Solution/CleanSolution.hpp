#ifndef CLEANSOLUTION_HPP_
#define CLEANSOLUTION_HPP_

#include "../Solution.h"

#include <iostream>

using namespace std;

template<class R>
class CleanSolution : public Solution<R>
{
private:
	R* representacao;

public:

	CleanSolution(R* representacao);	
	~CleanSolution();

	//
	//Solution interface
	//

	Solution<R>* clone();	
	void print();

	R* getRepresentation();
	void setRepresentation(R*);

	//bool operator== (Solution<R>* s) const;
};


template<class R>
CleanSolution<R>::CleanSolution(R* representacao)
{
	this->representacao = representacao;
}

template<class R>
CleanSolution<R>::~CleanSolution<R>()
{
	if(representacao) // if not null
		delete representacao;
}

template<class R>
Solution<R>* CleanSolution<R>::clone()
{
	R* nova = new R(*representacao);
	return new CleanSolution<R>(nova);
}

template<class R>
void CleanSolution<R>::print()
{
	//cout << "Solution: " << *(representacao) << endl;
}

template<class R>
R* CleanSolution<R>::getRepresentation()
{
	return this->representacao;
}

template<class R>
void CleanSolution<R>::setRepresentation(R* r)
{
	this->representacao = r;
}

#endif /*CLEANSOLUTION_HPP_*/
