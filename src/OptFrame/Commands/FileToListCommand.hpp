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

#ifndef LISTFROMFILEMODULE_HPP_
#define LISTFROMFILEMODULE_HPP_

#include "../Command.hpp"

#include "ListSilentDefineCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class FileToListCommand: public Command<R, ADS, DS>
{
public:

	virtual ~FileToListCommand()
	{
	}

	string id()
	{
		return "file.to_list";
	}
	string usage()
	{
		return "file.to_list new_list_name filename";
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		Scanner scan(input);
		if (!scan.hasNext()) // no file
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string listName = scan.next();

		if (!scan.hasNext()) // no file
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		// Open file
		Scanner* scanner;

		try
		{
			scanner = new Scanner(new File(scan.trim(scan.rest())));
		}
		catch (FileNotFound& e)
		{
			cout << "File '" << e.getFile() << "' not found!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector < string > elements;

		while (scanner->hasNextLine())
		{
			string line = scanner->nextLine();

			// WHY?
			/*
			for (unsigned int c = 0; c < line.size(); c++)
				if ((line.at(c) == ',') || (line.at(c) == '[') || (line.at(c) == ']'))
					line[c] = '?';
			*/

			elements.push_back(line);
		}

		delete scanner;

		stringstream listContent;
		listContent << listName << " " << OptFrameList::listToString(elements);

		// TODO: should register directly (for performance)!

		return Command<R, ADS, DS>::run_module("list.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, listContent.str());
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* LISTFROMFILEMODULE_HPP_ */
