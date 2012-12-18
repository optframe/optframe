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

#ifndef ECHO_TO_FILE_MODULE_HPP_
#define ECHO_TO_FILE_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class FileEchoModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~FileEchoModule()
	{
	}

	string id()
	{
		return "file.echo";
	}

	string usage()
	{
		return "file.echo filename text";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>&, vector<OptFrameFunction*>&, HeuristicFactory<R, ADS, M>&, map<string, string>&,  map< string,vector<string> >&, string input)
	{
		Scanner scanner(input);
		if(!scanner.hasNext()) // no file
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string filename = scanner.next();

		FILE* file = fopen(filename.c_str(), "a");

		if(!file)
		{
			cout << "file.echo module: couldn't open file: '" << filename << "'" << endl;
			return false;
		}

		string text = scanner.nextLine();

		fprintf(file, "%s\r\n", text.c_str());

		fclose(file);

		return true;
	}

};

#endif /* ECHO_TO_FILE_MODULE_HPP_ */
