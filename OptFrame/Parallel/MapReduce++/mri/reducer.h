#ifndef REDUCER_H
#define REDUCER_H

///Reducer.
template<class KeyA, class A, class KeyB, class B, class C>
class Reducer 
{ 
public:  
	///Iterator reducing execution (implemented by library).
#ifndef MRI_USE_MULTIMAP
	virtual vector< pair<KeyB,C> > run( vector< pair<KeyB,B> > & ) = 0 ;
#else
	virtual vector< pair<KeyB,C> > run( multimap<KeyB,B> & ) = 0 ;
#endif
	///Reduce function (implemented by user).
	virtual pair<KeyB,C> reduce( pair<KeyB, vector<B> > ) = 0 ;
private:
	MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
};

///StrReducer.
class StrReducer : public Reducer<string,string,string,string,string> {};

#endif /* REDUCER_H */
