#ifndef OPTFRAME_GRAPH_MAP_HPP_
#define OPTFRAME_GRAPH_MAP_HPP_

#include <map> 
#include <vector>
#include "Graph.hpp"

using namespace std;

namespace optframe
{

// GRAPH IMPLEMENTED BY MAP (OPTIMIZED FOR NUMERIC VALUES! INT, BOOL, DOUBLE...)
class GraphMap: public Graph<double>
{
protected:
	map<pair<unsigned, unsigned>, double> mapAll;

public:
	GraphMap(unsigned _nNodes, bool _isDirected = true) :
			Graph<double>(_nNodes, _isDirected)
	{
		setNullValue(0);
	}

	virtual ~GraphMap()
	{
	}

	// insert node and return id number (should increase node count)
	virtual unsigned insertNode()
	{
		nNodes++;
		return nNodes - 1;
	}

	// remove node (should decrease node count)
	virtual void removeNode(unsigned n)
	{
		map<pair<unsigned, unsigned>, double>::iterator pm_it = mapAll.begin();

		// store changed elements (cannot change constant key?)
		vector<pair<pair<unsigned, unsigned>, double> > vr;

		while(pm_it != mapAll.end())
		{
			double e1 = pm_it->first.first;
			double e2 = pm_it->first.second;
			if((e1 == n) || (e2 == n))
			{
				mapAll.erase(pm_it++); // USE POST OPERATOR!

				if(directed)
					nArcs--;
				else
					nEdges--;
			}
			else if((e1 > n) || (e1 > n))
			{
				if(e1 > n)
					e1--;
				if(e2 > n)
					e2--;

				double r = pm_it->second;
				mapAll.erase(pm_it++); // USE POST OPERATOR!

				vr.push_back(make_pair(make_pair(e1, e2), r));
			}
			else
				++pm_it; // USE BEFORE FOR BETTER OPTIMIZATION
		}

		// add relocated elements
		for(unsigned i = 0; i < vr.size(); i++)
			mapAll[vr[i].first] = vr[i].second;

		nNodes--;
	}

	// insert arc (should increase arc count)
	virtual void insertArc(unsigned n1, unsigned n2, const double& value)
	{
		if(directed)
		{
			mapAll[make_pair(n1, n2)] = value;
			nArcs++;
		}
	}

	// remove arc (should decrease arc count)
	virtual void removeArc(unsigned n1, unsigned n2)
	{
		if(directed)
		{
			mapAll.erase(make_pair(n1, n2));
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
			mapAll[make_pair(n1, n2)] = value;
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
			mapAll.erase(make_pair(n1, n2));
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

		map<pair<unsigned, unsigned>, double>::const_iterator it;
		pair<unsigned, unsigned> p(n1, n2);
		it = mapAll.find(p);
		if(it != mapAll.end())
			return it->second;
		else
			return nullVal;
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

		mapAll[make_pair(n1, n2)] = v;
	}

	virtual void setNullValue(const double& v)
	{
		mapAll.clear();
		nullVal = v;
	}

	// class id (for printing)
	string id() const
	{
		return "GraphMap";
	}

};

}

#endif /*OPTFRAME_GRAPH_MAP_HPP_*/
