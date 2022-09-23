#ifndef OPTFRAME_GRAPH_HPP_
#define OPTFRAME_GRAPH_HPP_

#include <vector>

using namespace std;

namespace optframe
{
// Graph class supporting arcs or edges (may be mixed some day)
template<class X>
class Graph
{
protected:
	unsigned nNodes;
	unsigned nEdges;
	unsigned nArcs;
	bool directed;
	X nullVal;

public:
	Graph(unsigned _nNodes, bool _isDirected = true) :
			nNodes(_nNodes), nEdges(0), nArcs(0), directed(_isDirected)
	{
	}

	virtual ~Graph()
	{
	}

	inline unsigned numNodes() const
	{
		return nNodes;
	}

	inline unsigned numEdges() const
	{
		return nEdges;
	}

	inline unsigned numArcs() const
	{
		return nArcs;
	}

	inline bool isDirected() const
	{
		return isDirected;
	}

	// get null value (standard is zero)
	virtual void setNullValue(const X& v)
	{
		nullVal = v;
	}

	// get null value (standard is zero)
	inline X getNullValue() const
	{
		return nullVal;
	}

	inline bool isNull(const X& x) const
	{
		return x == nullVal;
	}

	// insert node and return id number (should increase node count)
	virtual unsigned insertNode() = 0;

	// remove node (should decrease node count)
	virtual void removeNode(unsigned n) = 0;

	// insert arc (should increase arc count)
	virtual void insertArc(unsigned n1, unsigned n2, const X& value) = 0;

	// remove arc (should decrease arc count)
	virtual void removeArc(unsigned n1, unsigned n2) = 0;

	// insert edge (should increase edge count)
	virtual void insertEdge(unsigned n1, unsigned n2, const X& value) = 0;

	// remove edge (should decrease edge count)
	virtual void removeEdge(unsigned n1, unsigned n2) = 0;

	// get arc/edge value
	virtual X getValue(unsigned n1, unsigned n2) const = 0;

	// set arc/edge value
	virtual inline void setValue(unsigned n1, unsigned n2, const X& v)
	{
		if(directed)
			insertArc(n1, n2, v);
		else
			insertEdge(n1, n2, v);
	}

	// return true if arc exists
	inline bool hasArc(unsigned n1, unsigned n2) const
	{
		if(!directed)
			return false;
		return getValue(n1, n2) != nullVal;
	}

	// return true if arc exists
	inline bool hasEdge(unsigned n1, unsigned n2) const
	{
		if(directed)
			return false;
		return getValue(n1, n2) != nullVal;
	}

	// return true if arc/edge exists
	inline bool exists(unsigned n1, unsigned n2) const
	{
		return getValue(n1, n2) != nullVal;
	}

	// get all arcs/edges
	virtual vector<pair<pair<unsigned, unsigned>, X> > getAll() const
	{
		vector<pair<pair<unsigned, unsigned>, X> > r;
		if(directed)
			for(unsigned i = 0; i < numNodes(); i++)
				for(unsigned j = 0; j < numNodes(); j++)
				{
					X v = getValue(i, j);
					if(getValue(i, j) != nullVal)
						r.push_back(make_pair(make_pair(i, j), v));
				}
		else
			for(int i = 0; i < ((int) numNodes()) - 1; i++)
				for(int j = i + 1; j < ((int) numNodes()); j++)
				{
					X v = getValue(i, j);
					if(getValue(i, j) != nullVal)
						r.push_back(make_pair(make_pair(i, j), v));
				}

		return r;
	}

private:
	// comparison for sorting in ascending order
	static bool compAsc(const pair<pair<unsigned, unsigned>, X>& v1, const pair<pair<unsigned, unsigned>, X>& v2)
	{
		return v1.second < v2.second;
	}

	// comparison for sorting in descending order
	static bool compDesc(const pair<pair<unsigned, unsigned>, X>& v1, const pair<pair<unsigned, unsigned>, X>& v2)
	{
		return v1.second > v2.second;
	}

public:
	// get all arcs/edges sorted
	virtual vector<pair<pair<unsigned, unsigned>, X> > getAllSorted(bool ascending = true) const
	{
		vector<pair<pair<unsigned, unsigned>, X> > r = getAll();

		if(ascending)
			std::sort(r.begin(), r.end(), compAsc);
		else
			std::sort(r.begin(), r.end(), compDesc);

		return r;
	}

	// print graph
	virtual void print() const
	{
		if(directed)
			cout << id() << ": |V|=" << nNodes << " |A|=" << nArcs << " => " << getAll() << endl;
		else
			cout << id() << ": |V|=" << nNodes << " |E|=" << nEdges << " => " << getAll() << endl;
	}

	// class id (for printing)
	string id() const
	{
		return "Graph";
	}

};

}

#endif /*OPTFRAME_GRAPH_HPP_*/
