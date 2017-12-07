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

// File Object - Scanner++

// Copyright (C) 2009
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
//
// This file is part of the Scanner++ Library v0.91.  This library is free
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

#ifndef FILE_H_
#define FILE_H_

#include<fstream>
#include<string>

using namespace std;
#include<iostream>

class FileNotFound
{
private:
	string file;	
public:
	FileNotFound(string str){file = str;};	
};

class File
{
public:
	std::ifstream* file;	

	//File(const char* filename);
	File(string filename)
	{
		std::fstream foo;

		foo.open(filename.c_str());

    	if(foo.is_open())
       		foo.close();
    	else
    	{ 
     		cerr << "File Doesn't Exist: "<<filename << endl;
     		throw FileNotFound(filename);
    	}
	
		file = new std::ifstream(filename.c_str(), std::ifstream::in);
		
	}
	
	virtual ~File() { close();}
	
	int get(){ return file->get();}
	
	bool eof(){ return file->eof();}
	
	void close(){ file->close();}
};

#endif /*FILE_H_*/
