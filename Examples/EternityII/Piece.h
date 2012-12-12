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

#ifndef PIECE_H_
#define PIECE_H_

class Piece
{
public:
	int down, left, up, right;
	int index, rotation;

	Piece()
	{
		down = 0;
		left = 0;
		up = 0;
		right = 0;
		index = -1;
		rotation = 0;
	}

	Piece(int _down, int _left, int _up, int _right, int _index, int _rotation) :
		down(_down), left(_left), up(_up), right(_right), index(_index), rotation(_rotation)
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

	friend ostream& operator<<(ostream &os, const Piece &obj)
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
