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

#ifndef PIECE_H_
#define PIECE_H_

class Piece
{
public:
   int down, left, up, right;
   int index, rotation;
   // ...

   Piece()
   {
      down = 0;
      left = 0;
      up = 0;
      right = 0;
      index = -1;
      rotation = 0;
   }

   Piece(int _down, int _left, int _up, int _right, int _index, int _rotation)
     : down(_down)
     , left(_left)
     , up(_up)
     , right(_right)
     , index(_index)
     , rotation(_rotation)
   {
   }

   void rotate() // rotate 90 degrees clockwise
   {
      int first_down = down;

      down = right;
      right = up;
      up = left;

      left = first_down;

      rotation++;
      if (rotation == 4)
         rotation = 0;
   }

   friend ostream& operator<<(ostream& os, const Piece& obj)
   {
      os << "(" << obj.down << "," << obj.left << "," << obj.up << "," << obj.right << ")";
      return os;
   }
};

/*
ostream& operator<<(ostream &os, const Matrix<Piece> &obj)
{
	cout << "game" << endl;
	for (unsigned int i = 0; i < obj.getNumRows(); i++)
		for (int count = 0; count < 3; count++)
		{
			if (count != 1)
				cout << " ";
			for (unsigned int j = 0; j < obj.getNumCols(); j++)
			{
				if (count == 0)
					cout << obj(i, j).up << "   ";
				else if (count == 1)
					cout << obj(i, j).left << " " << obj(i, j).right << " ";
				else
					cout << obj(i, j).down << "   ";
			}

			cout << endl;
		}

	return os;
}
*/

#endif /* PIECE_H_ */
