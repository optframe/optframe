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

#ifndef OPTFRAME_COMPONENT_NULLPTR_MODULE_HPP_
#define OPTFRAME_COMPONENT_NULLPTR_MODULE_HPP_

#include "../Command.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentNullCommand: public Command<R, ADS, DS>
{
public:

   virtual ~ComponentNullCommand()
   {
   }

   string id()
   {
      return "component.null";
   }

   string usage()
   {
      string u = id();
      u += "component id result_variable\n";
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

   bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
   {
      //cout << "component.null command: " << input << endl;

      Scanner scanner(input);

      if (!scanner.hasNext())
      {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      string type = scanner.next();

      if (!scanner.hasNext())
      {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      int id = scanner.nextInt();

      if (!scanner.hasNext())
      {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      string variable = scanner.next();

      if(factory.components.count(type) > 0)
      {
    	  vector<Component*> v = factory.components[type];

    	  if(id < ((int)v.size()))
    	  {
    		  string result = formatBool(v[id]==nullptr);

    		  return Command<R, ADS, DS>::defineText(variable,result,dictionary);
    	  }
      }

      cout << "component.null command: component '" << type << " " << id << "' not found!" << endl;

      return false;
   }

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}

};

}

#endif /* OPTFRAME_COMPONENT_NULLPTR_MODULE_HPP_ */
