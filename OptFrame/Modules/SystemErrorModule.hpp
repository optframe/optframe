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

#ifndef ERROR_MODULE_HPP_
#define ERROR_MODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class SystemErrorModule: public OptFrameModule<R, ADS, M>
{
public:

   virtual ~SystemErrorModule()
   {

   }

   string id()
   {
      return "system.error";
   }

   string usage()
   {
      return "system.error message";
   }
   bool run(vector<OptFrameModule<R, ADS, M>*>&, vector<OptFrameFunction*>&, HeuristicFactory<R, ADS, M>&, map<string, string>&, map< string,vector<string> >&, string input)
   {
      Scanner scanner(input);

      if (!scanner.hasNext())
      {
         return false;
      }
      else
      {
    	  cerr << scanner.nextLine() << endl;
    	  return false;
      }
   }

};

#endif /* ERROR_MODULE_HPP_ */
