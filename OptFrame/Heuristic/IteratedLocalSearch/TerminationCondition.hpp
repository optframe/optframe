#ifndef TERMINATIONCONDITION_HPP_
#define TERMINATIONCONDITION_HPP_

template<class H>
class TerminationCondition
{
public:
    // Default implementation
    virtual bool terminate (H* history)
    {
    	return false;
    }
};

#endif /*TERMINATIONCONDITION_HPP_*/
