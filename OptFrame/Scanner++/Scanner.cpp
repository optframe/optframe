// Scanner Class for C++ - Scanner++

// Copyright (C) 2009-2017
// scannerpp.sourceforge.net
//
// This file is part of the Scanner++ Library v0.98.  This library is free
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

#include "Scanner.h"

using namespace scannerpp;

Scanner::Scanner(File* inputfile)
{
	isString = false;
	this->inputfile = inputfile;
	this->input = inputfile->file;
	useDefaultSeparators();
}

Scanner::Scanner(istream* input)
{
	isString = false;
	this->inputfile = nullptr;
	this->input = input;
	useDefaultSeparators();
}

Scanner::Scanner(string input)
{
	isString = true;
	this->inputfile = nullptr;
	this->contentString = input;
	this->input = new istringstream(input);
	useDefaultSeparators();
}

Scanner::Scanner(const Scanner& scanner)
{
	contentString = scanner.contentString;
	isString = scanner.isString;
	discarded = scanner.discarded;

	if (scanner.inputfile) //for files
	{
		inputfile = new File(scanner.inputfile->filename);
		input = inputfile->file;
	}

	if (isString) // for string
		input = new istringstream(contentString);

	if (!isString) // for cin
		input = scanner.input;

	useDefaultSeparators();
}

Scanner::~Scanner()
{
	if (inputfile)
	{
		delete inputfile;
		inputfile = nullptr;
		input = nullptr;
	}

	if(input && isString)
	{
		delete input;
	}
}

Scanner& Scanner::operator=(const Scanner& scanner)
{
	if (&scanner == this) // auto ref check
		return *this;

	// ==========
	// destructor
	// ==========

	if (inputfile)
	{
		delete inputfile;
		inputfile = nullptr;
		input = nullptr;
	}

	if(input && isString)
	{
		delete input;
	}
	// ==========

	contentString = scanner.contentString;
	isString = scanner.isString;
	discarded = scanner.discarded;

	if (scanner.inputfile) //for files
	{
		inputfile = new File(scanner.inputfile->filename);
		input = inputfile->file;
	}

	if (isString) // for string
		input = new istringstream(contentString);

	if (!isString) // for cin
		input = scanner.input;

	useDefaultSeparators();

	return *this;
}

void Scanner::useDefaultSeparators()
{
	useSeparators(string("\n\r\t "));
}

void Scanner::useSeparators(string s)
{
	sep = s;
}

bool Scanner::inSeparators(char c) const
{
	for (unsigned int i = 0; i < sep.length(); i++)
		if (sep[i] == c)
			return true;
	return false;
}

// =================================================================
// =================================================================

bool Scanner::hasNextChar() const
{
	if (input->eof())
		return false;

	int x = input->peek();

	if(input->fail())
	{
		cout << "WARNING::SCANNER FAILED!" << endl;
	}

	if (x > 0)
		return true;

	if (x == 0)
		return false;

	return false;
}

char Scanner::nextChar()
{
	int x = input->get();

	if (x <= 0)
		throw ConversionError("char");

	return x;
}


bool Scanner::nextCharIs(char c) const
{
   stringstream ss;
   ss << c;
   string s = ss.str();

   return nextCharIn(s);
}

bool Scanner::nextCharIn(string s) const
{
   if(!hasNextChar())
      return false;

   bool r = false;

   int x = input->get();

   if (x > 0)
   {
      char c = x;

      for(unsigned i=0; i<s.length(); i++)
         if(c == s.at(i))
         {
            r = true;
            break;
         }

   }

   input->putback((char) x);

   return r;
}

void Scanner::trimInput()
{
   string s = " \t\n";

   if(!hasNextChar())
      return;

   int x = input->get();

   while (x > 0)
   {
      char c = x;

      bool t = false;

      for(unsigned i=0; i<s.length(); i++)
         if(c == s.at(i))
         {
            t = true;
            break;
         }

      if(!t)
      {
         input->putback((char) x);
         return;
      }

      if(!hasNextChar())
         return;

      x = input->get();
   }
}

// =================================================================
// =================================================================

int Scanner::nextInt()
{
	int x;
	istringstream myStream(next());
	if (myStream >> x)
		return x;
	else
		throw ConversionError("int");
}

