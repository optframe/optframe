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

#ifndef TRY_MODULE_HPP_
#define TRY_MODULE_HPP_

#include<string>

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class TryModule: public OptFrameModule<R, ADS, M>
{
private:
	OptFrameModule<R, ADS, M>* getModule(vector<OptFrameModule<R, ADS, M>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

	bool exec_command(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		OptFrameModule<R, ADS, M>* m = getModule(all_modules, module);

		if (m == NULL)
			return false;

		string* rest = m->preprocess(allFunctions, dictionary, ldictionary, scanner.rest());

		if(!rest)
			return NULL;

		bool b = m->run(all_modules, allFunctions, factory, dictionary, ldictionary, *rest);

		delete rest;

		return b;
	}

public:

	virtual ~TryModule()
	{
	}

	string id()
	{
		return "try";
	}

	string usage()
	{
		return "try block_of_try_commands [except block_of_exception_commands]";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string>  ltry;
		vector<string>* p_ltry = OptFrameList::readBlock(scanner);
		if(p_ltry)
		{
			ltry = vector<string>(*p_ltry);
			delete p_ltry;
		}
		else
			return false;

		vector<string>  lcatch;
		if(scanner.hasNext())
		{
			string text_except = scanner.next();

			if(text_except != "except")
			{
				cout << "try module: error expecting word 'except' and got '" << text_except << "'" << endl;
				return false;
			}

			vector<string>* p_lcatch = OptFrameList::readBlock(scanner);
			if(p_lcatch)
			{
				lcatch = vector<string>(*p_lcatch);
				delete p_lcatch;
			}
			else
				return false;
		}

		if(!OptFrameModule<R, ADS, M>::testUnused(id(), scanner))
			return false;

		bool ok = true;

		for (unsigned int c = 0; c < ltry.size(); c++)
		{
			string command = ltry.at(c);

			if (command.at(0) == '%') // first line comment
				command = "";

			if (command != "")
				if (!exec_command(allModules, allFunctions, factory, dictionary, ldictionary, command))
				{
					// EXPECTED ERROR, NO MESSAGE!
					/*
					if (ltry.at(c) == "")
						cout << "try module: (TRY) empty command! (perhaps an extra comma in list?)" << endl;
					else
						cout << "try module: (TRY) problem in command '" << ltry.at(c) << "'" << endl;
					*/

					ok = false;
					break;
				}
		}

		if (!ok)
		{
			// DEBUG MESSAGE ONLY
			/*
			if(lcatch.size()>0)
				cout << "try_catch module: calling CATCH block" << endl;
			*/

			for (unsigned int c = 0; c < lcatch.size(); c++)
			{
				string command = lcatch.at(c);

				if(command.at(0)=='%') // first line comment
					command = "";

				if (command != "")
					if(!exec_command(allModules, allFunctions, factory, dictionary, ldictionary, command))
					{
						if(lcatch.at(c)=="")
							cout << "try module: (AFTER ERROR) empty command! (perhaps an extra comma in list?)" << endl;
						else
							cout << "try module: (AFTER ERROR) error in command '" << lcatch.at(c) << "'!" << endl;

						return false;
					}
			}
		}

		return true;
	}

	// should preprocess only until list of commands
	virtual string* preprocess(vector<OptFrameFunction*>& allFunctions, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		string ibegin = "";
		string iend   = "";
		unsigned j = 0;
		for(unsigned i=0; i<input.length(); i++)
		{
			if(input.at(i)=='{')
				break;
			else
				ibegin += input.at(i);
			j++;
		}

		for(unsigned k=j; k<input.length(); k++)
			iend += input.at(k);

		string* ninput = OptFrameModule<R, ADS, M>::defaultPreprocess(allFunctions, dictionary, ldictionary, ibegin);

		if(!ninput)
			return NULL;

		ninput->append(" "); // after boolean value
		ninput->append(iend);

		return ninput;
	}
};

#endif /* TRY_MODULE_HPP_ */
