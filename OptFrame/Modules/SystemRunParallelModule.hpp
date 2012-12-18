// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#include "../OptFrameModule.hpp"


template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class SystemRunParallelModule: public OptFrameModule<R, ADS, M>
{
private:
	// INPUT
	vector<vector<string> > allCommands;

	// AUXILIAR INPUT
	vector<OptFrameModule<R, ADS, M>*>* allModules;
	vector<OptFrameFunction*>* allFunctions;
	HeuristicFactory<R, ADS, M>* factory;
	map<string, string>* dictionary;
	map< string,vector<string> >* ldictionary;

    // AUXILIAR FUNCTION
    OptFrameModule<R, ADS, M>* getModule(string module)
    {
    	for (unsigned int i = 0; i < allModules->size(); i++)
    		if (module == allModules->at(i)->id())
    			return allModules->at(i);
    	return NULL;
    }

	// RESULTS
	vector<bool> results;

	// THREADS
	vector<pthread_t>       threads;
	vector<pthread_mutex_t> mutexes;

public:

	SystemRunParallelModule()
	{
		allModules   = NULL;
		allFunctions = NULL;
		factory      = NULL;
		dictionary   = NULL;
		ldictionary  = NULL;
	}

	virtual ~SystemRunParallelModule()
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

	bool run(vector<OptFrameModule<R, ADS, M>*>& _allModules, vector<OptFrameFunction*>& _allFunctions, HeuristicFactory<R, ADS, M>& _factory, map<string, string>& _dictionary,  map< string,vector<string> >& _ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		allCommands.clear();

		allModules   = &_allModules;
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

	        pthread_mutex_init(&mutexes[t], NULL);
	        pthread_create(&threads[t], 0, &SystemRunParallelModule<R, ADS, M>::start_thread, this);
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
	virtual string* preprocess(vector<OptFrameFunction*>&, map<string, string>&, map< string,vector<string> >&, string input)
	{
		return new string(input); // disable pre-processing
	}




	// This is the static class function that serves as a C style function pointer
	// for the pthread_create call
	static void* start_thread(void *obj)
	{
		reinterpret_cast<SystemRunParallelModule<R, ADS, M> *>(obj)->exec_command();
		pthread_exit(NULL);
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
    		OptFrameModule<R, ADS, M>* m = getModule(module);

    		if (m == NULL)
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
    		bool r = m->run(*allModules, *allFunctions, *factory, *dictionary, *ldictionary, rest);
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
