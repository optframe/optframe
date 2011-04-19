#ifndef OPTFRAME_NSENUMVECTORSHIFT_HPP_
#define OPTFRAME_NSENUMVECTORSHIFT_HPP_

// Framework includes

#include "../../NSEnum.hpp"

#include "./Moves/MoveVectorShift.hpp"

using namespace std;

//============================================================================
//                  Shift Neighborhood Structure
//============================================================================


template<class T, class M>
class NSEnumVectorShift: public NSEnum< vector<T>, M >
{
private:
	int n;

public:	

	NSEnumVectorShift(int n)
	{
		this->n = n;
	}


	virtual Move<vector<T>,M>* kmove(int k)
	{
		if(k>size())
		{
			cerr << "Neighborhood Shift Error! Move " << k << " doesnt exist! Valid Interval from 0 to " << (size()-1) << "." << endl;
			exit(1);

			return NULL;
		}

		return new MoveVectorShift<T,M>((k/n), (k%n) );
	}


	int size()
	{
		return n*n;
	}

	virtual void print()
	{
		cout << "NSEnum Vector Shift ("<< size() << ")" << endl;
	}

};

#endif /*OPTFRAME_NSENUMVECTORSHIFT_HPP_*/
