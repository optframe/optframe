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

// Scanner Object - Scanner++

// Copyright (C) 2009
// Igor Machado - Mario Henrique Perche - Pablo Munhoz - Sabir Ribas
//
// This file is part of the Scanner++ Library v0.91.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef SCANNERPP_H_
#define SCANNERPP_H_

#include <iostream>
#include <istream>
#include <sstream>
#include <string>

#include <vector>

#include <stdlib.h>

using namespace std;

#include "File.h"

class Scanner
{
private:
   istream* input;

   File* inputfile;

   bool hasNextChar();
   char nextChar();

   string sep;

public:
   Scanner(File* inputfile);
   Scanner(istream* input);
   Scanner(string input);

   virtual ~Scanner();

   // useDefaultSeparators: chama o useSeparators para os caracteres:
   // espaco, quebra de linha (\n), tabulacao (\t) e retorno de carro (\r)

   void useDefaultSeparators();

   // useSeparators: equivalente ao useDelimiter de Java
   // a diferenca e que Java trata a string como uma
   // expressao regular, e neste caso o useSeparators
   // apenas considera cada caractere da string separadamente
   // como um separador.

   void useSeparators(string s);
   bool inSeparators(char c);

   std::string next();
   std::string nextLine();
   int nextInt();
   long nextLong();
   float nextFloat();
   double nextDouble();

   bool hasNext();
   bool hasNextLine(); // Unimplemented! Calling hasNext() instead
   bool hasNextInt() { return true; }
   bool hasNextLong() { return true; }
   bool hasNextFloat() { return true; }
   bool hasNextDouble() { return true; }

   void close();
};

#endif /*SCANNERPP_H_*/
