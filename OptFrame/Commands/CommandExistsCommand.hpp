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

#ifndef OPTFRAME_MODULE_EXISTS_MODULE_HPP_
#define OPTFRAME_MODULE_EXISTS_MODULE_HPP_

#include "../Command.hpp"


namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class CommandExistsCommand: public Command<R, ADS, DS>
{
public:

   virtual ~CommandExistsCommand()
   {
   }

   string id()
   {
      return "command.exists";
   }

   string usage()
   {
      string u = id();
      u += " module_name result_variable\n";
      return u;
   }

   string formatBool(bool b)
   {
	   if(b)
		   return "true";
	   else
		   return "false";
   }

   bool parseBool(string b)
   {
	   return b == "true";
   }

	Command<R, ADS, DS>* getCommand(vector<Command<R, ADS, DS>*>& modules, string module, string rest)
	{
		for (unsigned int i = 0; i < modules.size(); i++)
		{
			//cout << "run: testing module '" << modules[i]->id() << "'" << endl;
			if (modules[i]->canHandle(module, rest))
				return modules[i];
		}
		//cout << "run: nullptr MODULE! module='" << module << "' rest='" << rest << "'" << endl;
		return nullptr;
	}

   bool run(vector<Command<R, ADS, DS>*>& allCommands, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
   {
      //cout << "command.exists command: " << input << endl;

      Scanner scanner(input);

      if (!scanner.hasNext())
      {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      string module_name = scanner.next();

      if (!scanner.hasNext())
      {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      string variable = scanner.next();

      Command<R, ADS, DS>* m = getCommand(allCommands, module_name, "");

      //if(m)
      //   cout << "command.exists real id is: " << m->id() << endl;

      string result = formatBool(m != nullptr);

      //cout << "command.exists result is: " << result << endl;

      return Command<R, ADS, DS>::defineText(variable, result, dictionary);
   }

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* OPTFRAME_MODULE_EXISTS_MODULE_HPP_ */
