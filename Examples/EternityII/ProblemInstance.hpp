#ifndef EtII_PROBLEMINSTANCE_HPP_
#define EtII_PROBLEMINSTANCE_HPP_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

#include "Piece.h"

using namespace std;

class EtIIProblemInstance
{
public:
	vector<Piece> pieces;
	int width, height;

	EtIIProblemInstance(Scanner& scanner)
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

#endif /*EtII_PROBLEMINSTANCE_HPP_*/

