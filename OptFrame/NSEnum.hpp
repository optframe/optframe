#ifndef OPTFRAME_NSENUM_HPP_
#define OPTFRAME_NSENUM_HPP_

#include "NSSeq.hpp"
#include "RandGen.hpp"

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY> class NSEnum;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class NSEnumIterator: public NSIterator<R, M>
{
private:
	NSEnum<R, M>& ns;
	unsigned int move;
	unsigned int nsSize;

public:

	NSEnumIterator(NSEnum<R, M>& _ns) :
		ns(_ns)
	{
		move = 0;
		nsSize = _ns.size();
	}

	virtual ~NSEnumIterator()
	{
	}

	void first()
	{
		move = 0;
	}

	void next()
	{
		move++;
	}

	bool isDone()
	{
		return move >= nsSize;
	}

	Move<R, M>& current()
	{
		if (isDone())
			throw IteratorOutOfBound(move);
		return ns.move(move);
	}

	Move<R, M>& at(unsigned int m)
	{
		// TODO: throw exception if m >= size
		return ns.move(m);
	}

	unsigned int size()
	{
		return nsSize;
	}
};

template<class R, class M> // M = OPTFRAME_DEFAULT_MEMORY already defined
class NSEnum: public NSSeq<R, M>
{
public:

	using NSSeq<R, M>::move; // prevents name hiding

	virtual ~NSEnum()
	{
	}

	virtual Move<R, M>& move(const R&)
	{
		return move(rand() % size());
	}

	virtual NSIterator<R, M>& getIterator(const R&)
	{
		return *new NSEnumIterator<R, M> (*this);
	}

	virtual Move<R, M>& move(unsigned int m) = 0;

	virtual unsigned int size() = 0;

	virtual void print() = 0;
};

#endif /*OPTFRAME_NSENUM_HPP_*/
