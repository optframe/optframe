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

#ifndef PCAP_PROBLEMINSTANCE_HPP_
#define PCAP_PROBLEMINSTANCE_HPP_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"
#include "./cidade.h"

using namespace scannerpp;
using namespace std;


class PCAPProblemInstance
{
private:



public:
	int nCidades;
	int nMedianas;
	vector<Cidade> vecCidades;
	PCAPProblemInstance(Scanner& scanner)
	{
		nCidades = scanner.nextInt();
		nMedianas = scanner.nextInt();

		while(scanner.hasNext())
		{
			double x = scanner.nextDouble();
			double y = scanner.nextDouble();
			int cap = scanner.nextInt();
			int d = scanner.nextInt();
			Cidade c(x,y,cap,d);
			vecCidades.push_back(c);
		}
		// Put here your code
		// You can read the input data from the 'scanner' object
	}
};

#endif /*PCAP_PROBLEMINSTANCE_HPP_*/

