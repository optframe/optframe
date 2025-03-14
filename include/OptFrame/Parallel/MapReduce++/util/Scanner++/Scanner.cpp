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

#include "Scanner.h"

Scanner::Scanner(File* inputfile)
{
   this->inputfile = inputfile;
   this->input = inputfile->file;
   useDefaultSeparators();
}

Scanner::Scanner(istream* input)
{
   this->inputfile = nullptr;
   this->input = input;
   useDefaultSeparators();
}

Scanner::Scanner(string input)
{
   this->inputfile = nullptr;
   this->input = new istringstream(input);
   useDefaultSeparators();
}

Scanner::~Scanner()
{
   close();
}

void
Scanner::useDefaultSeparators()
{
   useSeparators(string("\n\r\t "));
}

void
Scanner::useSeparators(string s)
{
   sep = s;
}

bool
Scanner::inSeparators(char c)
{
   for (unsigned int i = 0; i < sep.length(); i++)
      if (sep[i] == c)
         return true;
   return false;
}

// =================================================================
// =================================================================

bool
Scanner::hasNextChar()
{
   if (input->eof())
      return false;

   char x = input->get();

   //cout << "get = " << x << " (" << (int)x << ")" << std::endl;

   if (x > 0) {
      input->putback(x);
      return true;
   }

   if (x == 0) {
      input->putback(x);
      return false;
   }

   return false;
}

char
Scanner::nextChar()
{
   char x = input->get();

   if (x <= 0) {
      cerr << "Excecao: Nao ha proximo char!" << std::endl;
      exit(1);
   }

   return x;
}

// =================================================================
// =================================================================

int
Scanner::nextInt()
{
   int x;
   istringstream myStream(next());
   if (myStream >> x)
      return x;
   else {
      cerr << "Scanner++: Falha na conversao nextInt()." << std::endl;
      exit(1);
   }
}

long
Scanner::nextLong()
{
   long x;
   istringstream myStream(next());
   if (myStream >> x)
      return x;
   else {
      cerr << "Scanner++: Falha na conversao nextLong()." << std::endl;
      exit(1);
   }
}

float
Scanner::nextFloat()
{
   float x;
   istringstream myStream(next());
   if (myStream >> x)
      return x;
   else {
      cerr << "Scanner++: Falha na conversao nextFloat()." << std::endl;
      exit(1);
   }
}

double
Scanner::nextDouble()
{
   double x;
   istringstream myStream(next());
   if (myStream >> x)
      return x;
   else {
      cerr << "Scanner++: Falha na conversao nextDouble()." << std::endl;
      exit(1);
   }
}

// =================================================================
// =================================================================

bool
Scanner::hasNext()
{
   if (!hasNextChar())
      return false;

   vector<char> buffer;

   char novo = nextChar();

   bool next = true;

   while (inSeparators(novo)) {
      buffer.push_back(novo);

      if (!(hasNextChar())) {
         next = false;
         break;
      }

      novo = nextChar();
   }

   if (next) // ha proximo!
      input->putback(novo);

   //devolver o buffer ao IO
   while (buffer.size() > 0) {
      input->putback(buffer.at(buffer.size() - 1));
      buffer.erase(buffer.begin() + (buffer.size() - 1));
   }

   return next;
}

std::string
Scanner::next()
{
   std::string x = "";

   // Consome caracteres iniciais
   while (hasNextChar()) {
      char c = nextChar();

      if (!inSeparators(c)) {
         x = x + c;
         break;
      }
   }

   while (hasNextChar()) {
      char c = nextChar();

      if (inSeparators(c)) {
         input->putback(c);
         break;
      }

      x = x + c;
   }

   return x;
}

bool
Scanner::hasNextLine()
{
   std::cout << "Unimplemented hasNextLine() -> calling hasNext()" << std::endl;
   return hasNext();
}

std::string
Scanner::nextLine()
{
   string backup_sep = sep;
   useSeparators("\n");
   string linha = next();
   useSeparators(backup_sep);

   return linha;
}

// =================================================================
// =================================================================

void
Scanner::close()
{
   if (inputfile) {
      delete inputfile;
      inputfile = nullptr;
   }
}
