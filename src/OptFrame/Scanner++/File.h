// Scanner Object for C++ - Scanner++

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

#ifndef SCANNERPP_FILE_H_
#define SCANNERPP_FILE_H_

#include<fstream>
#include<string>

using namespace std;
#include<iostream>

namespace scannerpp
{

class FileNotFound
{
private:
	string file;
public:
	FileNotFound(string str) :
		file(str)
	{
	}
	string getFile()
	{
		return file;
	}
};

class File
{
public:
	std::ifstream* file;
	string filename;

	File(string _filename)
	{
		filename = _filename;

		// ====================
		// check if file exists
		// ====================

		std::fstream foo;

		foo.open(filename.c_str());

		if (foo.is_open())
			foo.close();
		else
			throw FileNotFound(filename);
		// ===============================

		file = new std::ifstream(filename.c_str(), std::ifstream::in);
	}

	virtual ~File()
	{
		close();
		delete file;
	}

	int get()
	{
		return file->get();
	}

	bool eof()
	{
		return file->eof();
	}

	void close()
	{
		file->close();
	}
};

} // end namespace scannerpp

#endif /*SCANNERPP_FILE_H_*/
