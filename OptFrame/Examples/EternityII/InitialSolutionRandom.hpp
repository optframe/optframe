#ifndef EtII_INITIALSOLUTION_Random_HPP_
#define EtII_INITIALSOLUTION_Random_HPP_

#include "../../../OptFrame/InitialSolution.h"
#include "../../../OptFrame/Util/TestSolution.hpp"

#include "../../RandGen.hpp"

#include "ProblemInstance.hpp"

#include "Representation.h"
#include "Solution.h"

#include "Evaluator.hpp"

#include <list>

#include <algorithm>
#include <stdlib.h>

using namespace std;

class EtIIInitialSolutionRandom: public InitialSolution<RepEtII>
{
private:
	EtIIProblemInstance& pEtII;
	RandGen& rg;
	// Your private vars

public:

	EtIIInitialSolutionRandom(EtIIProblemInstance& _pEtII, RandGen& _rg) : // If necessary, add more parameters
		pEtII(_pEtII), rg(_rg)
	{
	}

	SolutionEtII& generateSolution()
	{
		RepEtII* tab = new RepEtII(pEtII.height, pEtII.width);

		vector<Piece> corner_pieces;
		vector<Piece> side_pieces;
		vector<Piece> center_pieces;

		for (int i = 0; i < pEtII.pieces.size(); i++)
		{
			int zeros = 0;

			if (pEtII.pieces[i].down == 0)
				zeros++;
			if (pEtII.pieces[i].left == 0)
				zeros++;
			if (pEtII.pieces[i].up == 0)
				zeros++;
			if (pEtII.pieces[i].right == 0)
				zeros++;

			if (zeros == 2)
				corner_pieces.push_back(pEtII.pieces[i]);
			if (zeros == 1)
				side_pieces.push_back(pEtII.pieces[i]);
			if (zeros == 0)
				center_pieces.push_back(pEtII.pieces[i]);
		}

		int x;
		Piece p;

		// random one piece for left-upper corner
		x = rg.rand(corner_pieces.size());
		p = corner_pieces[x];
		corner_pieces.erase(corner_pieces.begin() + x);

		while ((p.left != 0) || (p.up != 0))
			p.rotate();

		(*tab)(0, 0) = p;

		// random one piece for right-upper corner
		x = rg.rand(corner_pieces.size());
		p = corner_pieces[x];
		corner_pieces.erase(corner_pieces.begin() + x);

		while ((p.right != 0) || (p.up != 0))
			p.rotate();

		(*tab)(0, tab->getCols() - 1) = p;

		// random one piece for right-lower corner
		x = rg.rand(corner_pieces.size());
		p = corner_pieces[x];
		corner_pieces.erase(corner_pieces.begin() + x);

		while ((p.right != 0) || (p.down != 0))
			p.rotate();

		(*tab)(tab->getRows() - 1, tab->getCols() - 1) = p;

		// random one piece for left-lower corner
		x = rg.rand(corner_pieces.size());
		p = corner_pieces[x];
		corner_pieces.erase(corner_pieces.begin() + x);

		while ((p.left != 0) || (p.down != 0))
			p.rotate();

		(*tab)(tab->getRows() - 1, 0) = p;

		// random the pieces for upper-bottom sides
		for (int i = 1; i < tab->getCols() - 1; i++)
		{
			// top
			x = rg.rand(side_pieces.size());
			p = side_pieces[x];
			side_pieces.erase(side_pieces.begin() + x);

			while (p.up != 0)
				p.rotate();
			(*tab)(0, i) = p;

			// bottom
			x = rg.rand(side_pieces.size());
			p = side_pieces[x];
			side_pieces.erase(side_pieces.begin() + x);

			while (p.down != 0)
				p.rotate();
			(*tab)(tab->getRows() - 1, i) = p;
		}

		// random the pieces for left-right sides
		for (int i = 1; i < tab->getRows() - 1; i++)
		{
			// left
			x = rg.rand(side_pieces.size());
			p = side_pieces[x];
			side_pieces.erase(side_pieces.begin() + x);

			while (p.left != 0)
				p.rotate();
			(*tab)(i, 0) = p;

			// right
			x = rg.rand(side_pieces.size());
			p = side_pieces[x];
			side_pieces.erase(side_pieces.begin() + x);

			while (p.right != 0)
				p.rotate();
			(*tab)(i, tab->getCols() - 1) = p;
		}

		// random the center pieces
		for (int i = 1; i < tab->getRows() - 1; i++)
			for (int j = 1; j < tab->getCols() - 1; j++)
			{
				x = rg.rand(center_pieces.size());
				p = center_pieces[x];
				center_pieces.erase(center_pieces.begin() + x);
				(*tab)(i, j) = p;

				int nRotation = rg.rand(4);
				for (int r = 0; r < nRotation; r++)
					(*tab)(i, j).rotate();
			}

		if (corner_pieces.size() > 0 || side_pieces.size() > 0 || center_pieces.size() > 0)
		{
			cout << "Warning: construction problem!" << endl;
		}

		return *new TestSolution<RepEtII> (*tab);
	}

};

#endif /*EtII_INITIALSOLUTION_Random_HPP_*/
