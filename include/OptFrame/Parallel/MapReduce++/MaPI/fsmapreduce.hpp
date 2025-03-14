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

/* =============================================================== *
   MaPI_FSMapReduce (old MaPI.hpp from MaPI v1.2)
 * =============================================================== */

/* \mainpage MaPI v1.1
 *
 * MaPI é uma implementação em C++ da abstração MapReduce.
 * 
 * O objetivo do MaPI é fornecer ao usuário todo o aparato necessário para a implementação rápida de procedimentos paralelos. Ao usar o MaPI um usuário inexperiente quanto a programação distribuída é capaz de implementar uma aplicação paralela sem se preocupar com a forma de comunicação entre os processos ou como o sistema fará a paralelização. Toda a implementação feita pelo usuário é sequencial.
 *
 * \authors Sabir Ribas  (sabirribas@gmail.com)
 * \authors Mário Perché (mariohpp@gmail.com)
 * \authors Igor Machado (igor.machado@yahoo.com.br)
 * \authors Pablo Munhoz (pablo.munhoz@gmail.com)
 */

#ifndef FSMAPREDUCE_H
#define FSMAPREDUCE_H

#include <iostream>
#include <map>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

///MaPI_FSMapReduce.
/**
*/

template<class T>
class MaPI_FSMapReduce
{
public:
   ///Inicialização o objeto MaPI.

   // Initializes a MaPI object with four parameters
   // Parameters:	1 - argc
   //				2 - argv
   //				3 - function
   //				4 - object
   // RETURN: void

   void init(int argc, char** argv, string (*)(string, T), T object);

   ///Inicialização o objeto MaPI.

   // Initializes a MaPI object with four parameters
   // Parameters:	1 - argc
   //				2 - argv
   //				3 - vector of functions
   //				4 - object
   // RETURN: void

   void init(int argc, char** argv, vector<string (*)(string, T)> functions, T object);

   ///Função de mapeamento.

   // Initializes a MaPI object with four parameters
   // Parameters:	1 - Map function
   //				2 - Input vector of strings
   // RETURN: This function returns a vector of MAPPED strings

   vector<string>* mapper(string (*map)(string, T), vector<string>* inputs); // Map

   ///Chamada do MapReduce.

   vector<string>* mapreduce(string (*map)(string, T), vector<string>* (*reduce)(std::vector<string>*, T), vector<string>* input); // MapReduce

   ///Finalização do MaPI.

   void finalize();

   ///Número de processos de mapeamento (np-1).

   int mapperSize()
   {
      return (MRSize - 1);
   }

private:
   T shared;
   vector<string (*)(string, T)> functionsT;

   void applyFunction();

   int MRSize, MRRank;
   MPI_Status MRStat;
};

// ====================================
// MaPI.cpp
// ====================================

template<class T>
void
MaPI_FSMapReduce<T>::init(int argc, char** argv, string (*function)(string, T), T object)
{
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &MRSize);
   MPI_Comm_rank(MPI_COMM_WORLD, &MRRank);
   functionsT.push_back(function);
   shared = object;
   if (MRRank != 0)
      applyFunction();
}

template<class T>
void
MaPI_FSMapReduce<T>::init(int argc, char** argv, vector<string (*)(string, T)> functions, T object)
{
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &MRSize);
   MPI_Comm_rank(MPI_COMM_WORLD, &MRRank);
   functionsT = functions;
   shared = object;
   if (MRRank != 0)
      applyFunction();
}

