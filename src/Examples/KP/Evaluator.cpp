#include "Evaluator.h"

using namespace KP;

MyEvaluator::MyEvaluator(ProblemInstance& _pKP):
pKP(_pKP)
{
}
	
MyEvaluator::~MyEvaluator()
{
}

Evaluation<> MyEvaluator::evaluate(const ESolutionKP& s) 
{
   const RepKP& rep = s.getR();
    double fo = 0; // parcela viável
    for(int i=0; i<pKP.N; i++)
        if(rep[i] == 1)
            fo += pKP.p[i];    // lucros 'p'
            
    double sum_w = 0; // soma peso dos itens na mochila
    for(int i=0; i<pKP.N; i++)
        if(rep[i] == 1)
            sum_w += pKP.w[i]; // pesos 'w'
    
    double fo_inv = 0; // parcela inviável
    if(sum_w > pKP.W)
        fo_inv = -1000000*(sum_w - pKP.W); 
// penalizamos excesso na mochila por um fator de -1000000, ou seja, permitimos que soluções inviáveis sejam representadas, mas terão uma avaliação desinteressante         

    // armazenamos a avaliação (viável e inviável) em um objeto Evaluation
    return Evaluation<>(fo, fo_inv);
}


bool MyEvaluator::isMinimization() const
{
	// MAXIMIZATION
	return false;
}






