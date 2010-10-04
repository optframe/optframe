#ifndef OPTFRAME_MOVE_HPP_
#define OPTFRAME_MOVE_HPP_

#include "Solution.hpp"
#include "Evaluation.hpp"

using namespace std;

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class Move
{
public:

	virtual ~Move()
	{
	}

	bool canBeApplied(const Solution<R>& s)
	{
		return canBeApplied(s.getR());
	}

	bool canBeApplied(const Evaluation<M>& e, const Solution<R>& s)
	{
		return canBeApplied(e.getM(), s.getR());
	}

	virtual bool canBeApplied(const R&) = 0;

	virtual bool canBeApplied(const M& m, const R& r)
	{
		return canBeApplied(r);
	}

	Move<R, M>& apply(Solution<R>& s)
	{
		return apply(s.getR());
	}

	Move<R, M>& apply(Evaluation<M>& e, Solution<R>& s)
	{
		return apply(e.getM(), s.getR());
	}

	virtual Move<R, M>& apply(R& r) = 0;

	virtual Move<R, M>& apply(M& m, R& r)
	{
		return apply(r);
	}

	virtual bool operator==(const Move<R, M>& m) const = 0;

	bool operator!=(const Move<R, M>& m) const
	{
		return !(*this == m);
	}

	virtual void print() = 0;
};

#endif /*OPTFRAME_MOVE_HPP_*/