template<class T>
void
MaPI_FSMapReduce<T>::applyFunction()
{
   while (true) {
      int id_func;
      MPI_Recv(&id_func, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &MRStat);
      if (id_func == -1) {
         MPI_Finalize();
         exit(0);
      }

      int input_size;
      MPI_Recv(&input_size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &MRStat);

      char input[input_size];
      MPI_Recv(&input, input_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &MRStat);
      if (id_func >= functionsT.size()) {
         std::cout << "\nERRO: overflow id_func\n";
         exit(1);
      }

      string _output = input[0] == '_' ? "_" : functionsT[id_func](string(input), shared);
      int output_size = _output.size() + 1;

      char* output = (char*)_output.c_str();
      MPI_Send(&output_size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
      MPI_Send(output, output_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);
   }
}

template<class T>
vector<string>*
MaPI_FSMapReduce<T>::mapper(string (*map)(string, T), vector<string>* inputs)
{
   vector<string>* outputs = new vector<string>;

   int numMapProcs = MRSize - 1;
   int numInputs = inputs->size();

   // Retorna posição da função no vetor MRFunctions
   int id_func = 0;
   while (map != functionsT[id_func])
      id_func++;

   // Caso 1 - Número de entradas menor ou igual ao número de processadores (n-1)
   if (numInputs <= numMapProcs) {
      // Mensagem
      // enviar para os processos
      for (int i = 0; i < numInputs; i++) { //cout << "Enviando" << i << std::endl;
         // enviar
         int stsize = inputs->at(i).size() + 1;
         char st[stsize];
         strcpy(st, inputs->at(i).c_str());

         MPI_Send(&id_func, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);

         MPI_Send(&stsize, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);

         MPI_Send(st, stsize, MPI_CHAR, i + 1, 1, MPI_COMM_WORLD);
      }

      // enviar _ para os demais processos
      for (int i = numInputs; i < numMapProcs; i++) { //cout << "Enviando_" << i << std::endl;
         // enviar
         int stsize = 1;
         char st[] = "_";
         MPI_Send(&id_func, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
         MPI_Send(&stsize, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
         MPI_Send(st, stsize, MPI_CHAR, i + 1, 1, MPI_COMM_WORLD);
      }

      // receber
      for (int i = 1; i <= numMapProcs; i++) { //cout << "Recebendo" << i << std::endl;
         // enviar
         int stsize;
         MPI_Recv(&stsize, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &MRStat);
         char st[stsize];
         MPI_Recv(&st, stsize, MPI_CHAR, i, 1, MPI_COMM_WORLD, &MRStat);
         if (st[0] != '_')
            outputs->push_back(string(st));
      }
   }

   else

   // Caso 2 - Número de entradas maior que o número de processadores (n-1)
   {
      // Mensagem
      // enviar para os processos
      for (int i = 0; i < numInputs; i++) {
         int dest = i % numMapProcs + 1;
         //cout << "EnviandoDest " << dest << std::endl;
         // enviar
         int stsize = inputs->at(i).size();
         char st[stsize];
         strcpy(st, inputs->at(i).c_str());
         MPI_Send(&id_func, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
         MPI_Send(&stsize, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
         MPI_Send(st, stsize, MPI_CHAR, dest, 1, MPI_COMM_WORLD);
      }

      // receber
      for (int i = 0; i < numInputs; i++) {
         int from = i % numMapProcs + 1;
         int stsize;
         MPI_Recv(&stsize, 1, MPI_INT, from, 1, MPI_COMM_WORLD, &MRStat);
         char st[stsize];
         MPI_Recv(&st, stsize, MPI_CHAR, from, 1, MPI_COMM_WORLD, &MRStat);
         if (st[0] != '_')
            outputs->push_back(string(st));
      }
   }

   return outputs;
}

template<class T>
vector<string>*
MaPI_FSMapReduce<T>::mapreduce(string (*map)(string, T), vector<string>* (*reduce)(std::vector<string>*, T), vector<string>* input)
{
   // Map
   std::cout << "== Mapper ==\n";
   vector<string>* mapped = mapper(map, input);
   // Reduce
   std::cout << "== Reducer ==\n";
   vector<string>* output = reduce(mapped, shared);
   delete mapped;
   return output;
}

template<class T>
void
MaPI_FSMapReduce<T>::finalize()
{
   // Termina servidores
   int id_func = -1;
   for (int i = 1; i < MRSize; i++)
      MPI_Send(&id_func, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

   MPI_Finalize();
}

#endif /* FSMAPREDUCE_H */
