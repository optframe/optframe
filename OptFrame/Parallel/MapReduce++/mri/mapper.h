#ifndef MAPPER_H
#define MAPPER_H

///Mapper.
template<class KeyA, class A, class KeyB, class B, class C>
class Mapper 
{ 
public: 
	///Iterator mapping execution (implemented by library).
#ifndef MRI_USE_MULTIMAP
	virtual vector< pair<KeyB,B> > run( vector< pair<KeyA,A> > & ) = 0 ;
#else
	virtual multimap<KeyB,B> run( vector< pair<KeyA,A> > & ) = 0 ;
#endif
	///Map function (implemented by user).
	virtual vector< pair<KeyB,B> > map( pair<KeyA,A> ) = 0 ;
private:
	MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
};

///StrMapper.
class StrMapper : public Mapper<string,string,string,string,string> {};

#endif /* MAPPER_H */
