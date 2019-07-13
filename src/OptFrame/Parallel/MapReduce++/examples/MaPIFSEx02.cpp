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
#include <sstream>

string fmap(string st, string obj) { printf("%s\n", (st + obj).c_str()); return st + obj; };

vector<string> * freduce (vector<string> * mapped, string obj) {
    vector<string> * reduced = new vector<string>;
    reduced->push_back(mapped->at(0) + "(reduzida) ");
    return reduced;
};

int main(int argc, char** argv)
{
    string text(" mapeada");
    MaPI_FSMapReduce<string> mr; mr.init(argc,argv,fmap,text);

    vector<string> input;
    for (int i = 0; i < 4; i++) { stringstream s; s << "Entrada " << i; input.push_back(s.str()); }

    for (int i = 0; i < input.size(); i++) { printf("%s\n", input.at(i).c_str()); }

    vector<string> * output = mr.mapreduce(fmap,freduce,&input);

    printf("%s\n", output->at(0).c_str());

    mr.finalize();
}

