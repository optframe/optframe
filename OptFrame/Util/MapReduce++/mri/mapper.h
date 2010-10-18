#ifndef MAPPER_H
#define MAPPER_H

///Mapper.
template<class KeyA, class A, class KeyB, class B, class C>
class Mapper 
{ 
public: 
	///Iterator mapping execution (implemented by library).
	virtual multimap<KeyB,B> run( multimap<KeyA,A> & ) = 0 ; 
	///Map function (implemented by user).
	virtual multimap<KeyB,B> map( pair<KeyA,A> ) = 0 ;
private:
	MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
};

///StrMapper.
class StrMapper : public Mapper<string,string,string,string,string> {};

#endif /* MAPPER_H */
