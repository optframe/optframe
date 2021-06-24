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

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

template<class KeyA, class A, class KeyB, class B, class C>
class MaPI_Serializer
{
public:
   virtual KeyA KeyA_fromString(string s) = 0;
   virtual string KeyA_toString(KeyA o) = 0;

   virtual A A_fromString(string s) = 0;
   virtual string A_toString(A o) = 0;

   virtual KeyB KeyB_fromString(string s) = 0;
   virtual string KeyB_toString(KeyB o) = 0;

   virtual B B_fromString(string s) = 0;
   virtual string B_toString(B o) = 0;

   virtual C C_fromString(string s) = 0;
   virtual string C_toString(C o) = 0;
};

#include "../MaPI/MaPI.h"
#include <iostream>

class MaPI_StrSerializer : public MaPI_Serializer<string, string, string, string, string>
{
public:
   virtual string KeyA_fromString(string s) { return s; };
   virtual string KeyA_toString(string o) { return o; };

   virtual string A_fromString(string s) { return s; };
   virtual string A_toString(string o) { return o; };

   virtual string KeyB_fromString(string s) { return s; };
   virtual string KeyB_toString(string o) { return o; };

   virtual string B_fromString(string s) { return s; };
   virtual string B_toString(string o) { return o; };

   virtual string C_fromString(string s) { return s; };
   virtual string C_toString(string o) { return o; };
};

#endif /* SERIALIZER_HPP */
