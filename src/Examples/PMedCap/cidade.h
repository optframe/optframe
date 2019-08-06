// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
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

#ifndef CIDADE_H_
#define CIDADE_H_
#include <math.h>
class Cidade
{

private:

public:
	double x, y;
	int capacidade;
	int demanda;

	Cidade(double a, double b,int c, int d)
	{
		x = a;
		y = b;
		capacidade = c;
		demanda = d;
	}


	Cidade()
	{
		x = -5;
		y = -5;
	}


};


#endif /* CIDADE_H_ */
