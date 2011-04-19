#ifndef OPTFRAME_NSSEQ_HPP_
#define OPTFRAME_NSSEQ_HPP_

#include "NS.hpp"
#include "NSIterator.hpp"

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class NSSeq: public NS<R, M>
{
public:

	using NS<R, M>::move; // prevents name hiding

	virtual ~NSSeq()
	{
	}

	virtual Move<R, M>& move(const R&) = 0;

	virtual NSIterator<R, M>& getIterator(const R&) = 0;
	virtual NSIterator<R, M>& getIterator(const M&, const R& r)
	{
		return getIterator(r);
	}

	virtual void print() = 0;
};

#endif /*OPTFRAME_NSSEQ_HPP_*/
