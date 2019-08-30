#ifndef OPTFRAME_GRAPH_MATRIX_HPP_
#define OPTFRAME_GRAPH_MATRIX_HPP_

#include "Graph.hpp"
#include "../Matrix.hpp"

using namespace std;

namespace optframe
{

// GRAPH IMPLEMENTED BY MATRIX (OPTIMIZED FOR NUMERIC VALUES! INT, BOOL, DOUBLE...)
class GraphMatrix: public Graph<double>
{
protected:
	Matrix<double>* m;

public:
	GraphMatrix(unsigned _nNodes, bool _isDirected = true) :
			m(new Matrix<double>(nNodes)), Graph<double>(_nNodes, _isDirected)
	{
		setNullValue(0);
	}

	virtual ~GraphMatrix()
	{
		delete m;
	}

	// insert node and return id number (should increase node count)
	virtual unsigned insertNode()
	{
		nNodes++;
		Matrix<double>* m2 = new Matrix<double>(nNodes);
		m2->fill(nullVal);
		if(directed)
			for(int i = 0; i < ((int) nNodes) - 1; i++)
				for(int j = 0; j < ((int) nNodes) - 1; i++)
					(*m2)(i, j) = (*m)(i, j);
		else
			for(int i = 0; i < ((int) nNodes) - 1; i++)
				for(int j = i; j < ((int) nNodes) - 1; i++)
					(*m2)(i, j) = (*m)(i, j);
		delete m;
		m = m2;

		return nNodes - 1;
	}

	// remove node (should decrease node count)
	virtual void removeNode(unsigned n)
	{
		Matrix<double>* m2 = new Matrix<double>(nNodes - 1);
		m2->fill(nullVal);
		if(directed)
			for(int i = 0; i < ((int) nNodes) - 1; i++)
				for(int j = 0; j < ((int) nNodes) - 1; i++)
				{
					int i2 = (i >= n ? i - 1 : i);
					int j2 = (j >= n ? j - 1 : j);
					(*m2)(i2, j2) = (*m)(i, j);
				}
		else
			for(int i = 0; i < ((int) nNodes) - 1; i++)
				for(int j = i; j < ((int) nNodes) - 1; i++)
				{
					int i2 = (i >= n ? i - 1 : i);
					int j2 = (j >= n ? j - 1 : j);
					(*m2)(i2, j2) = (*m)(i, j);
				}
		delete m;
		m = m2;

		nNodes--;
	}

	// insert arc (should increase arc count)
	virtual void insertArc(unsigned n1, unsigned n2, const double& value)
	{
		if(directed)
		{
			(*m)(n1, n2) = value;
			nArcs++;
		}
	}

	// remove arc (should decrease arc count)
	virtual void removeArc(unsigned n1, unsigned n2)
	{
		if(directed)
		{
			(*m)(n1, n2) = nullVal;
			nArcs--;
		}
	}

	// insert edge (should increase edge count)
	virtual void insertEdge(unsigned n1, unsigned n2, const double& value)
	{
		if(n1 > n2)
		{
			unsigned z = n1;
			n1 = n2;
			n2 = z;
		}

		if(!directed)
		{
			(*m)(n1, n2) = value;
			nEdges++;
		}

	}

	// remove edge (should decrease edge count)
	virtual void removeEdge(unsigned n1, unsigned n2)
	{
		if(n1 > n2)
		{
			unsigned z = n1;
			n1 = n2;
			n2 = z;
		}

		if(!directed)
		{
			(*m)(n1, n2) = nullVal;
			nEdges--;
		}
	}

	// get arc/edge value
	virtual double getValue(unsigned n1, unsigned n2) const
	{
		if(!directed && (n1 > n2))
		{
			unsigned z = n1;
			n1 = n2;
			n2 = z;
		}

		return (*m)(n1, n2);
	}

	// set arc/edge value
	virtual inline void setValue(unsigned n1, unsigned n2, const double& v)
	{
		if(!directed && (n1 > n2))
		{
			unsigned z = n1;
			n1 = n2;
			n2 = z;
		}

		(*m)(n1, n2) = v;
	}

	virtual void setNullValue(const double& v)
	{
		m->fill(v);
		nullVal = v;
	}

	// class id (for printing)
	string id() const
	{
		return "GraphMatrix";
	}

	virtual void print() const
	{
		if(directed)
		{
			cout << id() << ": |V|=" << nNodes << " |A|=" << nArcs << ":" << endl;
			cout << "*\t";
			for(unsigned i = 0; i < nNodes; i++)
				cout << "(" << i << ")\t";
			cout << endl;
			for(unsigned i = 0; i < nNodes; i++)
			{
				cout << "(" << i << ")\t";
				for(unsigned j = 0; j < nNodes; j++)
					cout << (*m)(i, j) << "\t";
				cout << endl;
			}
		}
		else
		{
			cout << id() << ": |V|=" << nNodes << " |E|=" << nEdges << " (symmetric matrix; '.'=null; '-'=unused)" << endl;
			cout << "*\t";
			for(unsigned i = 0; i < nNodes; i++)
				cout << "(" << i << ")\t";
			cout << endl;
			for(unsigned i = 0; i < nNodes; i++)
			{
				cout << "(" << i << ")\t";
				for(unsigned j = 0; j < nNodes; j++)
					if(j < i)
						cout << "-\t";
					else
					{
						if(!isNull((*m)(i, j)))
							cout << (*m)(i, j) << "\t";
						else
							cout << ".\t";
					}
				cout << endl;
			}
		}

	}

};

}

#endif /*OPTFRAME_GRAPH_MATRIX_HPP_*/
