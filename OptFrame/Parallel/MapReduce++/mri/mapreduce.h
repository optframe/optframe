#ifndef MAPREDUCE_H
#define MAPREDUCE_H

///MapReduce.
template<class KeyA, class A, class KeyB, class B, class C>
class MapReduce
{
public:
	///MapReduce execution (implemented by library).
	virtual vector< pair<KeyB,C> > run( Mapper<KeyA,A,KeyB,B,C> & , Reducer<KeyA,A,KeyB,B,C> & , vector< pair<KeyA,A> > & ) = 0 ;
};

///StrMapReduce.
class StrMapReduce : public MapReduce<string,string,string,string,string> {};

#endif /* MAPREDUCE_H */
