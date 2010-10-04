#ifndef TSP_PROBLEMINSTANCE_HPP_
#define TSP_PROBLEMINSTANCE_HPP_

#include <iostream>

#include "../../../OptFrame/Util/Scanner++/Scanner.h"

using namespace std;

#include <iostream>
#include <values.h>

#include "../../../OptFrame/Util/Scanner++/Scanner.h"
#include "../../../OptFrame/Util/Matrix.hpp"

using namespace std;

class TSPProblemInstance
{
public:
	TSPProblemInstance(Scanner& scanner)
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


#endif /*TSP_PROBLEMINSTANCE_HPP_*/
