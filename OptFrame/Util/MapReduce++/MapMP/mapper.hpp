#ifndef MAPPER_HPP
#define MAPPER_HPP
#include <iterator>

///MapMP_Mapper.
template<class KeyA, class A, class KeyB, class B, class C>
class MapMP_Mapper : public Mapper<KeyA,A,KeyB,B,C>
{ 
public:
	///Constructor.
	MapMP_Mapper(MapReduce<KeyA,A,KeyB,B,C> * _mapReduce):mapReduce(_mapReduce){};
	///Iterator mapping execution (implemented by library).
	virtual multimap<KeyB,B> run( multimap<KeyA,A> & as )
	{
		multimap<KeyB,B> bs;
		vector<pair<KeyA,A> > vcAs;
		typename multimap<KeyA,A>::iterator itAs;
		for (itAs = as.begin() ; itAs != as.end() ; ++itAs) vcAs.push_back( *itAs );
		#pragma omp parallel for
		for (int i = 0 ; i < vcAs.size() ; i++)// { sleep(1); cout << "#" << endl; };
		{
			multimap<KeyB,B> bss = map( vcAs[i] );
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

///MapMP_StrMapper.
class MapMP_StrMapper : public MapMP_Mapper<string,string,string,string,string> 
{
public:
	///Constructor.
	MapMP_StrMapper(MapReduce<string,string,string,string,string> * _mapReduce)
		:MapMP_Mapper<string,string,string,string,string>(_mapReduce) {};	
};

#endif /* MAPPER_HPP */