long Scanner::nextLong()
{
	long x;
	istringstream myStream(next());
	if (myStream >> x)
		return x;
	else
		throw ConversionError("long");
}

float Scanner::nextFloat()
{
	float x;
	istringstream myStream(next());
	if (myStream >> x)
		return x;
	else
		throw ConversionError("float");

}

double Scanner::nextDouble()
{
	double x;
	istringstream myStream(next());
	if (myStream >> x)
		return x;
	else
		throw ConversionError("double");

}

// =================================================================
// =================================================================

bool Scanner::hasNext() const
{
	if (!hasNextChar())
		return false;

	istream* input = const_cast<istream*>(this->input);

	vector<char> buffer;

	char novo = nextChar(*input);

	bool next = true;

	while (inSeparators(novo))
	{
		buffer.push_back(novo);

		if (!(hasNextChar()))
		{
			next = false;
			break;
		}

		novo = nextChar(*input);
	}

	if (next) // ha proximo!
		input->putback(novo);

	//devolver o buffer ao IO
	while (buffer.size() > 0)
	{
		input->putback(buffer.at(buffer.size() - 1));
		buffer.erase(buffer.begin() + (buffer.size() - 1));
	}

	return next;
}

std::string Scanner::next()
{
	discarded = "";

	std::string x = "";

	while (hasNextChar())
	{
		char c = nextChar();

		if (!inSeparators(c))
		{
			x = x + c;
			break;
		}
		else
			discarded += c;
	}

	while (hasNextChar())
	{
		char c = nextChar();

		if (inSeparators(c))
		{
			input->putback(c);
			break;
		}

		x = x + c;
	}

	return x;
}

std::string Scanner::peekNext() const
{
	if(input->eof())
		return "";

	std::string discarded = "";

	std::string x = "";

	istream* input = const_cast<istream*>(this->input);

	while (hasNextChar())
	{
		char c = nextChar(*input);

		if (!inSeparators(c))
		{
			x = x + c;
			break;
		}
		else
			discarded += c;
	}

	while (hasNextChar())
	{
		char c = nextChar(*input);

		if (inSeparators(c))
		{
			input->putback(c);
			break;
		}

		x = x + c;
	}

	discarded.append(x);

	put_back(const_cast<istream**>(&this->input), discarded);

	return x;
}


pair<string, map<string, string> > Scanner::nextXMLTag()
{
	string x = "";

	while (hasNextChar())
	{
		char c = nextChar();

		if (c == '<')
		{
			x = x + c;
			break;
		}
	}

	while (hasNextChar())
	{
		char c = nextChar();
		x = x + c;

		if (c == '>')
			break;
	}

	cout << "base: " << x << endl;

	if (x.size() < 2 || x.at(0) != '<' || x.at(x.size() - 1) != '>')
		return make_pair("", map<string, string> ());

	Scanner scanner(x);
	scanner.useSeparators("<>");

	string tagname = "";
	map<string, string> attr;

	if (scanner.hasNext())
	{
		string tag = scanner.next();
		//cout << "tag: " << tag << endl;

		Scanner sc_tag(tag);
		sc_tag.useSeparators(" ");

		if (sc_tag.hasNext())
			tagname = sc_tag.next();

		//cout << "tagname: " << tagname << endl;

		// TODO usar trim

		sc_tag.useSeparators(" =");
		while (sc_tag.hasNext())
		{
			string at_name = sc_tag.next();
			//cout << "at_name: " << at_name << "\t";
			sc_tag.useSeparators("=\"");

			string at_value = sc_tag.next();
			//cout << "at_value: " << at_value << "\t";

			attr[at_name] = at_value;

			sc_tag.useSeparators("\" =");
		}
	}

	return make_pair(tagname, attr);
}

std::string Scanner::nextLine()
{
	string backup_sep = sep;
	useSeparators("\n");
	string linha = next();
	useSeparators(backup_sep);

	return linha;
}

// =================================================================
// =================================================================

string Scanner::rest() // Returns the rest of the input as string
{
	string backup_sep = sep;

	useSeparators("");

	string rest = "";

	if (hasNext())
		rest = next();

	sep = backup_sep;

	return rest;
}
