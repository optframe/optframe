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

#ifndef OPTFRAME_DROP_MODULE_HPP_
#define OPTFRAME_DROP_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class ComponentDropModule: public OptFrameModule<R, ADS, M>
{
public:

   virtual ~ComponentDropModule()
   {
   }

   string id()
   {
      return "component.drop";
   }

   string usage()
   {
      string u = id();
      u += "component id\n";
      return u;
   }

   bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, map< string,vector<string> >& ldictionary, string input)
   {
      //cout << "component.drop module: " << input << endl;
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

      return factory.drop(type, id);
   }

};

#endif /* OPTFRAME_DROP_MODULE_HPP_ */
