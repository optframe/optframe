// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef TSP_PROBLEMINSTANCE_HPP_
#define TSP_PROBLEMINSTANCE_HPP_

#include <cmath>
#include <iostream>

#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>

using namespace std;
using namespace scannerpp;
using namespace optframe;

namespace TSP {

class ProblemInstance
{
public:
   ProblemInstance(Scanner& scanner)
   {
      n = *scanner.nextInt();

      cout << n << endl;

      // Initializing values
      xvalues = new vector<double>(n);
      yvalues = new vector<double>(n);

      // Initializing distance matrix
      dist = new Matrix<double>(n, n);
      //dist->fill(INT_MAX);

      for (int i = 0; i < n; i++) {
         //cout << scanner.next(); << "\t"; // id
         scanner.next();
         (*xvalues)[i] = *scanner.nextDouble(); // x
         //cout << (*xvalues)[i] << "\t";
         (*yvalues)[i] = *scanner.nextDouble(); // y
                                                //cout << (*yvalues)[i] << endl;
      }

      cout << "All data read ok!" << endl;

      // Calculating the distances
      for (int i = 0; i < n; i++)
         for (int j = 0; j < n; j++)
            (*dist)(i, j) = distance(xvalues->at(i), yvalues->at(i), xvalues->at(j), yvalues->at(j));

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
      return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
   }
};

}

#endif /*TSP_PROBLEMINSTANCE_HPP_*/
