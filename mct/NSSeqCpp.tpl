#include "NSSeq$neighborhood.h"

using namespace $project;

// ============ Move$neighborhood ==============

Move$neighborhood::Move$neighborhood()
{
}

Move$neighborhood::~Move$neighborhood()
{
}

bool Move$neighborhood::canBeApplied(const Rep$project& rep, const MY_ADS&)
{
    return true;
}

Move< Rep$project , MY_ADS $commadproject >& Move$neighborhood::apply(Rep$project& rep, MY_ADS&)
{
    // apply this move to 'rep'
    // rep. (...) = (...)
    // return reverse move
    return * new Move$neighborhood; 
}

bool Move$neighborhood::operator==(const Move< Rep$project , MY_ADS $commadproject >& _m) const
{
    const Move$neighborhood& m = (const Move$neighborhood&) _m;
    return false;
}

void Move$neighborhood::print() const
{
    cout << id() << "with params: '" << "" << "'" << endl;
}

string Move$neighborhood::id() const
{
    return Move<Rep$project , MY_ADS $commadproject>::idComponent().append(":Move$neighborhood");
}


// ============ NSIterator$neighborhood ==============

NSIterator$neighborhood::NSIterator$neighborhood()
{
}

NSIterator$neighborhood::~NSIterator$neighborhood()
{
}

void NSIterator$neighborhood::first(){};

void NSIterator$neighborhood::next(){};
	
bool NSIterator$neighborhood::isDone(){};
	
Move< Rep$project , MY_ADS $commadproject >& NSIterator$neighborhood::current(){};


// ============ NSSeq$neighborhood ==============

NSSeq$neighborhood::NSSeq$neighborhood()
{
}

NSSeq$neighborhood::~NSSeq$neighborhood()
{
}

Move<Rep$project , MY_ADS $commadproject>& NSSeq$neighborhood::move(const Rep$project& rep, const MY_ADS&)
{
   return * new Move$neighborhood; // return a random move
}

NSIterator<Rep$project , MY_ADS $commadproject>& NSSeq$neighborhood::getIterator(const Rep$project& rep, const MY_ADS&)
{
    return * new NSIterator$neighborhood; // return an iterator to the neighbors of 'rep' 
}

void NSSeq$neighborhood::print() const
{
}
