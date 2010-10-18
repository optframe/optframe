#ifndef PERTURBATION_HPP_
#define PERTURBATION_HPP_

#include "../../Solution.h"

template<class R, class H>
class Perturbation
{
public:    
    virtual Solution<R>* perturb (Solution<R>* s, H* history)
    {    	
    	// Implicit copy    	
    	Solution<R>* novo = s->clone();
    	exec(novo,history);
    	
    	return novo;
    }
	virtual void exec (Solution<R>* s, H* history) = 0;
};


#endif /*PERTURBATION_HPP_*/
