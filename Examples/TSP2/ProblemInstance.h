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

#ifndef TSP2_PROBLEMINSTANCE_HPP_
#define TSP2_PROBLEMINSTANCE_HPP_

#include <iostream>
#include <cmath>
#include <stdio.h>

#include "../../OptFrame/Scanner++/Scanner.h"
#include "../../OptFrame/Util/Matrix.hpp"

using namespace std;
using namespace scannerpp;

namespace TSP2
{

class ProblemInstance
{
public:
	int n;

	Matrix<double> dist;

	vector<double> xvalues;
	vector<double> yvalues;

	// using euclidean distance
	double distance(double x1, double y1, double x2, double y2)
	{
		return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	}

	ProblemInstance(Scanner& scanner) :
			dist(Matrix<double>(1))
	{
		scanner.nextLine();
		scanner.nextLine();
		scanner.nextLine();
		scanner.next();
		n = scanner.nextInt();
		scanner.nextLine();
		scanner.nextLine();

		cout << n << endl;

		// Initializing values
		xvalues = vector<double>(n);
		yvalues = vector<double>(n);

		// Initializing distance matrix
		dist = Matrix<double>(n, n);
		//dist->fill(INT_MAX);

		for (int i = 0; i < n; i++)
		{
			//cout << scanner.next(); << "\t"; // id
			scanner.next();
			xvalues[i] = scanner.nextDouble(); // x
			//cout << (*xvalues)[i] << "\t";
			yvalues[i] = scanner.nextDouble(); // y
			//cout << (*yvalues)[i] << endl;
		}

		// Calculating the distances
		for (int i = 0; i < n; i++)
			for (int j = 0; j < n; j++)
				dist(i, j) = distance(xvalues.at(i), yvalues.at(i), xvalues.at(j), yvalues.at(j));

		cout << "All data read ok!" << endl;
	}

	virtual ~ProblemInstance()
	{
	}

};

}

#endif /*TSP2_PROBLEMINSTANCE_HPP_*/
