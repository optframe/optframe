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

#ifndef MaPI_MAPPER_HPP
#define MaPI_MAPPER_HPP
#include <iterator>

///MaPI_Mapper.
template<class KeyA, class A, class KeyB, class B, class C>
class MaPI_Mapper : public Mapper<KeyA, A, KeyB, B, C>
{
public:
   ///Constructor.
   MaPI_Mapper(MaPI_MapReduce<KeyA, A, KeyB, B, C>* _mapReduce, MaPI_Serializer<KeyA, A, KeyB, B, C>* _serializer)
     : mapReduce(_mapReduce)
     , serializer(_serializer)
   {
      mapReduce->registry(this);
   };
      ///Iterator mapping execution (implemented by library).
#ifndef MRI_USE_MULTIMAP
   virtual std::vector<std::pair<KeyB, B>> run(std::vector<pair<KeyA, A>>& as)
   {
      std::vector<std::pair<KeyB, B>> bs;

#else
   virtual multimap<KeyB, B> run(std::vector<pair<KeyA, A>>& as)
   {
      multimap<KeyB, B> bs;
#endif

      vector<string> outputs;

      vector<string> inputs;
      typename std::vector<std::pair<KeyA, A>>::iterator itAs;
      for (itAs = as.begin(); itAs != as.end(); ++itAs) {
         stringstream s;
         s << serializer->KeyA_toString((*itAs).first) << "\1"
           << serializer->A_toString((*itAs).second) << "\1";
         inputs.push_back(s.str());
      }

      int numMapProcs = mapReduce->getMPISize() - 1;
      int numInputs = inputs.size();
      int mapperId = mapReduce->id(this);
      int operation = 1;

      // Caso 1 - Número de entradas menor ou igual ao número de processadores (n-1)
      if (numInputs <= numMapProcs) {
         // Mensagem
         // enviar para os processos
         for (int i = 0; i < numInputs; i++) { //cout << "Enviando" << i << std::endl;
            // enviar

            int stsize = inputs.at(i).size() + 1;
            char st[stsize];
            strcpy(st, inputs.at(i).c_str());

            MPI_Send(&operation, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
            MPI_Send(&mapperId, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
            MPI_Send(&stsize, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
            MPI_Send(st, stsize, MPI_CHAR, i + 1, 1, MPI_COMM_WORLD);
         }

         // enviar _ para os demais processos
         for (int i = numInputs; i < numMapProcs; i++) { //cout << "Enviando_" << i << std::endl;
            // enviar
            int stsize = 1;
            char st[] = "_";
            MPI_Send(&operation, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
            MPI_Send(&mapperId, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
            MPI_Send(&stsize, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
            MPI_Send(st, stsize, MPI_CHAR, i + 1, 1, MPI_COMM_WORLD);
         }

         // receber
         for (int i = 1; i <= numMapProcs; i++) {
            // enviar
            int stsize;
            MPI_Recv(&stsize, 1, MPI_INT, i, 1, MPI_COMM_WORLD, mapReduce->getMPIStatus());
            char st[stsize];
            MPI_Recv(&st, stsize, MPI_CHAR, i, 1, MPI_COMM_WORLD, mapReduce->getMPIStatus());
            if (st[0] != '_')
               outputs.push_back(string(st));
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
            int stsize = inputs.at(i).size();
            char st[stsize];
            strcpy(st, inputs.at(i).c_str());
            MPI_Send(&operation, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&mapperId, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(&stsize, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
            MPI_Send(st, stsize, MPI_CHAR, dest, 1, MPI_COMM_WORLD);
         }

         // receber
         for (int i = 0; i < numInputs; i++) {
            int from = i % numMapProcs + 1;
            int stsize;
            MPI_Recv(&stsize, 1, MPI_INT, from, 1, MPI_COMM_WORLD, mapReduce->getMPIStatus());
            char st[stsize];
            MPI_Recv(&st, stsize, MPI_CHAR, from, 1, MPI_COMM_WORLD, mapReduce->getMPIStatus());
            if (st[0] != '_')
               outputs.push_back(string(st));
         }
      }

      for (int i = 0; i < outputs.size(); i++) {
         Scanner scan(outputs[i]);
         scan.useSeparators("\1");

         while (scan.hasNext()) {
            pair<KeyB, B> p;
            p.first = getSerializer()->KeyB_fromString(scan.next());
            p.second = getSerializer()->B_fromString(scan.next());
#ifndef MRI_USE_MULTIMAP
            bs.push_back(p);
#else
            bs.insert(p);
#endif
         }
      }

      return bs;
   };
   ///Map function (implemented by user).
   virtual std::vector<std::pair<KeyB, B>> map(pair<KeyA, A>) = 0;

   MaPI_Serializer<KeyA, A, KeyB, B, C>* getSerializer() { return serializer; };

   void setSerializer(MaPI_Serializer<KeyA, A, KeyB, B, C>* _serializer) { serializer = _serializer; };

protected:
   MaPI_MapReduce<KeyA, A, KeyB, B, C>* mapReduce;
   MaPI_Serializer<KeyA, A, KeyB, B, C>* serializer;
};

///MaPI_StrMapper.
class MaPI_StrMapper : public MaPI_Mapper<string, string, string, string, string>
{
public:
   ///Constructor.
   MaPI_StrMapper(MaPI_MapReduce<string, string, string, string, string>* _mapReduce)
     : MaPI_Mapper<string, string, string, string, string>(_mapReduce, nullptr)
   {
      serializer = new MaPI_StrSerializer();
      deleteSerializer = true;
   };
   ///Constructor.
   MaPI_StrMapper(MaPI_MapReduce<string, string, string, string, string>* _mapReduce,
                  MaPI_Serializer<string, string, string, string, string>* _serializer)
     : MaPI_Mapper<string, string, string, string, string>(_mapReduce, _serializer)
   {
      deleteSerializer = false;
   };
   ///Destructor.
   virtual ~MaPI_StrMapper()
   {
      if (deleteSerializer)
         delete serializer;
   };

private:
   bool deleteSerializer;
};

#endif /* MaPI_MAPPER_HPP */
