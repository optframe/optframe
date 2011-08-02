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

template<class R, class M>
class ExportModule: public OptFrameModule<R, M>
{
public:
	string id()
	{
		return "export";
	}

	string usage()
	{
		string out = "\texport initsol id output_filename\n";
		out += "\texport loadsol id output_filename\nWHERE:\n";
		out += "\tid can assume two different type values:\n";
		out += "\t\t1 - an specific solution id,\n";
		// out += "\t\t2 - a set of solutions id's placed between the left ([) and right (]) brackets,\n"; // TODO
		out	+= "\t\t2 - the special character (*) meaning that all solutions will be saved.\n";

		return out;
	}

	void run(vector<OptFrameModule<R, M>*>& all_modules,
			HeuristicFactory<R, M>* factory, map<string, string>* dictionary,
			string input)
	{
		//cout << "export: " << input << endl;
		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string sol = scanner.next();

		if (sol != "initsol" && sol != "loadsol")
		{
			cout << "First parameter must be either 'initsol' or 'loadsol'!"
					<< endl;
			cout << "Usage: " << usage() << endl;
			return;
		}

		string id = scanner.next();

		string filename = scanner.next();

		FILE * pFile;

		pFile = fopen(filename.c_str(), "w");

		if (id == "*") // Saving all solutions
		{
			if (sol == "initsol")
			{
				for (int i = 0; i < (factory->initsol_size()); i++)
				{
					stringstream i_temp;
					i_temp << i;

					Scanner s2((sol + " " + i_temp.str()));

					Solution<R>* s =
							&(factory->read_initsol(&s2))->generateSolution();

					stringstream stream;

					stream << s->getR() << endl;

					fprintf(pFile, "%s", (stream.str()).c_str());
				}
			}
			else
			{
				for (int i = 0; i < (factory->loadsol_size()); i++)
				{
					stringstream i_temp;
					i_temp << i;

					Scanner s2((sol + " " + i_temp.str()));

					Solution<R>* s = factory->read_loadsol(&s2);

					stringstream stream;

					stream << s->getR() << endl;

					fprintf(pFile, "%s", (stream.str()).c_str());
				}
			}
		}
		/*
		else if (id == "[") // TODO
		{
			bool right_bracket = false;

			vector<string> solutions;

			while (scanner.hasNext())
			{
				string temp = scanner.next();

				if (temp == "]")
				{
					right_bracket = true;
					break;
				}
				else
				{
					solutions.push_back(scanner.next());
				}
			}

			if (right_bracket)
			{
				if (sol == "initsol")
				{
					for (int i = 0; i < (solutions.size()); i++)
					{
						Scanner s2((sol + " " + solutions[i]));

						Solution<R>
								* s =
										&(factory->read_initsol(&s2))->generateSolution();

						stringstream stream;

						stream << s->getR() << endl;

						fprintf(pFile, "%s", (stream.str()).c_str());
					}
				}
				else
				{
					for (int i = 0; i < (solutions.size()); i++)
					{
						Scanner s2((sol + " " + solutions[i]));

						Solution<R>* s = factory->read_loadsol(&s2);

						stringstream stream;

						stream << s->getR() << endl;

						fprintf(pFile, "%s", (stream.str()).c_str());
					}
				}
			}
			else
			{
				cerr << "Right Bracket \"]\" not found." << endl;
				//exit(1);
				return;
			}
		}
		*/
		else // Saving only solution
		{
			Scanner s2(sol + " " + id);

			Solution<R>* s;

			if (sol == "initsol")
				s = &(factory->read_initsol(&s2))->generateSolution();
			else
				s = factory->read_loadsol(&s2);

			stringstream stream;

			stream << s->getR() << endl;

			fprintf(pFile, "%s", (stream.str()).c_str());

		}

		fclose(pFile);
	}
};

#endif /* EXPORT_HPP_ */
