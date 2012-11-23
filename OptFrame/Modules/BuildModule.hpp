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

#ifndef BUILDMODULE_HPP_
#define BUILDMODULE_HPP_

#include "../OptFrameModule.hpp"

#include "../LocalSearch.hpp"
#include "../SingleObjSearch.hpp"
#include "../MultiObjSearch.hpp"

//! \english The BuildModule class is a Module that enables the user to create a heuristic and give a name to it. \endenglish \portuguese A classe BuildModule é o Módulo que permite o usuário criar uma heurística e definir um nome para ela. \endportuguese
/*!
  \sa run()
*/

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class M = OPTFRAME_DEFAULT_EMEMORY>
class BuildModule: public OptFrameModule<R, ADS, M>
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

   virtual ~BuildModule()
   {}

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
      string u = "build builder_and_parameters [my_method_name]\n";
      u += "to see the full list of builders type: 'list_builders OptFrame:'\n";

      return u;
   }

   //! \english Execution of the module \endenglish \portuguese Execução do módulo \endportuguese
   /*! \english
       The BuildModule receives as parameter a ComponentBuilder and the method parameters, also the desired name is optionally passed.

       For more information type 'usage build' in OptFrame Command Line Interface

       \sa OptFrame::execute()
       \endenglish

       \portuguese
       O BuildModule recebe como parâmetro um ComponentBuilder e os parâmetros do método, bem como um nome desejado para o método, opcionalmente.

       Para mais informações digite 'usage build' na Interface de Linha de Comando do OptFrame

       \sa OptFrame::execute()
       \endportuguese
   */

   bool run(vector<OptFrameModule<R, ADS, M>*>& all_modules, vector<OptFrameFunction*>& allFunctions, HeuristicFactory<R, ADS, M>& factory, map<string, string>& dictionary, string input)
   {
      //cout << "build module: " << input << endl;
      Scanner scanner1(input);

      if (!scanner1.hasNext())
      {
         cout << "Usage: " << usage() << endl;
         return false;
      }

      string type = scanner1.next();
      stringstream ss;
      ss << type << " " << scanner1.rest();
      //cout << "rest: " << ss.str() << endl;

      Scanner scanner(ss.str());

      int new_id = -1;
      string base = "";

      if(factory.compareBase(LocalSearchBuilder<R, ADS, M>::idComponent(), type))
      {
          pair<LocalSearch<R, ADS, M>*, string> method;
          method = factory.createLocalSearch(scanner.rest());
          scanner = Scanner(method.second);

          if(method.first!=NULL)
          {
        	  base   = LocalSearch<R, ADS, M>::idComponent();
        	  new_id = factory.addComponent(*method.first, base); // Adicionando como 'base', poderia adicionar como o proprio... o que eh melhor?
          }
      }
      else if(factory.compareBase(SingleObjSearchBuilder<R, ADS, M>::idComponent(), type))
      {
          pair<SingleObjSearch<R, ADS, M>*, string> method;
          method = factory.createSingleObjSearch(scanner.rest());
          scanner = Scanner(method.second);

          if(method.first!=NULL)
          {
        	  base   = SingleObjSearch<R, ADS, M>::idComponent();
        	  new_id = factory.addComponent(*method.first, base); // Adicionando como 'base', poderia adicionar como o proprio... o que eh melhor?
          }
      }
      else if(factory.compareBase(MultiObjSearchBuilder<R, ADS, M>::idComponent(), type))
      {
          pair<MultiObjSearch<R, ADS, M>*, string> method;
          method = factory.createMultiObjSearch(scanner.rest());
          scanner = Scanner(method.second);

          if(method.first!=NULL)
          {
        	  base   = MultiObjSearch<R, ADS, M>::idComponent();
        	  new_id = factory.addComponent(*method.first, base); // Adicionando como 'base', poderia adicionar como o proprio... o que eh melhor?
          }
      }
      else if(factory.getBuilder(type))
      {
    	  scanner.next(); // drop builder name!

    	  ComponentBuilder<R, ADS, M>* builder = factory.getBuilder(type);
    	  OptFrameComponent* comp = builder->buildComponent(scanner, factory);

    	  if(comp)
    	  {
    		  base   = comp->id();
    		  new_id = factory.addComponent(*comp, base);
    	  }
      }
      else
      {
    	  cout << "build_module: error '" << type << "' is not a known builder!" << endl;
    	  return false;
      }

      if(new_id < 0)
      {
    	  cout << "build module: couldn't build component!" << endl;
    	  return false;
      }

      stringstream str;
      str << base << " " << new_id;
      string s_new_id = str.str();

      //cout << "'" << s_new_id << "' added." << endl;

      if (scanner.hasNext())
      {
         string new_name = scanner.next();
         return OptFrameModule<R, ADS, M>::run_module("silent_define", all_modules, allFunctions, factory, dictionary, new_name + " " + s_new_id);
      }

      return true;
   }

};

#endif /* BUILDMODULE_HPP_ */
