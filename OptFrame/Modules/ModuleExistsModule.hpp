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

#ifndef OPTFRAME_MODULE_EXISTS_MODULE_HPP_
#define OPTFRAME_MODULE_EXISTS_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ModuleExistsModule: public OptFrameModule<R, ADS, M>
{
public:

   virtual ~ModuleExistsModule()
   {
   }

   string id()
   {
      return "module.exists";
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

   OptFrameModule<R, ADS, M>* getModule(vector<OptFrameModule<R, ADS, M>*>& modules, string module)
   {
	   for (unsigned int i = 0; i < modules.size(); i++)
		   if (module == modules[i]->id())
			   return modules[i];
	   return NULL;
   }

   bool run(vector<OptFrameModule<R, ADS, M>*>& allModules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
   {
      //cout << "module.exists module: " << input << endl;

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

      OptFrameModule<R, ADS, M>* m = getModule(allModules, module_name);

      //if(m)
      //   cout << "module.exists real id is: " << m->id() << endl;

      string result = formatBool(m != NULL);

      //cout << "module.exists result is: " << result << endl;

      return OptFrameModule<R, ADS, M>::defineText(variable, result, dictionary);
   }

};

#endif /* OPTFRAME_MODULE_EXISTS_MODULE_HPP_ */
