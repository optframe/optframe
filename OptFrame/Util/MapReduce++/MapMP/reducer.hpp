#ifndef REDUCER_HPP
#define REDUCER_HPP

///MapMP_Reducer.
template<class KeyA, class A, class KeyB, class B, class C>
class MapMP_Reducer : public Reducer<KeyA,A,KeyB,B,C>
{ 
public:
	///Constructor.
	MapMP_Reducer(MapReduce<KeyA,A,KeyB,B,C> * _mapReduce):mapReduce(_mapReduce){};
	///Iterator reducing execution (implemented by library).
	virtual map<KeyB,C> run( multimap<KeyB,B> & mapped)
	{
		map<KeyB,C> reduced;
		typename std::multimap<KeyB,B>::iterator it = mapped.begin();
		KeyB lastKey = (*it).first;
		vector< pair < KeyB,vector<B> > > toReduce(1, pair < KeyB,vector<B> >( lastKey , vector<B>() ) );
		for( ; it != mapped.end() ; ++it)
		{
			if ((*it).first != lastKey) {
				toReduce.push_back( pair< KeyB,vector<B> > ( (*it).first , vector<B>() ) ); 
				lastKey = (*it).first;
			}
			toReduce.back().second.push_back( (*it).second );
		}

		#pragma omp parallel for
		for ( int i = 0 ; i < toReduce.size() ; i++ ) reduced.insert( reduce( toReduce[i] ) ); // Reduce call

		return reduced;
	};
	///Reduce function (implemented by user).
	virtual pair<KeyB,C> reduce( pair<KeyB, vector<B> > ) = 0 ;
protected:
	MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
};

///MapMP_StrReducer.
class MapMP_StrReducer : public MapMP_Reducer<string,string,string,string,string> 
{
public:
	///Constructor.
	MapMP_StrReducer(MapReduce<string,string,string,string,string> * _mapReduce)
		:MapMP_Reducer<string,string,string,string,string>(_mapReduce){};	
};

#endif /* REDUCER_HPP */
