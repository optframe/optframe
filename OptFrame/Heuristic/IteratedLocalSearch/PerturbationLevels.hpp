#ifndef PERTURBATIONLEVELS_HPP_
#define PERTURBATIONLEVELS_HPP_

#include "Perturbation.hpp"
#include "HistoryLevels.h"

template<class R>
class PerturbationLevels : public Perturbation< R, levelHistory >
{
public:
    
    void exec (Solution<R>* s, levelHistory* history)
    {
		int iter  = history->first.first;  
		int level = history->first.second;
		int iterMax  = history->second.first;  	
    	//int levelMax = history->second.second;
    	
    	//cout << "level = " << level << " e iter = " << iter << endl;
    	
    	// nivel atual: 'level'
    	exec(level,s);
    	
    	// Incrementa a iteracao
    	iter++;
    	
    	if(iter>=iterMax)
    	{
    		iter = 0;
    		level++;
    		cout << "level "<<level<<".."<<endl;
    	}
    	
		// Atualiza o historico
		history->first.first = iter;
		history->first.second = level;
    };
    
    virtual void exec (int n, Solution<R>* s) = 0;    
};


#endif /*PERTURBATIONLEVELS_HPP_*/
