/**
Exemplo de uso do MaPI
ex01.cpp
Objetivo: Calcular o somatório dos elementos do quadrado de uma matriz de órdem n (Neste caso n=100).
Exemplo desenvolvido por Sabir Ribas (sabirribas@gmail.com) e Mário Perché (mariohpp@gmail.com), em 31/10/2009.
*/

#include "../MaPI/MaPI.h"

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <iostream>

/* Classe compartilhada */

typedef struct Shared {
    int n;                // Ordem da matriz (n<=100)
    int matrix[100][100]; // Matriz
} Shared;

/* Função de mapeamento */

string fmap( string i_j , Shared _shared)
{
    int result = 0;
    
    int posSeparator = i_j.find("_");
    
    string strLine = i_j.substr (0, (posSeparator-1) ),
           strColumn = i_j.substr ( (posSeparator+1) );

    int line = atoi(strLine.c_str());
    int column = atoi(strColumn.c_str());

    for(int i=0;i<(_shared.n);i++)
    { result += _shared.matrix [line] [i] * _shared.matrix [i] [column]; }

    stringstream r; 
    r << result;

    cout << i_j << " = " << r.str() << "\n";

    return r.str();
};

/* Função de redução */

vector<string> * freduce (vector<string> * mapped, Shared _shared)
{
    vector<string> * reduced = new vector<string>;

    int result = 0;

    for(int i = 0 ; i < mapped->size() ; i++) result += atoi(mapped->at(i).c_str());
    
    stringstream r;
    r << result;
    reduced->push_back(r.str());

    return reduced;
};

/* Função main */

int main(int argc, char** argv)
{
    // Tempo
    clock_t clockini; clockini = clock();

    // Objeto compartilhado
    Shared shared;
    shared.n = 100; // ordem da matriz
    for(int i = 0 ; i < shared.n ; i++) for(int j = 0 ; j < shared.n ; j++) shared.matrix[i][j] = 1; // inicialização da matriz

    // Inicialização do MaPI
    MaPI_FSMapReduce<Shared> mr;
    mr.init(argc,argv,fmap,shared);

    // Dados de entrada
    vector<string> input;
    for (int i = 0 ; i < shared.n ; i++) {
        for (int j = 0 ; j < shared.n ; j++) {
            stringstream i_j;
            i_j << i << "_" << j;
            input.push_back(i_j.str());
        }
    }

    // Chamada do MapReduce
    vector<string> * output = mr.mapreduce(fmap,freduce,&input);

    // Resultado do MapReduce
    for(int i = 0 ; i < output->size() ; i++) printf("Result: %s\n", output->at(i).c_str());

    // Finalização do MaPI
    mr.finalize();

    // Tempo
    printf( "Total time: %.10f segs\n" , (float) ((clock()-clockini*1.0)/CLOCKS_PER_SEC) );    
};

