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

#ifndef MaPI_REDUCER_HPP
#define MaPI_REDUCER_HPP

///MaPI_Reducer.
template<class KeyA, class A, class KeyB, class B, class C>
class MaPI_Reducer : public Reducer<KeyA,A,KeyB,B,C>
{ 
public:
	///Constructor.
	MaPI_Reducer(MaPI_MapReduce<KeyA,A,KeyB,B,C> * _mapReduce, MaPI_Serializer<KeyA,A,KeyB,B,C> * _serializer)
		:mapReduce(_mapReduce),serializer(_serializer){ mapReduce->registry(this); };
	///Iterator reducing execution (implemented by library).
#ifndef MRI_USE_MULTIMAP
	virtual vector< pair<KeyB,C> > run( vector< pair<KeyB,B> > & _mapped)
	{
		multimap<KeyB,B> mapped;
		for (int i = 0 ; i < _mapped.size() ; i++) mapped.insert(_mapped[i]);
#else
	virtual vector< pair<KeyB,C> > run( multimap<KeyB,B> & mapped)
	{
#endif

		vector< pair<KeyB,C> > reduced;

		typename std::multimap<KeyB,B>::iterator it = mapped.begin();
		KeyB lastKey = (*it).first;
		vector< pair < KeyB,vector<B> > > toReduce(1, pair < KeyB,vector<B> >( lastKey , vector<B>() ) );
		for( ; it != mapped.end() ; ++it)
		{
			if ((*it).first != lastKey) {
				toReduce.push_back( pair< KeyB,vector<B> > ( (*it).first , vector<B>() ) ); 
				lastKey = (*it).first;
			}
			toReduce.back().second.push_back( (*it).second );
		}

		// Parallel Reduction

	    vector<string> outputs;

		vector<string> inputs;
		typename vector< pair < KeyB,vector<B> > >::iterator itr;
		for (itr = toReduce.begin() ; itr != toReduce.end() ; ++itr) 
		{
			stringstream s; s << (*itr).first << "\1";
			for (int i = 0 ; i < (*itr).second.size() ; i++) s << (*itr).second[i] << "\1";
			inputs.push_back(s.str());
		}

	    int numMapProcs = mapReduce->getMPISize()-1;
	    int numInputs   = inputs.size();
	    int reducerId	= mapReduce->id(this);
		int operation	= 2;

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
            	MPI_Send(&reducerId, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
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
    	        MPI_Send(&reducerId, 1, MPI_INT, i+1, 1, MPI_COMM_WORLD);
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
				//cout << "st: " << st << endl;
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
    	        MPI_Send(&reducerId, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
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
				pair<KeyB,C> p;
				p.first = getSerializer()->KeyB_fromString(scan.next());
				p.second = getSerializer()->C_fromString(scan.next());
				reduced.push_back(p);
			}
		}

		return reduced;
	};
	///Reduce function (implemented by user).
	virtual pair<KeyB,C> reduce( pair<KeyB, vector<B> > ) = 0 ;

	MaPI_Serializer<KeyA,A,KeyB,B,C> * getSerializer(){return serializer;};

	void setSerializer(MaPI_Serializer<KeyA,A,KeyB,B,C> * _serializer){serializer = _serializer;};

protected:
	MaPI_MapReduce<KeyA,A,KeyB,B,C> * mapReduce;
	MaPI_Serializer<KeyA,A,KeyB,B,C> * serializer;
};

///MaPI_StrReducer.
class MaPI_StrReducer : public MaPI_Reducer<string,string,string,string,string> 
{
public:
	///Constructor.
	MaPI_StrReducer(MaPI_MapReduce<string,string,string,string,string> * _mapReduce)
		:MaPI_Reducer<string,string,string,string,string>(_mapReduce,nullptr) { serializer = new MaPI_StrSerializer(); deleteSerializer = true; };
	///Constructor.
	MaPI_StrReducer(MaPI_MapReduce<string,string,string,string,string> * _mapReduce, 
					MaPI_Serializer<string,string,string,string,string> * _serializer)
		:MaPI_Reducer<string,string,string,string,string>(_mapReduce,_serializer) { deleteSerializer = false; };
	///Destructor.
	virtual ~MaPI_StrReducer(){ if (deleteSerializer) delete serializer; };
private:
	bool deleteSerializer;
};

#endif /* MaPI_REDUCER_HPP */
