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

#ifndef OPTFRAME_EXPORTLOGMODULE_HPP_
#define OPTFRAME_EXPORTLOGMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ExportLogModule: public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ExportLogModule()
	{
	}

	string id()
	{
		return "export_log";
	}
	string usage()
	{
		return "export_log method 0 filename";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& hf, map<string, string>& dictionary, string input)
	{
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string m = scanner.next();

		if (m != "method")
		{
			cout << "First parameter must be 'method'!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		int id = scanner.nextInt();

		string filename = scanner.next();

		FILE * pFile = fopen(filename.c_str(), "a");

		SingleObjSearch<R, ADS, M>* h = NULL;
		hf.assign(h, id, SingleObjSearch<R, ADS, M>::idComponent());

		stringstream stream;

		if (h) // not null
			stream << h->log() << endl;

		fprintf(pFile, "%s", (stream.str()).c_str());

		fclose(pFile);

		return true;
	}

};

#endif /* OPTFRAME_EXPORTLOGMODULE_HPP_ */
