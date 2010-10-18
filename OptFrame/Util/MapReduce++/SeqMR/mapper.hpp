#ifndef MAPPER_HPP
#define MAPPER_HPP
#include <iterator>

///SeqMR_Mapper.
template<class KeyA, class A, class KeyB, class B, class C>
class SeqMR_Mapper : public Mapper<KeyA,A,KeyB,B,C>
{ 
public:
	///Constructor.
	SeqMR_Mapper(MapReduce<KeyA,A,KeyB,B,C> * _mapReduce):mapReduce(_mapReduce){};
	///Iterator mapping execution (implemented by library).
	virtual multimap<KeyB,B> run( multimap<KeyA,A> & as )
	{
		multimap<KeyB,B> bs;
		typename multimap<KeyA,A>::iterator itAs;
		for (itAs = as.begin() ; itAs != as.end() ; ++itAs) 
		{
			multimap<KeyB,B> bss = map( *itAs );
			typename multimap<KeyB,B>::iterator itBss;
			for (itBss = bss.begin() ; itBss != bss.end() ; ++itBss) bs.insert( *itBss ); 
		}
		return bs;
	};
	///Map function (implemented by user).
	virtual multimap<KeyB,B> map( pair<KeyA,A> ) = 0 ;
protected:
	MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
};

///SeqMR_StrMapper.
class SeqMR_StrMapper : public SeqMR_Mapper<string,string,string,string,string> 
{
public:
	///Constructor.
	SeqMR_StrMapper(MapReduce<string,string,string,string,string> * _mapReduce)
		:SeqMR_Mapper<string,string,string,string,string>(_mapReduce) {};	
};

#endif /* MAPPER_HPP */
