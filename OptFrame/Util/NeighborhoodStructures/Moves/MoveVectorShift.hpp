#ifndef OPTFRAME_MOVEVECTORSHIFT_HPP_
#define OPTFRAME_MOVEVECTORSHIFT_HPP_

// Framework includes
#include "../../../Move.hpp"

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

#endif /*OPTFRAME_MOVEVECTORSHIFT_HPP_*/
