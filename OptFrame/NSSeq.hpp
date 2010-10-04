#ifndef OPTFRAME_NSSEQ_HPP_
#define OPTFRAME_NSSEQ_HPP_

#include "NS.hpp"

using namespace std;

class IteratorOutOfBound
{
private:
	int id;
public:
	IteratorOutOfBound(int _id) :
		id(_id)
	{
	}

	int getId()
	{
		return id;
	}
};

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class NSIterator
{
public:
	virtual ~NSIterator()
	{
	}

	virtual void first() = 0;
	virtual void next() = 0;
	virtual bool isDone() = 0;
	virtual Move<R, M>& current() = 0;
};

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

	virtual void print() = 0;
};

#endif /*OPTFRAME_NSSEQ_HPP_*/
