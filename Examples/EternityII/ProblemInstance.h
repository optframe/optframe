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

#ifndef EtII_PROBLEMINSTANCE_HPP_
#define EtII_PROBLEMINSTANCE_HPP_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

#include "Piece.h"

using namespace std;
using namespace scannerpp;

namespace EtII
{

class ProblemInstance
{
public:
	vector<Piece> pieces;
	int width, height;

	ProblemInstance(Scanner& scanner)
	{
		cout << "Reading problem instance" << endl;
		width = scanner.nextInt();
		height = scanner.nextInt();

		cout << "width = " << width << "; height = " << height << ";" << endl;
		cout << "pieces" << endl;

		pieces.clear();
		int numPieces = width * height;

		for (int i = 0; i < numPieces; i++)
		{
			cout << i << ": ";

			int down = scanner.nextInt();
			int left = scanner.nextInt();
			int up = scanner.nextInt();
			int right = scanner.nextInt();

			Piece& p = *new Piece(down, left, up, right, i, 0);

			cout << p << endl;

			pieces.push_back(p);
		}
	}
};

}

#endif /*EtII_PROBLEMINSTANCE_HPP_*/

