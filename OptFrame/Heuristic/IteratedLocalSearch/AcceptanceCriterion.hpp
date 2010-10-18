#ifndef ACCEPTANCECRITERION_HPP_
#define ACCEPTANCECRITERION_HPP_

#include "../../Solution.h"

template<class R, class H>
class AcceptanceCriterion
{
public:    
    virtual Solution<R>* accept (Solution<R>* s, Solution<R>* s1, H* history)
    {
    	Solution<R>* sStar = exec(s,s1,history);
    	
    	// Implicit copy
    	return sStar->clone();
    } 
	virtual Solution<R>* exec (Solution<R>* s, Solution<R>* s1, H* history) = 0;
};

#endif /*ACCEPTANCECRITERION_HPP_*/
