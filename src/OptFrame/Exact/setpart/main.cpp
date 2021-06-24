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
SetPart example
Author: Sabir Ribas
Email: sabirribas@gmail.com
Date: Jun 22, 2010
*/

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

#include "../../Util/Scanner++/Scanner.h"
#include "../../Util/printable.h"

#include "SetPart.hpp"

using namespace std;

int
main()
{
   cout << "Solving Set Partition" << endl;

   vector<bool> temp(3); // column with 3 lines

   SetPart setPart(3, 2); // 3 lines and the solution must have 2 or less columns

   temp[0] = true;
   temp[1] = false;
   temp[2] = false;
   // SetPart::setColumn
   //   Parameters: cost and column
   //   Return: int column index (if >= 0) or error code (if < 0)
   cout << "Trying to add a column: " << setPart.addColumn(10.0, temp) << endl;

   temp[0] = true;
   temp[1] = true;
   temp[2] = false;
   cout << "Trying to add a column: " << setPart.addColumn(20, temp) << endl;

   temp[0] = false;
   temp[1] = false;
   temp[2] = true;
   cout << "Trying to add a column: " << setPart.addColumn(10, temp) << endl;

   temp[0] = true;
   temp[1] = false;
   temp[2] = true;
   cout << "Trying to add a column: " << setPart.addColumn(20, temp) << endl;

   temp[0] = false;
   temp[1] = true;
   temp[2] = false;
   cout << "Trying to add a column: " << setPart.addColumn(5, temp) << endl;

   temp[0] = false;
   temp[1] = false;
   temp[2] = true;
   cout << "Trying to add a column: " << setPart.addColumn(10, temp) << endl;

   temp[0] = false;
   temp[1] = false;
   temp[2] = false;
   cout << "Trying to add a column: " << setPart.addColumn(10, temp) << endl;

   temp[0] = false;
   temp[1] = false;
   temp[2] = false;
   temp.push_back(true);
   cout << "Trying to add a column: " << setPart.addColumn(10, temp) << endl;

   pair<double, vector<int>> usedColumns = setPart.solve();

   cout << "Used columns: " << usedColumns << endl;

   cout << "Finish" << endl;

   return 0;
}
