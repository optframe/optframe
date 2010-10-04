#ifndef MAPREDUCE_HPP
#define MAPREDUCE_HPP

///SeqMR_MapReduce.
template<class KeyA, class A, class KeyB, class B, class C>
class SeqMR_MapReduce : public MapReduce<KeyA,A,KeyB,B,C>
{
public:
	///MapReduce execution (implemented by library).
	virtual vector< pair<KeyB,C> > run( Mapper<KeyA,A,KeyB,B,C> & mapper, Reducer<KeyA,A,KeyB,B,C> & reducer, vector< pair<KeyA,A> > & as)
	{	
		//cout << "Input  :\t" << as << endl;
#ifndef MRI_USE_MULTIMAP
		vector< pair<KeyB,B> > bs = mapper.run( as );
#else
		multimap<KeyB,B> bs = mapper.run( as );
#endif
		//cout << "Mapped :\t" << bs << endl;
		vector< pair<KeyB,C> > cs = reducer.run( bs );
		//cout << "Reduced:\t" << cs << endl;
		return cs;
	};
};

///SeqMR_StrMapReduce.
class SeqMR_StrMapReduce : public SeqMR_MapReduce<string,string,string,string,string> {};

#endif /* MAPREDUCE_HPP */
