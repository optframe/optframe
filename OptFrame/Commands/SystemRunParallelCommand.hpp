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

#ifndef OPTFRAME_RUN_PARALLEL_MODULE_HPP_
#define OPTFRAME_RUN_PARALLEL_MODULE_HPP_

#ifdef OPTFRAME_PARALLEL

#include<string>

#include <pthread.h>

#include "../Command.hpp"


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_EMEMORY>
class SystemRunParallelCommand: public Command<R, ADS, DS>
{
private:
	// INPUT
	vector<vector<string> > allCommands;

	// AUXILIAR INPUT
	vector<Command<R, ADS, DS>*>* allCommands;
	vector<PreprocessFunction<R, ADS, DS>*>* allFunctions;
	HeuristicFactory<R, ADS, DS>* factory;
	map<string, string>* dictionary;
	map< string,vector<string> >* ldictionary;

    // AUXILIAR FUNCTION
    Command<R, ADS, DS>* getCommand(string module)
    {
    	for (unsigned int i = 0; i < allCommands->size(); i++)
    		if (module == allCommands->at(i)->id())
    			return allCommands->at(i);
    	return nullptr;
    }

	// RESULTS
	vector<bool> results;

	// THREADS
	vector<pthread_t>       threads;
	vector<pthread_mutex_t> mutexes;

public:

	SystemRunParallelCommand()
	{
		allCommands   = nullptr;
		allFunctions = nullptr;
		factory      = nullptr;
		dictionary   = nullptr;
		ldictionary  = nullptr;
	}

	virtual ~SystemRunParallelCommand()
	{
	}

	string id()
	{
		return "system.run_parallel";
	}

	string usage()
	{
		return "system.run_parallel block_of_commands_0 [block_of_commands_1] [block_of_commands_2] ... ";
	}

	bool run(vector<Command<R, ADS, DS>*>& _allCommands, vector<PreprocessFunction<R, ADS, DS>*>& _allFunctions, HeuristicFactory<R, ADS, DS>& _factory, map<string, string>& _dictionary,  map< string,vector<string> >& _ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		allCommands.clear();

		allCommands   = &_allCommands;
		allFunctions = &_allFunctions;
		factory      = &_factory;
		dictionary   = &_dictionary;
		ldictionary  = &_ldictionary;

		while(scanner.hasNext())
		{
			vector<string>  lcommands;
			vector<string>* p_lcommands = OptFrameList::readBlock(scanner);
			if(p_lcommands)
			{
				lcommands = vector<string>(*p_lcommands);
				delete p_lcommands;
			}
			else
				return false;

			allCommands.push_back(lcommands);
		}

		results = vector<bool>(allCommands.size(), false);

		threads.clear();
		mutexes.clear();

		pthread_attr_t attr;

		// Initialize and set thread joinable
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

		for(unsigned t=0; t<allCommands.size(); t++)
		{
			pthread_t       thread;
			pthread_mutex_t mutex;

	        threads.push_back(thread);
	        mutexes.push_back(mutex);

	        pthread_mutex_init(&mutexes[t], nullptr);
	        pthread_create(&threads[t], 0, &SystemRunParallelCommand<R, ADS, DS>::start_thread, this);
		}


		// free attribute and wait for the other threads
		pthread_attr_destroy(&attr);

		bool r = true;
		for(unsigned t=0; t<allCommands.size(); t++)
		{
			void* status;
			pthread_join(threads[t], &status);
	        pthread_mutex_destroy(&mutexes[t]);
			r &= results[t];
		}

		return r;
	}


	// leave preprocessing to each module
	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return new string(input); // disable pre-processing
	}




	// This is the static class function that serves as a C style function pointer
	// for the pthread_create call
	static void* start_thread(void *obj)
	{
		reinterpret_cast<SystemRunParallelCommand<R, ADS, DS> *>(obj)->exec_command();
		pthread_exit(nullptr);
	}

    void exec_command()
    {
        pthread_t ptid = pthread_self();

        int id = -1;

        for(int t=0; t<(int)threads.size(); t++)
        	if(pthread_equal(ptid,threads[t]))
        	{
        		id = t;
        		break;
        	}

        if(id < 0)
        	return;

        pthread_mutex_t mutex   = mutexes[id];

    	vector<string> commands = allCommands[id];

    	bool ok = true;

    	for(unsigned c=0; c<commands.size(); c++)
    	{
    		string command = commands.at(c);

    		Scanner scanner(command);
    		string module = scanner.next();
    		Command<R, ADS, DS>* m = getCommand(module);

    		if (m == nullptr)
    			return;

    		string* rest1 = m->preprocess(*allFunctions, *dictionary, *ldictionary, scanner.rest());

    		string rest = "";

    		if(!rest1)
    		{
    			ok = false;
    			break;
    		}
    		else
    		{
    			rest = *rest1;
    			delete rest1;
    		}

    		pthread_mutex_lock(&mutex);
    		bool r = m->run(*allCommands, *allFunctions, *factory, *dictionary, *ldictionary, rest);
    		pthread_mutex_unlock(&mutex);

    		ok &= r;

    		if(!r)
    			break;
    	}

    	results[id] = ok;
    }

};

#endif /* OPTFRAME_PARALLEL */

#endif /* OPTFRAME_RUN_PARALLEL_MODULE_HPP_ */
