// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef TSP_PROBLEMINSTANCE_HPP_
#define TSP_PROBLEMINSTANCE_HPP_

#include <iostream>
#include <cmath>

#include "../../OptFrame/Scanner++/Scanner.h"
#include "../../OptFrame/Util/Matrix.hpp"

using namespace std;
using namespace scannerpp;

namespace TSP
{

class ProblemInstance
{
public:
	ProblemInstance(Scanner& scanner)
	{
		n = scanner.nextInt();

		cout << n << endl;

		// Initializing values
		xvalues = new vector<double>(n);
		yvalues = new vector<double>(n);


		// Initializing distance matrix
		dist = new Matrix<double>( n, n );
		//dist->fill(INT_MAX);

		for(int i=0;i<n;i++)
		{
			//cout << scanner.next(); << "\t"; // id
			scanner.next();
			(*xvalues)[i] = scanner.nextDouble(); // x
			//cout << (*xvalues)[i] << "\t";
			(*yvalues)[i] = scanner.nextDouble(); // y
			//cout << (*yvalues)[i] << endl;
		}

		cout << "All data read ok!" << endl;


		// Calculating the distances
		for(int i=0;i<n;i++)
			for(int j=0;j<n;j++)
				(*dist)(i,j)=distance(xvalues->at(i),yvalues->at(i),xvalues->at(j),yvalues->at(j));

		//cout << (*dist);
	}

	virtual ~ProblemInstance()
	{
		delete dist;
		delete xvalues;
		delete yvalues;
	}

//private:
	// Your private vars
	int n;

	Matrix<double>* dist;

	vector<double>* xvalues;
	vector<double>* yvalues;

	double distance(double x1, double y1, double x2, double y2)
	{
		return sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
	}

};

}

#endif /*TSP_PROBLEMINSTANCE_HPP_*/
