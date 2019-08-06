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

#ifndef MaPI_MAPREDUCE_HPP
#define MaPI_MAPREDUCE_HPP

///MaPI_MapReduce.
/**
Esta classe deve ser única na aplicação. Além disso é preciso registrar todos os mappers e todos os reducers.
*/
template<class KeyA, class A, class KeyB, class B, class C>
class MaPI_MapReduce : public MapReduce<KeyA,A,KeyB,B,C>
{
public:
	///MapReduce execution (implemented by library).
	virtual vector< pair<KeyB,C> > run( Mapper<KeyA,A,KeyB,B,C> & mapper, Reducer<KeyA,A,KeyB,B,C> & reducer, vector< pair<KeyA,A> > & as)
	{
		if (mappers.size() == 0 || reducers.size() == 0 ) { 
			cout << "Aborting: It is necessary to registry almost one mapper and almost one reducer\n"; 
			return vector< pair<KeyB,C> >(); 
		}
		//cout << "Input  :\t" << as << endl;
#ifndef MRI_USE_MULTIMAP
		vector< pair<KeyB,B> > bs = mapper.run( as );
#else
		multimap<KeyB,B> bs = mapper.run( as );
#endif
		//cout << "Mapped :\t" << bs << endl;
		vector< pair<KeyB,C> > cs = reducer.run( bs );
		//cout << "Reduced:\t" << cs << endl;
		return cs;
	};
	///Constructor.
	MaPI_MapReduce(){};
	///Constructor.
	MaPI_MapReduce(MaPI_Mapper<KeyA,A,KeyB,B,C> * _mapper, MaPI_Reducer<KeyA,A,KeyB,B,C> * _reducer)
	{
		mappers.push_back(_mapper);
		reducers.push_back(_reducer);
	};
	///Mapper and Reducer registry.
	void registry(MaPI_Mapper<KeyA,A,KeyB,B,C> * _mapper, MaPI_Reducer<KeyA,A,KeyB,B,C> * _reducer)
	{
		mappers.push_back(_mapper);
		reducers.push_back(_reducer);
	};
	///Mapper registry.
	void registry(MaPI_Mapper<KeyA,A,KeyB,B,C> * _mapper) { mappers.push_back(_mapper); };
	///Reducer registry.
	void registry(MaPI_Reducer<KeyA,A,KeyB,B,C> * _reducer) { reducers.push_back(_reducer); };
	///Servers initialization.
	void initServers(int argc, char **argv)
	{ 
	    MPI_Init(&argc,&argv);
	    MPI_Comm_size(MPI_COMM_WORLD,&MRSize);
	    MPI_Comm_rank(MPI_COMM_WORLD,&MRRank);
	    if (MRRank!=0) server();
	};
	///Destructor.
	virtual ~MaPI_MapReduce(){ finalize(); };
	///Mapper identification.
	int id(MaPI_Mapper<KeyA,A,KeyB,B,C> * _mapper){ int i=0; while (mappers[i]!=_mapper) i++; return i; };
	///Mapper identification.
	int id(MaPI_Reducer<KeyA,A,KeyB,B,C> * _reducer){ int i=0; while (reducers[i]!=_reducer) i++; return i; };
	///Number of processes.
	int getMPISize(){return MRSize;};
	///MRRank.
	int getMPIRank(){return MRRank;};
	///MRStat.
	MPI_Status * getMPIStatus(){return &MRStat;};
protected:
	vector<MaPI_Mapper<KeyA,A,KeyB,B,C> * > mappers;
	vector<MaPI_Reducer<KeyA,A,KeyB,B,C> * > reducers;
	int MRSize,MRRank;
	MPI_Status MRStat;
	void server()
	{
	    while (true) 
	    {
	        int operation; // 0: to terminate ; 1: to map ; 2: to reduce ;
	        MPI_Recv(&operation, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &MRStat);    
	        if (operation == 0) { MPI_Finalize(); exit(0); }
	
	        int object_id; // where the object is an instance of Mapper or an instance of Reducer
	        MPI_Recv(&object_id, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &MRStat);    
	
	        int input_size;
	        MPI_Recv(&input_size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &MRStat);    
	
	        char input[input_size]; // Receive a serialization of "pair<KeyA,A>" or "pair<KeyB, vector<B> >"
	        MPI_Recv(&input, input_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &MRStat);

			if (operation == 1) // Mapping
			{
				if (object_id >= mappers.size()) { cout << "\nERROR: Overflow at map server\n"; exit(1); }

				//printf("Server mapper called\n");

				stringstream _output;
				if (input[0]=='_') 
					_output << "_";
				else {
					pair<KeyA,A> a;

					Scanner scan(input);
					scan.useSeparators("\1");

					a.first  = mappers[object_id]->getSerializer()->KeyA_fromString(scan.next());
					a.second = mappers[object_id]->getSerializer()->A_fromString(scan.next());
					
					//cout << "#" << a << endl;

					vector< pair<KeyB,B> > mapped = mappers[object_id]->map( a );

					typename vector< pair<KeyB,B> >::iterator it;
					for (it = mapped.begin() ; it != mapped.end() ; ++it) 
					{
						_output  << mappers[object_id]->getSerializer()->KeyB_toString((*it).first) << "\1"
								 << mappers[object_id]->getSerializer()->B_toString((*it).second) << "\1" ; 
					}

					//cout << "_output: " << _output.str() << endl;
				}

		        int output_size = _output.str().size() + 1;	
            	char output[output_size];
            	strcpy(output,_output.str().c_str());
		        MPI_Send(&output_size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		        MPI_Send(output, output_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);				
			} 
			else
			if (operation == 2) // Reducing
			{
				if (object_id >= reducers.size()) { cout << "\nERROR: Overflow at reduce server\n"; exit(1); }

				//printf("Server reductor called\n");

				//cout << input << endl;

				stringstream _output;
				if (input[0]=='_') 
					_output << "_";
				else {
					pair < KeyB,vector<B> > b;

					Scanner scan(input);
					scan.useSeparators("\1");

					b.first = reducers[object_id]->getSerializer()->KeyB_fromString(scan.next());
					while (scan.hasNext()) b.second.push_back(reducers[object_id]->getSerializer()->B_fromString(scan.next()));

					//cout << b.first << endl;
					//for (int i = 0 ; i < b.second.size() ; i++) cout << "#" << b.second[i] << endl;

					pair < KeyB , C > reduced = reducers[object_id]->reduce( b );
					_output << reducers[object_id]->getSerializer()->KeyB_toString(reduced.first) << "\1"
							<< reducers[object_id]->getSerializer()->C_toString(reduced.second) << "\1";
					//cout << "_output: " << _output.str() << endl;
				}

		        int output_size = _output.str().size() + 1;	
            	char output[output_size];
            	strcpy(output,_output.str().c_str());
		        MPI_Send(&output_size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		        MPI_Send(output, output_size, MPI_CHAR, 0, 1, MPI_COMM_WORLD);				
			}
	    }
	};
	void finalize()
	{
	    int operation = 0;
	    for (int i = 1; i < MRSize; i++) MPI_Send(&operation, 1, MPI_INT,  i, 1, MPI_COMM_WORLD);
	    MPI_Finalize();
	}
};

///MaPI_StrMapReduce.
class MaPI_StrMapReduce : public MaPI_MapReduce<string,string,string,string,string> {};

#endif /* MaPI_MAPREDUCE_HPP */
