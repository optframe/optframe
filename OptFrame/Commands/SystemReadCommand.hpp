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

#ifndef READMODULE_HPP_
#define READMODULE_HPP_

#include <unistd.h>
#include <stdlib.h>

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class SystemReadCommand : public Command<R, ADS, DS>
{
public:

	virtual ~SystemReadCommand()
	{
	}

	string id()
	{
		return "system.read";
	}

	string usage()
	{
		return "system.read filename";
	}

	string removeComments(string line)
	{
		string ss;
		for(unsigned i=0; i<line.length(); i++)
			if(line.at(i)!='%')
				ss += line.at(i);
			else
				break;

		return ss;
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string,string>& dictionary, map< string,vector<string> >& ldictionary, string input)
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

		// add a comment in every line end!
		// have to do this to avoid Scanner jumping empty lines and return a wrong 'numLine' later

		string allFile = scanner->rest();
		string newFile = "";
		for(unsigned i=0; i<allFile.length(); i++)
		{
			if(allFile[i]=='\n')
				newFile += '%';
			newFile += allFile[i];
		}

		delete scanner;
		scanner = new Scanner(newFile);

		vector<pair<int, string> > commands;

		// add all lines

		int numLine = 0;
		while(scanner->hasNext())
		{
			string line = Scanner::trim(removeComments(scanner->nextLine()));
			numLine++;
			//cout << "FILE: '" << input << "' line " << numLine << " is '" << line << "'" << endl;
			if(line.length()==0)
				continue;
			commands.push_back(make_pair(numLine,line));
		}

		// merge lines with multiline blocks or lists

		for(int i=0; i<(int)(commands.size()); i++)
			if((commands[i].second.at(0)=='{') || (commands[i].second.at(0)=='['))
			{
				if(i==0)
				{
					cout << "read module on file '" << input << "': error! block or list in the first line!" << endl;
					return false;
				}
				else
				{
					commands[i-1].second.append(" ");
					commands[i-1].second.append(commands[i].second);
					commands[i].second = "";
				}
			}

		// join in a string to proceed as usual

		string ss;
		for(unsigned i=0; i<commands.size(); i++)
			if(commands[i].second != "")
			{
				stringstream convNum;
				convNum << commands[i].first;
				ss.append(convNum.str()); // include line number in the begin!
				ss.append(" ");
				ss.append(commands[i].second);
				ss.append("\n");
			}

		delete scanner;

		scanner = new Scanner(ss);
		commands.clear();

		// proceed as usual

		while(scanner->hasNext())
		{
			string line = Scanner::trim(removeComments(scanner->nextLine()));

			Scanner scanNum(line);
			int numLine = scanNum.nextInt();

			if(!scanNum.hasNext()) //(line.length()==0)
				continue;

			line = Scanner::trim(scanNum.rest());

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
				string line2 = Scanner::trim(removeComments(scanner->nextLine()));

				Scanner scanNum2(line2);
				//int numLine2 = scanNum2.nextInt();
				scanNum2.nextInt(); // drop line number

				line2 = Scanner::trim(scanNum2.rest());

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
				commands.push_back(make_pair(numLine, line));
			else
			{
				if(lists > 0)
					cout << "read error on line " << numLine << " of file '" << input << "': wrong number of '[' and ']' => " << lists  << " '[' left open!" << endl;
				if(blocks > 0)
					cout << "read error on line " << numLine << " of file '" << input << "': wrong number of '{' and '}' => " << blocks << " '{' left open!" << endl;

				delete scanner;
				return false;
			}
		}

		// =====================
		// WILL CHANGE DIRECTORY
		// =====================

	    char *path=nullptr;
	    size_t size = 0;
	    path=getcwd(path,size);
	    //cout<<"current Path"<<path << endl;

		// changing current directory

		const char *symlinkpath = input.c_str();
		char actualpath [PATH_MAX+1];
		char *ptr;

		ptr = realpath(symlinkpath, actualpath);
		string fullpath(ptr);

		//cout << "Target filename is: '" << ptr << "'" << endl;

		size_t found;
		found=fullpath.find_last_of("/\\");
		string dir = fullpath.substr(0,found);
		//cout << " folder: " << fullpath.substr(0,found) << endl;
		//cout << " file: " << fullpath.substr(found+1) << endl;

		if (chdir(dir.substr(0, found).c_str()) != 0)
		{
			cout << "module " << id() << " error: failed to change directory!" << endl;
			return false;
		}

		// ==================
		// WILL EXEC COMMANDS
		// ==================

		for(unsigned int i = 0; i < commands.size(); i++)
		{
			string line = commands[i].second;
			int numLine = commands[i].first;

			Scanner s2(line);
			s2.useSeparators(" \t\r\n;="); // prevent errors with semicolon

			if(!s2.hasNext()) // no command found in the line
			{
				cout << "read module on line " << numLine << " of file '" << input << "': strange error! Empty command with line '" << line << "'" << endl;
				return false;
			}

			string command = s2.next();

			bool notfound = true;

			string original = s2.rest();

			for(unsigned int i=0;i<all_modules.size();i++)
				if(all_modules[i]->canHandle(command, original))
				{
					//cout << "READ COMMAND: '" << command << "'" << endl;

					string* after_preprocess = all_modules[i]->preprocess(allFunctions, factory, dictionary, ldictionary, original);

					if(!after_preprocess)
					{
						cout << "read module on line " << numLine << " of file '" << input << "': preprocessing error!" << endl;
						return false;
					}

					//cout << "READ COMMAND INPUT: '" << after_preprocess << "'" << endl;

					if(!all_modules[i]->run(all_modules, allFunctions, factory, dictionary, ldictionary, *after_preprocess, command))
					{
						delete after_preprocess;
						cout << "read module on line " << numLine << " of file '" << input << "': error in module '" << command << "'" << endl;
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
					cout << "read module error on line " << numLine << " of file '" << input << "': command '" << command << "' not found!" << endl;
					return false;
				}
			}
		}

		delete scanner;

		// ===============================
		// GOING BACK TO CURRENT DIRECTORY
		// ===============================

		if (chdir(path) != 0)
		{
			cout << "module " << id() << " error: failed to go back to old directory!" << endl;
			return false;
		}

        free(path);

		return true;
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* READMODULE_HPP_ */
