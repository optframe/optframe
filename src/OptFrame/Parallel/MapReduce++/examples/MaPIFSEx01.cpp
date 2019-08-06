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

#include "../MaPI/MaPI.h"

string fmap(string input, string shared)
{
	string mapped = input + shared;	printf("%s\n", mapped.c_str()); 
	return mapped; 
}

vector<string> * freduce (vector<string> * mapped, string shared) 
{
    vector<string> * reduced = new vector<string>;
    reduced->push_back(mapped->at(0) + " e reduzida");
    return reduced;
}

int main(int argc, char** argv)
{
    MaPI_FSMapReduce<string> mr; 
	mr.init(argc,argv,fmap,string(" mapeada"));

    vector<string> inputs;
	inputs.push_back("Entrada 1");
	inputs.push_back("Entrada 2");

    vector<string> * output = mr.mapreduce(fmap,freduce,&inputs);
    printf("%s\n", output->at(0).c_str());

    mr.finalize();
}

