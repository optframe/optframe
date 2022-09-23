// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_COMPONENT_HELPER_HPP_
#define OPTFRAME_COMPONENT_HELPER_HPP_

#include <cstdlib>
#include <iostream>
#include <vector>

#include "Scanner++/Scanner.hpp"
using namespace scannerpp;

namespace optframe {

class ComponentHelper
{
public:
   // taken from HeuristicFactory

   //! \english compareBase is an auxiliar method to compare a pattern to a component id. Check if 'component' inherits from 'base'. \endenglish \portuguese compareBase eh um metodo auxiliar para comparar um padrao a um id de componente. Testa se 'component' herda de 'base'. \endportuguese
   /*!
	 \sa compareBase(string, string)
	 */

   // Check if 'base' is inherited by 'component'
   // EXAMPLE: compareBase("OptFrame:", "OptFrame:Evaluator") returns TRUE!
   static bool compareBase(string _base, string _component)
   {
      if ((_base.length() < 3) || (_component.length() < 3)) {
         cout << "ComponentHelper::compareBase warning: comparing less than 3 characters! with base='" << _base << "' component='" << _component << "'" << endl;
         return false;
      }

      bool baseIsList = (_base.at(_base.length() - 2) == '[') && (_base.at(_base.length() - 1) == ']');
      bool componentIsList = (_component.at(_component.length() - 2) == '[') && (_component.at(_component.length() - 1) == ']');

      if (baseIsList != componentIsList)
         return false;

      // remove list (if exists)
      string base = typeOfList(_base);
      string component = typeOfList(_component);

      bool sameBase = true;

      if (base.length() <= component.length()) {
         for (unsigned i = 0; i < base.length(); i++)
            if (base.at(i) != component.at(i))
               sameBase = false;
      } else
         sameBase = false;

      if (sameBase)
         return true;

      // ------------------
      // check last family
      // ------------------

      Scanner scanner(base);
      scanner.useSeparators(":");

      string family = scanner.next();
      while (scanner.hasNext())
         family = scanner.next();

      Scanner scanComponent(component);
      scanComponent.useSeparators(":");
      string part;
      while (scanComponent.hasNext()) {
         part = scanComponent.next();
         if (part == family)
            sameBase = true;
      }

      return sameBase;
   }

   static string typeOfList(string listId)
   {
      Scanner scanner(listId);
      scanner.useSeparators(" \t\n[]");

      return scanner.next();
   }
};

}

#endif /* OPTFRAME_COMPONENT_HPP_ */
