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

