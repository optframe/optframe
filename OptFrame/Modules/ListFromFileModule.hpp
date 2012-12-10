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

#ifndef LISTFROMFILEMODULE_HPP_
#define LISTFROMFILEMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ListFromFileModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ListFromFileModule()
	{
	}

	string id()
	{
		return "list_from_file";
	}
	string usage()
	{
		return "list_from_file new_list_name filename";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
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

		return OptFrameModule<R, ADS, M>::run_module("silent_define_list", all_modules, allFunctions, factory, dictionary, ldictionary, listContent.str());
	}

};

#endif /* LISTFROMFILEMODULE_HPP_ */
