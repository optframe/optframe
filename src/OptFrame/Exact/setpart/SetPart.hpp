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

/*
Class SetPart
Description: exact set partition model
Author: Sabir Ribas
Email: sabirribas@gmail.com
Date: Jun 22, 2010
Version: 0.6
*/

#ifndef SETPART_HPP
#define SETPART_HPP

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include "../../Util/Scanner++/Scanner.h"
#include "../../Util/printable.h"

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

using namespace std;

/***
Class Column
*/

class Column
{
public:
   double cost;
   vector<bool> values;
   Column(double _cost, vector<bool> _values)
     : cost(_cost)
     , values(_values){};
};

typedef vector<Column> Columns;
typedef IloArray<IloNumArray> NumMatrix;
typedef IloArray<IloNumVarArray> NumVarMatrix;

/***
Class SetPart
*/

class SetPart
{
protected:
   int lines, maxColumns;
   Columns columns;

public:
   SetPart(int _lines, int _maxColumns)
     : lines(_lines)
     , maxColumns(_maxColumns){};

   // SetPart::setColumn
   //   Parameters: cost and column
   //   Return: int column index (if >= 0) or error code (if < 0)
   int addColumn(double cost, vector<bool> values)
   {
      if (values.size() != lines) {
         //cerr << "[SetPart::addColumn] Warning: the column size must be equal the number of lines" << endl;
         return -1;
      }

      bool hasATrue = false;
      for (unsigned i = 0; i < values.size() && !hasATrue; i++)
         if (values[i])
            hasATrue = true;

      if (!hasATrue) {
         //cerr << "[SetPart::addColumn] Warning: there are no elements at this column" << endl;
         return -2;
      }

      if (exists(cost, values)) {
         //cerr << "[SetPart::addColumn] Warning: this column already exists in the set" << endl;
         return -3;
      }

      if (cost < 0) {
         //cerr << "[SetPart::addColumn] Warning: the cost must be non-negative" << endl;
         return -4;
      }

      columns.push_back(Column(cost, values));

      return columns.size() - 1;
   };

   bool exists(double cost, vector<bool> values)
   {
      for (unsigned i = 0; i < columns.size(); i++)
         if (cost == columns[i].cost && values == columns[i].values)
            return true;
      return false;
   }

   pair<double, vector<int>> solve()
   {
      cout << "[SetPart::solve]: Calling optimizer" << endl;

      double finalCost = 0;
      vector<int> cs;

      /* Solving by Concert */

      //model
      /*
			minimize Z:
			   sum {j in C} Custos[j] * Y[j];

			subject to
			   R2 {i in L}:
		    	  sum {j in C} Y[j] * Matriz[i,j] = 1; 
			   R2 sum {j in C} Y[j] <= k; 
		 */

      IloEnv env;
      try {
         IloModel model(env);

         IloInt i, j;
         IloInt m(lines);
         IloInt n(columns.size());

         IloIntVarArray y(env, n);

         for (j = 0; j < n; j++) {
            stringstream str;
            str << "Y[" << j << "]";
            y[j] = IloIntVar(env, 0, 1, str.str().c_str());
         }

         IloNumArray costs(env, n);
         IloIntArray2 matrix(env, m);

         for (i = 0; i < lines; i++) {
            //cout << endl;
            matrix[i] = IloIntArray(env, columns.size());
            for (j = 0; j < columns.size(); j++) {
               matrix[i][j] = IloInt(columns[j].values[i]);
               //cout << i << " " << j << " " << matrix[i][j] << " | ";
            }
         }

         IloExpr obj(env);
         for (j = 0; j < n; j++) {
            costs[j] = IloNum(columns[j].cost);
            obj += costs[j] * y[j];
         }

         model.add(IloMinimize(env, obj));

         for (i = 0; i < m; i++) {
            IloExpr r2(env);
            for (j = 0; j < n; j++)
               r2 += y[j] * matrix[i][j];
            //model.add(v == IloInt(1));
            model.add(IloConstraint(r2 == 1));
            r2.end();
         }

         IloExpr r3(env);
         for (j = 0; j < n; j++)
            r3 += y[j];
         model.add(IloConstraint(r3 <= maxColumns));
         r3.end();

         //cplex
         IloCplex cplex(model);
         cplex.exportModel("setpart.lp");
         cplex.solve();

         for (j = 0; j < columns.size(); j++)
            if (cplex.getValue(y[j]) > .5) {
               cs.push_back(j);
               finalCost += costs[j];
            }

         for (i = 0; i < lines; i++) {
            cout << endl;
            for (j = 0; j < columns.size(); j++) {
               if (cplex.getValue(y[j]) > .5)
                  cout << matrix[i][j] << " | ";
            }
         }

      } catch (...) {
      };

      return make_pair(finalCost, cs);
   }

   int numberOfColumns() { return columns.size(); }
};

#endif /*SETPART_HPP*/
