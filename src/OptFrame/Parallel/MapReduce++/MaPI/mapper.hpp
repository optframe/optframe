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

#ifndef MaPI_MAPPER_HPP
#define MaPI_MAPPER_HPP
#include <iterator>

///MaPI_Mapper.
template<class KeyA, class A, class KeyB, class B, class C>
class MaPI_Mapper : public Mapper<KeyA,A,KeyB,B,C>
{ 
public:
	///Constructor.
	MaPI_Mapper(MaPI_MapReduce<KeyA,A,KeyB,B,C> * _mapReduce, MaPI_Serializer<KeyA,A,KeyB,B,C> * _serializer)
		:mapReduce(_mapReduce),serializer(_serializer){ mapReduce->registry(this); };
	///Iterator mapping execution (implemented by library).
#ifndef MRI_USE_MULTIMAP
	virtual vector< pair<KeyB,B> > run( vector< pair<KeyA,A> > & as )
	{
		vector< pair<KeyB,B> > bs;

#else
	virtual multimap<KeyB,B> run( vector< pair<KeyA,A> > & as )
	{
		multimap<KeyB,B> bs;
#endif

	    vector<string> outputs;

		vector<string> inputs;
		typename vector< pair<KeyA,A> >::iterator itAs;
		for (itAs = as.begin() ; itAs != as.end() ; ++itAs) 
		{
			stringstream s; 
			s << serializer->KeyA_toString((*itAs).first)	<< "\1" 
			  << serializer->A_toString((*itAs).second)	<< "\1" ;
			inputs.push_back( s.str() );
		}

	    int numMapProcs = mapReduce->getMPISize()-1;
	    int numInputs   = inputs.size();
	    int mapperId	= mapReduce->id(this);
		int operation	= 1;

	    // Caso 1 - Número de entradas menor ou igual ao número de processadores (n-1)
    	if (numInputs <= numMapProcs) 
   		{
        	// Mensagem
       		// enviar para os processos
        	for (int i = 0; i < numInputs; i++)
        	{   //cout << "Enviando" << i << endl;
            	// enviar

            	int  stsize = inputs.at(i).size() + 1;
            	char st[stsize];
            	strcpy(st,inputs.at(i).c_str());

            	MPI_Send(&operation, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
            	MPI_Send(&mapperId, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
            	MPI_Send(&stsize, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
            	MPI_Send(st, stsize, MPI_CHAR, i+1, 1, MPI_COMM_WORLD);
	        }
        
	        // enviar _ para os demais processos
    	    for (int i = numInputs; i < numMapProcs; i++) 
    	    {   //cout << "Enviando_" << i << endl;
    	        // enviar
    	        int  stsize = 1;
    	        char st[] = "_";
    	        MPI_Send(&operation, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
    	        MPI_Send(&mapperId, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
    	        MPI_Send(&stsize, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
    	        MPI_Send(st, stsize, MPI_CHAR, i+1, 1, MPI_COMM_WORLD);
    	    }
        
    	    // receber
    	    for (int i = 1; i <= numMapProcs; i++)
    	    {   
    	        // enviar
    	        int stsize;
    	        MPI_Recv(&stsize, 1, MPI_INT, i, 1, MPI_COMM_WORLD, mapReduce->getMPIStatus());
    	        char st[stsize];
    	        MPI_Recv(&st, stsize, MPI_CHAR, i, 1, MPI_COMM_WORLD, mapReduce->getMPIStatus());
    	        if (st[0] != '_') outputs.push_back(string(st));
    	    }        
    	}

    	else

    	// Caso 2 - Número de entradas maior que o número de processadores (n-1)
	    {
	        // Mensagem
    	    // enviar para os processos
    	    for (int i = 0; i < numInputs; i++)
    	    {
    	        int dest = i % numMapProcs + 1;
    	        //cout << "EnviandoDest " << dest << endl;
    	        // enviar
    	        int  stsize = inputs.at(i).size();
    	        char st[stsize];
    	        strcpy(st,inputs.at(i).c_str());
	            MPI_Send(&operation, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
    	        MPI_Send(&mapperId, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
    	        MPI_Send(&stsize, 1, MPI_INT,  dest, 1, MPI_COMM_WORLD);
	            MPI_Send(st, stsize, MPI_CHAR, dest, 1, MPI_COMM_WORLD);
    	    }
    	    
    	    // receber
    	    for (int i = 0; i < numInputs; i++)
    	    {	int from = i % numMapProcs + 1;
    	        int stsize;
    	        MPI_Recv(&stsize, 1, MPI_INT, from, 1, MPI_COMM_WORLD, mapReduce->getMPIStatus());
    	        char st[stsize];
    	        MPI_Recv(&st, stsize, MPI_CHAR, from, 1, MPI_COMM_WORLD, mapReduce->getMPIStatus());
    	        if (st[0] != '_') outputs.push_back(string(st));
    	    }        
    	}

		for (int i = 0 ; i < outputs.size() ; i++) 
		{
			Scanner scan(outputs[i]);
			scan.useSeparators("\1");

			while (scan.hasNext()) 
			{
				pair<KeyB,B> p;
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
	virtual vector< pair<KeyB,B> > map( pair<KeyA,A> ) = 0 ;

	MaPI_Serializer<KeyA,A,KeyB,B,C> * getSerializer(){return serializer;};

	void setSerializer(MaPI_Serializer<KeyA,A,KeyB,B,C> * _serializer){serializer = _serializer;};

protected:
	MaPI_MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
	MaPI_Serializer<KeyA,A,KeyB,B,C> * serializer;
};

///MaPI_StrMapper.
class MaPI_StrMapper : public MaPI_Mapper<string,string,string,string,string> 
{
public:
	///Constructor.
	MaPI_StrMapper(	MaPI_MapReduce<string,string,string,string,string> * _mapReduce)
		:MaPI_Mapper<string,string,string,string,string>(_mapReduce,nullptr) { serializer = new MaPI_StrSerializer(); deleteSerializer = true; };
	///Constructor.
	MaPI_StrMapper(	MaPI_MapReduce<string,string,string,string,string> * _mapReduce, 
					MaPI_Serializer<string,string,string,string,string> * _serializer)
		:MaPI_Mapper<string,string,string,string,string>(_mapReduce,_serializer) { deleteSerializer = false; };
	///Destructor.
	virtual ~MaPI_StrMapper(){ if (deleteSerializer) delete serializer; };
private:
	bool deleteSerializer;
};

#endif /* MaPI_MAPPER_HPP */
