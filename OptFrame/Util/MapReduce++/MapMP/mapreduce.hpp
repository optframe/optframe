#ifndef MAPREDUCE_HPP
#define MAPREDUCE_HPP

// CHANGELOG
// Removed output messages from method run(.) of MapMP_MapReduce in this version

///MapMP_MapReduce.
template<class KeyA, class A, class KeyB, class B, class C>
class MapMP_MapReduce : public MapReduce<KeyA,A,KeyB,B,C>
{
public:
	///MapReduce execution (implemented by library).
	virtual map<KeyB,C> run( Mapper<KeyA,A,KeyB,B,C> & mapper, Reducer<KeyA,A,KeyB,B,C> & reducer, multimap<KeyA,A> & as)
	{	
		//cout << "Input  :\t" << as << endl;
		multimap<KeyB,B> bs = mapper.run( as );
		//cout << "Mapped :\t" << bs << endl;
		map<KeyB,C> cs = reducer.run( bs );
		//cout << "Reduced:\t" << cs << endl;
		return cs;
	};
};

///MapMP_StrMapReduce.
class MapMP_StrMapReduce : public MapMP_MapReduce<string,string,string,string,string> {};

#endif /* MAPREDUCE_HPP */
