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

