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

//! \english The BuildModule class is a Module that enables the user to create a heuristic and give a name to it. \endenglish \portuguese A classe BuildModule é o Módulo que permite o usuário criar uma heurística e definir um nome para ela. \endportuguese
/*!
  \sa run()
*/

template<class R, class M>
class BuildModule: public OptFrameModule<R, M>
{
public:

   //! \english Module id \endenglish \portuguese Identificação do módulo \endportuguese
   /*! \english
       \return string with the id of this module
       \endenglish
       \portuguese
       \return string com a identificação deste módulo
       \endportuguese
   */

   string id()
   {
      return "build";
   }

   //! \english Help for this module \endenglish \portuguese Ajuda para este módulo \endportuguese
   /*! \english
       \return string with the help for this module
       \endenglish
       \portuguese
       \return string com a ajuda para este módulo
       \endportuguese
   */

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

   //! \english Execution of the module \endenglish \portuguese Execução do módulo \endportuguese
   /*! \english
       The BuildModule receives as parameters the description of the heuristic and the desired name.

       The string for the construction of the heuristic is sent to HeuristicFactory and it creates the method. Then, the BuildModule gives the desired name to the created method.

       For more information type 'usage build' in OptFrame Command Line Interface

       \sa OptFrame::execute()
       \endenglish

       \portuguese
       O BuildModule recebe como parâmetro a descrição da heurística e o nome desejado.

       A string para construção da heurística é repassada ao HeuristicFactory, que gera efetivamente o método. Após isso o BuildModule dá o nome desejado ao método criado.

       Para mais informações digite 'usage build' na Interface de Linha de Comando do OptFrame

       \sa OptFrame::execute()
       \endportuguese
   */

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
