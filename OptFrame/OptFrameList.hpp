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

	static vector<std::string>* readList(Scanner& scanner)
	{
		vector < std::string > *list = new vector<std::string> ;
		std::string word;
		char character = scanner.nextChar();
		int numberOfBrackets;

		while (character == ' ')
		{
			character = scanner.nextChar();
		}

		if(character != '[')
		{
			if(( character >= '0' ) && ( character <= '9' )) // syntax 1..n
			{
				std::string number = "";
				number += character;
				character = scanner.nextChar();

				while( ( ( character >= '0' ) && ( character <= '9' ) ) )
				{
					number += character;
					character = scanner.nextChar();
				}

				Scanner toInt(number);
				int firstInt = toInt.nextInt();

				int dots = 0;

				while(( character == ' ' ) || ( character == '.' ))
				{
					if(character == '.')
						dots++;

					character = scanner.nextChar();
				}

				if(dots != 2)
				{
					cout << "Error: expected two dots (..) and found " << dots << " dots!" << endl;
					return NULL;
				}

				stringstream rest;
				rest << character << scanner.rest();
				scanner = Scanner(rest.str());

				int secondInt = scanner.nextInt();

				if(firstInt < secondInt)
				{
					for(int i = firstInt; i <= secondInt; i++)
					{
						stringstream toStr;
						toStr << i;
						list->push_back(toStr.str());
					}
				}
				else
					for(int i = firstInt; i >= secondInt; i--)
					{
						stringstream toStr;
						toStr << i;
						list->push_back(toStr.str());
					}

				return list;
			}
			else
			{
				cout << "Error:! expected '[' and found '" << character << "'!" << endl;
				return NULL;
			}
		}


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

};

#endif /* OPTFRAME_LIST_HPP_ */
