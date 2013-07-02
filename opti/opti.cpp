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

// ====================================================
// Building OptFrame Script Language Interpreter - opti
// ====================================================

#include<vector>

#include "../OptFrame/Interpreter.hpp"

using namespace std;
using namespace optframe;

int main(int argc, char **argv)
{
	// parameters: -h, --help
	bool help = false;

	// parameters: -v, --verbose
	bool verbose = false;

	// parameters: -s, --silent
	bool silent = false;

	// parameters: -k, --enable-system-calls
	bool system_calls = false;

	// parameter: filename
	string filename = "";

	// get parameters
	vector<string> params;
	for (unsigned i = 1; i < argc; i++)
		params.push_back(argv[i]);

	// process parameters
	for (unsigned i = 0; i < params.size(); i++)
	{
		if ((params[i] == "-h") || (params[i] == "--help"))
			help = true;
		else if ((params[i] == "-v") || (params[i] == "--verbose"))
			verbose = true;
		else if ((params[i] == "-s") || (params[i] == "--silent"))
			silent = true;
		else if ((params[i] == "-k") || (params[i] == "--enable-system-calls"))
			system_calls = true;
		else
			filename = params[i];
	}

	// ==============
	// execute 'opti'
	// ==============

	typedef int RepGeneral;
	Interpreter<RepGeneral> optframe; // do not load 'component' prefix

	if (verbose)
	{
		cout << optframe.version() << endl;
		cout << "verbose mode enabled (but not fully supported yet...)" << endl;
	}

	optframe.unloadCommands();
	optframe.unloadFunctions();

	optframe.loadDefault("statements");
	optframe.loadDefault("deprecated"); // TODO: remove!
	optframe.loadDefault("system");
	optframe.loadDefault("file");
	optframe.loadDefault("list");
	optframe.loadDefault("command");
	optframe.loadDefault("function");
	optframe.loadDefault("plot");
	optframe.loadDefault("randgen");
	optframe.loadDefault("statistics");
	optframe.loadDefault("text");
	optframe.loadDefault("operator");
	optframe.loadDefault("math");

	if(system_calls)
		optframe.loadCallCommand();

	if (help)
	{
		cout << optframe.version() << endl;
		cout << "parameters are: opti [-h, --help] [-v, --verbose] [-s, --silent] [-k, --enable-system-calls] [filename]" << endl;
		cout << "opti: displaying help" << endl;
		optframe.execute("system.help");
	}
	else if (filename == "")
	{
		if (verbose)
			cout << "verbose: opening interactive mode (type 'system.help' for help)" << endl;

		optframe.execute();
	}
	else
	{
		if (verbose)
			cout << "verbose: reading file '" << filename << "'" << endl;

		optframe.execute("system.read " + filename);
	}

	return 0;
}
