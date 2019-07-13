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

#ifndef OPTFRAME_LIST_HPP_
#define OPTFRAME_LIST_HPP_

#include <vector>
#include <iostream>
#include <string>

#include "./Scanner++/Scanner.h"

using namespace std;
using namespace scannerpp;

class OptFrameList
{
public:

	static string intToString(int i)
	{
		stringstream ss;
		ss << i;
		return ss.str();
	}

	static vector<std::string>* readList(const map<string, vector<string> >& ldictionary, string list)
	{
		Scanner scanner(list);
		return readList(ldictionary, scanner);
	}

	static vector<std::string>* readList(const map<string, vector<string> >& ldictionary, Scanner& scanner)
	{
		scanner.trimInput();

		if (!scanner.hasNextChar())
			return nullptr;

		char character = scanner.nextChar();
		int numberOfBrackets;

		if (character != '[') // read from dictionary
		{
			stringstream ssword;
			while (character != ' ') // or... there is no next char!
			{
				ssword << character;
				if (!scanner.hasNextChar())
					break;
				character = scanner.nextChar();
			}

			string word = ssword.str();

			if (ldictionary.count(word) == 0) // will try numeric list
			{
				int begin;
				try
				{
					begin = Scanner::parseInt(word);
				}
				catch (ConversionError& e)
				{
					cout << "OptFrameList error: variable '" << word << "' not found in list dictionary!" << endl;
					return nullptr;
				}

				if (!scanner.hasNext())
					return nullptr;

				string doubledots = scanner.next();

				if (doubledots != "..")
					return nullptr;

				if (!scanner.hasNext())
					return nullptr;

				string send = scanner.next();

				int end;
				try
				{
					end = Scanner::parseInt(send);
				}
				catch (ConversionError& e)
				{
					return nullptr;
				}

				vector<string>* numeric_list = new vector<string>();

				if (begin < end)
					for (int i = begin; i <= end; i++)
						numeric_list->push_back(intToString(i));
				else
					for (int i = begin; i >= end; i--)
						numeric_list->push_back(intToString(i));

				return numeric_list;
			}
			else
			{
				vector<string>* list = new vector<string>(ldictionary.find(word)->second);
				return list;
			}
		}

		vector<std::string> *list = new vector<std::string>;
		std::string word;

		numberOfBrackets = 0;

		if (!scanner.hasNextChar())
			return nullptr;

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

			if (!scanner.hasNextChar())
				return nullptr;

			character = scanner.nextChar();
		}
		list->push_back(word);

		for (unsigned int i = 0; i < list->size(); i++)
		{
			list->at(i) = scanner.trim(list->at(i));
		}

		// TODO: error in code, dont know why but list should be empty!
		if ((list->size() == 1) && (Scanner::trim(list->at(0)) == ""))
			list->pop_back();

		return list;
	}

	static vector<std::string>* readBlock(Scanner& scanner)
	{
		if (!scanner.hasNextChar())
			return nullptr;

		char character = scanner.nextChar();
		int numberOfBrackets;

		// trim input
		while (character == ' ')
		{
			if (!scanner.hasNextChar())
				return nullptr;

			character = scanner.nextChar();
		}

		if (character != '{') // can't read block from dictionary
		{
			if (character == '[')
				cout << "OptFrameList::readBlock() error: trying to read block from a possible list structure!" << endl;
			return nullptr;
		}

		vector<string>* block = new vector<string>;
		string word;

		numberOfBrackets = 0;

		if (!scanner.hasNextChar())
			return nullptr;

		character = scanner.nextChar();

		word = "";
		while ((character != '}') || ((character == '}') && numberOfBrackets > 0))
		{
			if (character == '{')
				numberOfBrackets++;
			if (character == '}')
				numberOfBrackets--;

			if ((character == ';') && (numberOfBrackets == 0))
			{
				block->push_back(word);
				word = "";
			}
			else
			{
				word += character;
			}

			if (!scanner.hasNextChar())
				return nullptr;

			character = scanner.nextChar();
		}

		block->push_back(word);

		for (unsigned int i = 0; i < block->size(); i++)
			block->at(i) = scanner.trim(block->at(i));

		// allow last command with semicolon (optional)
		if ((block->size() > 0) && (block->at(block->size() - 1) == ""))
			block->pop_back();

		return block;
	}

	static string listToString(const vector<string>& list)
	{
		stringstream ss;

		ss << "[";
		for (unsigned i = 0; i < list.size(); i++)
		{
			ss << list.at(i);
			if (i != list.size() - 1)
				ss << ",";
		}
		ss << "]";

		return ss.str();
	}

	static string blockToString(const vector<string>& block)
	{
		stringstream ss;

		ss << "{";
		for (unsigned i = 0; i < block.size(); i++)
		{
			ss << block.at(i);
			if (i != block.size() - 1)
				ss << ";";
		}
		ss << "}";

		return ss.str();
	}

};

#endif /* OPTFRAME_LIST_HPP_ */
