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

#ifndef READMODULE_HPP_
#define READMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ReadModule : public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ReadModule()
	{
	}

	string id()
	{
		return "read";
	}

	string usage()
	{
		return "read filename";
	}

	string removeComments(string line)
	{
		stringstream ss;
		for(unsigned i=0; i<line.length(); i++)
			if(line.at(i)!='%')
				ss << line.at(i);
			else
				break;

		return Scanner::trim(ss.str());
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input)
	{
		Scanner trim(input);
		if(!trim.hasNext()) // no file
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		// Open file
		Scanner* scanner;

		try
		{
			scanner = new Scanner(new File(input));
		}
		catch (FileNotFound& e)
		{
			cout << "File '"<< e.getFile() << "' not found!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string> commands;

		while(scanner->hasNext())
		{
			string line = removeComments(scanner->nextLine());

			if(line.length()==0)
				continue;

			int lists  = 0;
			int blocks = 0;

			for(unsigned int c = 0; c < line.length(); c++)
			{
				if(line.at(c)=='[')
					lists++;
				if(line.at(c)==']')
					lists--;
				if(line.at(c)=='{')
					blocks++;
				if(line.at(c)=='}')
					blocks--;
			}

			while(((lists > 0) || (blocks > 0)) && scanner->hasNext())
			{
				string line2 = removeComments(scanner->nextLine());

				for(unsigned int c = 0; c < line2.length(); c++)
				{
					if(line2.at(c)=='[')
						lists++;
					if(line2.at(c)==']')
						lists--;
					if(line2.at(c)=='{')
						blocks++;
					if(line2.at(c)=='}')
						blocks--;
				}

				line.append(line2);
			}

			if((lists == 0) && (blocks == 0))
				commands.push_back(line);
			else
			{
				if(lists > 0)
					cout << "read error: wrong number of '[' and ']' => " << lists  << " '[' left open!" << endl;
				if(blocks > 0)
					cout << "read error: wrong number of '{' and '}' => " << blocks << " '{' left open!" << endl;

				delete scanner;
				return false;
			}
		}

		for(unsigned int i = 0; i < commands.size(); i++)
		{
			string line = commands[i];

			Scanner s2(line);

			if(!s2.hasNext()) // no command found in the line
			{
				cout << "read module: strange error! Empty command with line '" << line << "'" << endl;
				return false;
			}

			string command = s2.next();

			bool notfound = true;

			for(unsigned int i=0;i<all_modules.size();i++)
				if(command == all_modules[i]->id())
				{
					//cout << "READ COMMAND: '" << command << "'" << endl;

					string original = s2.rest();
					string* after_preprocess = all_modules[i]->preprocess(allFunctions, dictionary, ldictionary, original);

					if(!after_preprocess)
					{
						cout << "read module: preprocessing error!" << endl;
						return false;
					}

					//cout << "READ COMMAND INPUT: '" << after_preprocess << "'" << endl;

					if(!all_modules[i]->run(all_modules, allFunctions, factory, dictionary, ldictionary, *after_preprocess))
					{
						delete after_preprocess;
						return false;
					}

					delete after_preprocess;

					notfound = false;
					break;
				}

			if(notfound)
			{
				// special commands
				if(command=="exit")
					break;
				else
				{
					cout << "Warning: command '" << command << "' not found!" << endl;
					return false;
				}
			}
		}

		delete scanner;
		return true;
	}

};

#endif /* READMODULE_HPP_ */
