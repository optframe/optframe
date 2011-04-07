#ifndef OPTFRAME_NSENUMVECTORSHIFT_HPP_
#define OPTFRAME_NSENUMVECTORSHIFT_HPP_

// Framework includes
#include "../Move.hpp"
#include "../NSEnum.hpp"

using namespace std;

//============================================================================
//                           Shift Move
//============================================================================

template<class T, class M>
class MoveVectorShift : public Move<vector<T>, M>
{
private:
	int t1, t2;
	int n;

public:

	MoveVectorShift(int t1, int t2)
	{
		this->t1 = t1;
		this->t2 = t2;
	}

	bool canBeApplied(Solution<vector<T> >* s)
	{
		return t1 != t2;
	}

	Move<vector<T>, M>* apply(Solution<vector<T> >* s)
	{
		vector<T>* rep = s->getRepresentation();

		T aux;
		if(t1 < t2)
			for(int i = t1; i < t2; i++)
			{
				aux = rep->at(i);
				(*rep)[i] = (*rep)[i+1];
				(*rep)[i+1] = aux;
			}
		else
			for(int i = t1; i > t2; i--)
			{
				aux = rep->at(i);
				(*rep)[i] = (*rep)[i-1];
				(*rep)[i-1] = aux;
			}

		MoveVectorShift<T,M> * ms = new MoveVectorShift<T,M>(t2,t1);

		return ms;
	}

	void print() const
	{
		cout << "Move Vector Shift("<< t1 << " -> " << t2 <<")"<<endl;
	}
};


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
