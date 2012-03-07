// Scanner Object - Scanner++

// Copyright (C) 2009, 2010, 2011, 2012
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
//
// This file is part of the Scanner++ Library v0.94.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef SCANNERPP_H_
#define SCANNERPP_H_

#include<iostream>
#include<istream>
#include<string>
#include<sstream>
#include<algorithm>

#include<vector>
#include<map>

#include <stdlib.h>

using namespace std;

#include "File.h"

class Scanner
{
private:
	istream *input;
	File* inputfile;
	string sep;
	bool isString;

	string discarded;

	string contentString;

public:

	string getDiscarded()
	{
		return discarded;
	}

	bool hasNextChar();
	char nextChar();

	Scanner(File* inputfile);
	Scanner(istream* input);
	Scanner(string input);

	Scanner(const Scanner& scanner);

	virtual ~Scanner();

	virtual Scanner& operator=(const Scanner& scanner);

	// useDefaultSeparators: chama o useSeparators para os caracteres:
	// espaco, quebra de linha (\n), tabulacao (\t) e retorno de carro (\r)

	void useDefaultSeparators();

	// useSeparators: equivalente ao useDelimiter de Java
	// a diferenca e que Java trata a string como uma
	// expressao regular, e neste caso o useSeparators
	// apenas considera cada caractere da string separadamente
	// como um separador.

	void useSeparators(string s);
	bool inSeparators(char c);

	std::string next();
	std::string nextLine();
	int nextInt();
	long nextLong();
	float nextFloat();
	double nextDouble();
	//static string trim(string);
	static string trim(string word)
	{
		int i = 0;
		char c = word[i];
		string aux_word = "";

		//removing initial spaces
		while (c == ' ' && i < ((int) word.length()))
		{
			i++;
			c = word[i];
		}

		while (i < ((int) word.length()))
		{
			aux_word += word[i];
			i++;
		}

		word = aux_word;
		//removing final spaces
		i = word.length() - 1;
		c = word[i];
		aux_word = "";
		while (c == ' ' && i >= 0)
		{
			i--;
			c = word[i];
		}

		while (i >= 0)
		{
			aux_word += word[i];
			i--;
		}

		word = aux_word;

		//reverting the word
		reverse(word.begin(), word.end());

		return word;
	}

	pair<string, map<string, string> > nextXMLTag();

	bool hasNext();
	bool hasNextLine(); // Unimplemented! Calling hasNext() instead
	bool hasNextInt()
	{
		return true;
	}
	bool hasNextLong()
	{
		return true;
	}
	bool hasNextFloat()
	{
		return true;
	}
	bool hasNextDouble()
	{
		return true;
	}

	string rest(); // Returns the rest of the input as string
};

#endif /*SCANNERPP_H_*/
