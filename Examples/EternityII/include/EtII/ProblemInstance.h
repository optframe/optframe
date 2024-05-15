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

#ifndef EtII_PROBLEMINSTANCE_HPP_
#define EtII_PROBLEMINSTANCE_HPP_

#include <iostream>

#include <OptFrame/Scanner++/Scanner.hpp>

#include "Piece.h"

using namespace std;
using namespace scannerpp;

namespace EtII {

class ProblemInstance
{
public:
   std::vector<Piece> pieces;
   int width, height;

   ProblemInstance(Scanner& scanner)
   {
      cout << "Reading problem instance" << endl;
      width = *scanner.nextInt();
      height = *scanner.nextInt();

      cout << "width = " << width << "; height = " << height << ";" << endl;
      cout << "pieces" << endl;

      pieces.clear();
      int numPieces = width * height;

      for (int i = 0; i < numPieces; i++) {
         cout << i << ": ";

         int down = *scanner.nextInt();
         int left = *scanner.nextInt();
         int up = *scanner.nextInt();
         int right = *scanner.nextInt();

         Piece& p = *new Piece(down, left, up, right, i, 0);

         cout << p << endl;

         pieces.push_back(p);
      }
   }
};

}

#endif /*EtII_PROBLEMINSTANCE_HPP_*/
