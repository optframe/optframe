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

