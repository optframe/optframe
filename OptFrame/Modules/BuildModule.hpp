// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
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

#ifndef BUILDMODULE_HPP_
#define BUILDMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class M>
class BuildModule: public OptFrameModule<R, M>
{
public:
   string id()
   {
      return "build";
   }
   string usage()
   {
      string u = "build method [my_method_name]\n";
      u += "  eg.\n";
      u += "\tbuild BI ev id ns id my_Best_Improvement\n";
      u += "\tbuild FI ev id ns id my_First_Improvement\n";
      u += "\tbuild HC ev id BI ev id ns id my_HillClimbing_BI\n";
      u += "\tbuild HC ev id my_First_Improvement my_HillClimbing_FI\n";
      u += "\tbuild GA ev id initpop id <crossover rate> <mutation rate> <population size> <number of generations> ga_sel id ga_cross id ga_mut id my_GA\n";

      return u;
   }

   void run(vector<OptFrameModule<R, M>*>& all_modules, HeuristicFactory<R, M>* factory, map<string, string>* dictionary, string input)
   {
      cout << "build: " << input << endl;
      Scanner scanner(input);

      if (!scanner.hasNext())
      {
         cout << "Usage: " << usage() << endl;
         return;
      }

      pair<Heuristic<R, M>*, string> method = factory->createHeuristic(scanner.rest());

      scanner = Scanner(method.second);

      int new_id = factory->add_method(method.first);

      stringstream str;
      str << "method " << new_id;
      string s_new_id = str.str();

      cout << "'" << s_new_id << "' added." << endl;

      if (scanner.hasNext())
      {
         string new_name = scanner.next();
         run_module("define", all_modules, factory, dictionary, new_name + " " + s_new_id);
      }

   }

};

#endif /* BUILDMODULE_HPP_ */
