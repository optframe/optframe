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

#ifndef OPTFRAME_COMPONENT_NULL_MODULE_HPP_
#define OPTFRAME_COMPONENT_NULL_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ComponentNullModule: public OptFrameModule<R, ADS, DS>
{
public:

   virtual ~ComponentNullModule()
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

   bool run(vector<OptFrameModule<R, ADS, DS>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
   {
      //cout << "component.null module: " << input << endl;

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
    	  vector<OptFrameComponent*> v = factory.components[type];

    	  if(id < ((int)v.size()))
    	  {
    		  string result = formatBool(v[id]==NULL);

    		  return OptFrameModule<R, ADS, DS>::defineText(variable,result,dictionary);
    	  }
      }

      cout << "component.null module: component '" << type << " " << id << "' not found!" << endl;

      return false;
   }

};

#endif /* OPTFRAME_COMPONENT_NULL_MODULE_HPP_ */
