#ifndef REDUCER_HPP
#define REDUCER_HPP

///SeqMR_Reducer.
template<class KeyA, class A, class KeyB, class B, class C>
class SeqMR_Reducer : public Reducer<KeyA,A,KeyB,B,C>
{ 
public:
	///Constructor.
	SeqMR_Reducer(MapReduce<KeyA,A,KeyB,B,C> * _mapReduce):mapReduce(_mapReduce){};
	///Iterator reducing execution (implemented by library).
	virtual map<KeyB,C> run( multimap<KeyB,B> & mapped)
	{
		map<KeyB,C> reduced;
		// TODO
		typename std::multimap<KeyB,B>::iterator it = mapped.begin();
		KeyB lastKey = (*it).first;
		vector<B> toReduce;
		for( ; it != mapped.end() ; ++it)
		{
			if ((*it).first != lastKey) {
				pair<KeyB,C> r = reduce( pair<KeyB, vector<B> >( lastKey , toReduce ) ); toReduce.clear();
				reduced.insert( r );
				lastKey = (*it).first;
			}
			toReduce.push_back( (*it).second );
		}

		pair<KeyB,C> r = reduce( pair<KeyB, vector<B> >( lastKey , toReduce ) ); toReduce.clear();
		reduced.insert( r );

		return reduced;
	};
	///Reduce function (implemented by user).
	virtual pair<KeyB,C> reduce( pair<KeyB, vector<B> > ) = 0 ;
protected:
	MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
};

///SeqMR_StrReducer.
class SeqMR_StrReducer : public SeqMR_Reducer<string,string,string,string,string> 
{
public:
	///Constructor.
	SeqMR_StrReducer(MapReduce<string,string,string,string,string> * _mapReduce)
		:SeqMR_Reducer<string,string,string,string,string>(_mapReduce){};	
};

#endif /* REDUCER_HPP */
