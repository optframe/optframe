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

#ifndef READMODULE_HPP_
#define READMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ReadModule : public OptFrameModule<R, ADS, M>
{
public:

	virtual ~ReadModule()
	{
	}

	string id()
	{
		return "read";
	}
	string usage()
	{
		return "read filename";
	}

	bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>* factory, map<string,string>* dictionary, string input)
	{
		Scanner trim(input);
		if(!trim.hasNext()) // no file
		{
			cout << "Usage: " << usage() << endl;
			return false;
		}

		// Open file
		Scanner* scanner;

		try
		{
			scanner = new Scanner(new File(input));
		}
		catch (FileNotFound& e)
		{
			cout << "File '"<< e.getFile() << "' not found!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string> commands;

		while(scanner->hasNext())
		{
			string line = scanner->nextLine();

			if(line.length()==0)
				continue;
			if(line[0]=='%')
				continue;

         int brackets = 0;

         for(unsigned int c = 0; c < line.length(); c++)
         {
            if(line.at(c)=='[')
               brackets++;
            if(line.at(c)==']')
               brackets--;
         }

         while((brackets > 0) && scanner->hasNext())
         {
            string line2 = scanner->nextLine();

            for(unsigned int c = 0; c < line2.length(); c++)
            {
               if(line2.at(c)=='[')
                  brackets++;
               if(line2.at(c)==']')
                  brackets--;
            }

            line = line + line2;
         }

         if(brackets == 0)
            commands.push_back(line);
         else
         {
            cout << "read error: wrong number of '[' and ']'" << endl;
            delete scanner;
            return false;
         }
      }

		for(unsigned int i = 0; i < commands.size(); i++)
		{
			string line = commands[i];

			Scanner s2(line);

			if(!s2.hasNext()) // no command found in the line
				continue;

			string command = s2.next();

			bool notfound = true;

			if(command[0]=='%')
				notfound = false;

			for(unsigned int i=0;i<all_modules.size();i++)
				if(command == all_modules[i]->id())
				{
					string original = s2.rest();
					string after_preprocess = all_modules[i]->preprocess(allFunctions, dictionary, original);
					if(!all_modules[i]->run(all_modules, allFunctions, factory, dictionary, after_preprocess))
						return false;

					notfound = false;
					break;
				}

			if(notfound)
			{
				if(command=="exit")
					break;
				else
				{
					cout << "Warning: command '"<<command<<"' not found!"<<endl;
					return false;
				}
			}
		}

		delete scanner;
		return true;
	}

};

#endif /* READMODULE_HPP_ */
