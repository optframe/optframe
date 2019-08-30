// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

int main()
{
    cout << "Solving Set Partition" << endl;
	
	vector<bool> temp(3); // column with 3 lines
	
	SetPart setPart(3,2); // 3 lines and the solution must have 2 or less columns
	
	temp[0]=true;
	temp[1]=false;
	temp[2]=false; 
	// SetPart::setColumn
	//   Parameters: cost and column
	//   Return: int column index (if >= 0) or error code (if < 0)
	cout << "Trying to add a column: " << setPart.addColumn( 10.0,temp ) << endl;
	
	temp[0]=true;
	temp[1]=true;
	temp[2]=false; 
	cout << "Trying to add a column: " << setPart.addColumn( 20,temp ) << endl;
	
	temp[0]=false;
	temp[1]=false;
	temp[2]=true; 
	cout << "Trying to add a column: " << setPart.addColumn( 10,temp ) << endl;
	
	temp[0]=true;
	temp[1]=false;
	temp[2]=true; 
	cout << "Trying to add a column: " << setPart.addColumn( 20,temp ) << endl;
	
	temp[0]=false;
	temp[1]=true;
	temp[2]=false; 
	cout << "Trying to add a column: " << setPart.addColumn( 5,temp ) << endl;
	

	temp[0]=false;
	temp[1]=false;
	temp[2]=true; 
	cout << "Trying to add a column: " << setPart.addColumn( 10,temp ) << endl;
	
	temp[0]=false;
	temp[1]=false;
	temp[2]=false; 
	cout << "Trying to add a column: " << setPart.addColumn( 10,temp ) << endl;
	
	temp[0]=false;
	temp[1]=false;
	temp[2]=false;
	temp.push_back(true);
	cout << "Trying to add a column: " << setPart.addColumn( 10,temp ) << endl;
	
	pair< double , vector<int> > usedColumns = setPart.solve();

	cout << "Used columns: " << usedColumns << endl;
	
	cout << "Finish" << endl;

	return 0;
}

