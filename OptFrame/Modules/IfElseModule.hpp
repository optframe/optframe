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

#ifndef IFELSE_MODULE_HPP_
#define IFELSE_MODULE_HPP_

#include<string>

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class IfElseModule: public OptFrameModule<R, ADS, M>
{
private:
	OptFrameModule<R, ADS, M>* getModule(vector<OptFrameModule<R, ADS, M>*>& modules, string module)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
			if (module == modules[i]->id())
				return modules[i];
		return NULL;
	}

	bool exec_command(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string command)
	{
		Scanner scanner(command);
		string module = scanner.next();
		OptFrameModule<R, ADS, M>* m = getModule(all_modules, module);

		if (m == NULL)
			return false;

		string rest = m->preprocess(allFunctions, dictionary, ldictionary, scanner.rest());
		return m->run(all_modules, allFunctions, factory, dictionary, ldictionary, rest);
	}

public:

	virtual ~IfElseModule()
	{
	}

	string id()
	{
		return "if_else";
	}

	string usage()
	{
		return "if_else boolean list_of_if_commands [list_of_else_commands]";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string sbool = scanner.next();

		bool condition = (sbool=="true");


		vector<string>  lif;
		vector<string>* p_lif = OptFrameList::readList(ldictionary, scanner);
		if(p_lif)
		{
			lif = vector<string>(*p_lif);
			delete p_lif;
		}
		else
			return false;

		vector<string>  lelse;
		if(scanner.hasNext())
		{
			vector<string>* p_lelse = OptFrameList::readList(ldictionary, scanner);
			if(p_lelse)
			{
				lelse = vector<string>(*p_lelse);
				delete p_lelse;
			}
			else
				return false;
		}

		if (condition)
		{
			for (unsigned int c = 0; c < lif.size(); c++)
			{
				string command = lif.at(c);

				if (command.at(0) == '%') // first line comment
					command = "";

				if (command != "")
					if (!exec_command(all_modules, allFunctions, factory, dictionary, ldictionary, command))
					{
						if (lif.at(c) == "")
							cout << "if_else module: (IF) empty command! (perhaps an extra comma in list?)" << endl;
						else
							cout << "if_else module: (IF) error in command '" << lif.at(c) << "'" << endl;

						return false;
					}
			}
		}
		else
		{
			for (unsigned int c = 0; c < lelse.size(); c++)
			{
				string command = lelse.at(c);

				if(command.at(0)=='%') // first line comment
					command = "";

				if (command != "")
					if(!exec_command(all_modules, allFunctions, factory, dictionary, ldictionary, command))
					{
						if(lelse.at(c)=="")
							cout << "if_else module: (ELSE) empty command! (perhaps an extra comma in list?)" << endl;
						else
							cout << "if_else module: (ELSE) error in command '" << lelse.at(c) << "'" << endl;

						return false;
					}
			}
		}

		return true;
	}

	// should preprocess only until list of commands
	virtual string preprocess(vector<OptFrameFunction*>& allFunctions, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		string ibegin = "";
		string iend   = "";
		unsigned j = 0;
		for(unsigned i=0; i<input.length(); i++)
		{
			if(input.at(i)=='[')
				break;
			else
				ibegin += input.at(i);
			j++;
		}

		for(unsigned k=j; k<input.length(); k++)
			iend += input.at(k);

		string ninput = OptFrameModule<R, ADS, M>::defaultPreprocess(allFunctions, dictionary, ldictionary, ibegin);

		ninput.append(" "); // after boolean value
		ninput.append(iend);

		return ninput;
	}
};

#endif /* IFELSE_MODULE_HPP_ */
