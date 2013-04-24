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

#ifndef EXPORT_HPP_
#define EXPORT_HPP_

#include "../OptFrameModule.hpp"

#include <stdio.h>

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentExportModule: public OptFrameModule<R, ADS, DS>
{
public:

	virtual ~ComponentExportModule()
	{
	}

	string id()
	{
		return "component.export";
	}

	string usage()
	{
		string out = "component.export initsol id output_filename\n";
		out += "\texport loadsol id output_filename\nWHERE:\n";
		out += "\tid can assume two different type values:\n";
		out += "\t\t1 - an specific solution id,\n";
		// out += "\t\t2 - a set of solutions id's placed between the left ([) and right (]) brackets,\n"; // TODO
		out	+= "\t\t2 - the special character (*) meaning that all solutions will be saved.\n";

		return out;
	}

	bool run(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "export: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string sol = scanner.next();

		if (sol != Constructive<R,ADS>::idComponent() && sol != Solution<R,ADS>::idComponent())
		{
			cout << "First parameter must be either 'initsol' or 'loadsol'!"
					<< endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string id = scanner.next();

		string filename = scanner.next();

		FILE * pFile;

		pFile = fopen(filename.c_str(), "a");

		if (id == "*") // Saving all solutions
		{
			if (sol == Constructive<R,ADS>::idComponent())
			{
				//for (int i = 0; i < (factory->initsol_size()); i++)
				for (int i = 0; i < 0; i++) // TODO fix this!
				{
					stringstream i_temp;
					i_temp << i;

					Scanner s2((sol + " " + i_temp.str()));

					Constructive<R, ADS>* initsol = NULL;
					factory.readComponent(initsol, s2);

					Solution<R, ADS>* s = &(initsol->generateSolution());

					stringstream stream;

					stream << s->getR() << endl;

					fprintf(pFile, "%s", (stream.str()).c_str());
				}
			}
			else
			{
				//for (int i = 0; i < (factory->loadsol_size()); i++)
				for (int i = 0; i < 0; i++) // TODO fix this!
				{
					stringstream i_temp;
					i_temp << i;

					Scanner s2((sol + " " + i_temp.str()));

					Solution<R, ADS>* s = NULL;
					factory.readComponent(s, s2);

					stringstream stream;

					stream << s->getR() << endl;

					fprintf(pFile, "%s", (stream.str()).c_str());
				}
			}
		}
		else // Saving only solution
		{
			Scanner s2(sol + " " + id);

			Solution<R, ADS>* s = NULL;

			if (sol == Constructive<R,ADS>::idComponent())
			{
				Constructive<R, ADS>* initsol = NULL;
				factory.readComponent(initsol, s2);

				s = &(initsol->generateSolution());
			}
			else
			{
				factory.readComponent(s, s2);
			}

			stringstream stream;

			stream << s->getR() << endl;

			fprintf(pFile, "%s", (stream.str()).c_str());
		}

		fclose(pFile);

		return true;
	}
};

#endif /* EXPORT_HPP_ */
