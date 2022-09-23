#ifndef OPTFRAME_FLOYD_WARSHALL_HPP_
#define OPTFRAME_FLOYD_WARSHALL_HPP_

#include <limits>
#include "Graph.hpp"
#include "../Matrix.hpp"

using namespace std;

namespace optframe
{
struct FloydWarshall
{
	static Matrix<double>& run(const Graph<double>& g, Matrix<int>* pred = nullptr)
	{
		unsigned n = g.numNodes();
		Matrix<double>* mDist = new Matrix<double>(n);
		double INFINITE = numeric_limits<double>::max() / 2;
		mDist->fill(INFINITE);

		for(unsigned i = 0; i < n; i++)
			for(unsigned j = 0; j < n; j++)
			{
				double v = g.getValue(i, j);
				if(!g.isNull(v))
				{
					(*mDist)(i, j) = v;
					if(pred)
						(*pred)(i, j) = i;
				}
			}

		for(unsigned k = 0; k < n; k++)
			for(unsigned i = 0; i < n; i++)
				if((i != k) && ((*mDist)(i, k) < INFINITE))
					for(unsigned j = 0; j < n; j++)
					{
						double sum = (*mDist)(i, k) + (*mDist)(k, j);
						if((*mDist)(i, j) > sum)
						{
							(*mDist)(i, j) = sum;
							if(pred)
								(*pred)(i, j) = (*pred)(k, j);
						}
					}

		return *mDist;
	}

	/*
	 *
	 *
	 * 0 => 3: 0	{1}18	{3}3	SUM=4	OK!
	 vector(1) [18]

	 0 => 4: 0	(nullptr)3	{2}4	SUM=1000002	ERROR! EXPECTED 6
	 vector(1) [18]
	 */

	// return path in vector (i, ..., j]
	static vector<int> getPath(unsigned i, unsigned j, Matrix<int>& pred)
	{
		vector<int> path;
		int current = i;
		int next = pred(current, j);

		while(current != next)
		{
			if(pred(current, next) == current)
				path.push_back(next);
			else
			{
				vector<int> mpath = getPath(current, next, pred);
				path.insert(path.end(), mpath.begin(), mpath.end());
			}

			current = next;
			next = pred(current, j);
		}

		path.push_back(j);

		return path;
	}

};

}

#endif /*OPTFRAME_FLOYD_WARSHALL_HPP_*/
