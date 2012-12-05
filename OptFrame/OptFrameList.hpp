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

#ifndef OPTFRAME_LIST_HPP_
#define OPTFRAME_LIST_HPP_

#include <vector>
#include <iostream>
#include <string>

#include "./Scanner++/Scanner.h"

using namespace std;

class OptFrameList
{
public:

	static vector<std::string>* readList(map< string,vector<string> >& ldictionary, Scanner& scanner)
	{
		char character = scanner.nextChar();
		int numberOfBrackets;

		while (character == ' ')
		{
			character = scanner.nextChar();
		}

		if(character != '[') // read from dictionary
		{
			stringstream ssword;
			while(character != ' ')
			{
				ssword << character;
				character = scanner.nextChar();
			}

			string word = ssword.str();

			if(ldictionary.count(word) == 0)
				return NULL; // not registered
			else
			{
				vector<string>* list = new vector<string>(ldictionary.find(word)->second);
				return list;
			}
		}

		vector < std::string > *list = new vector<std::string> ;
		std::string word;

		numberOfBrackets = 0;

		character = scanner.nextChar();

		word = "";
		while ((character != ']') || ((character == ']') && numberOfBrackets > 0))
		{
			if (character == '[')
				numberOfBrackets++;
			if (character == ']')
				numberOfBrackets--;

			if ((character == ',') && (numberOfBrackets == 0))
			{
				list->push_back(word);
				word = "";
			}
			else
			{
				word += character;
			}

			character = scanner.nextChar();
		}
		list->push_back(word);

		for (unsigned int i = 0; i < list->size(); i++)
		{
			list->at(i) = scanner.trim(list->at(i));
		}

		// error in code, dont know why but list should be empty!
		if((list->size()==1) && (list->at(0)==""))
			list->pop_back();

		return list;
	}

	static string listToString(const vector<string>& list)
	{
		stringstream ss;

		ss << "[";
		for(unsigned i=0; i<list.size(); i++)
		{
			ss << list.at(i);
			if(i != list.size()-1)
				ss << ",";
		}
		ss << "]";

		return ss.str();
	}

};

#endif /* OPTFRAME_LIST_HPP_ */
