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

#ifndef TSP_REPRESENTATION_H_
#define TSP_REPRESENTATION_H_

#include <vector>
#include "../../OptFrame/Util/printable.h"

#include "../../OptFrame/Util/Matrix.hpp"

#include "Memory.h"

#include <vector>

using namespace std;

// Solution Representation
typedef vector < int >  RepTSP;

#ifdef MaPI

class TSPSerializer : public MyMaPISerializer<RepTSP,MemTSP>
{
public:

	virtual RepTSP KeyA_fromString(string s)
	{
		//printf("\nINIT================\n");
		RepTSP r;
		//cout << "# "<< s << endl;
		//int i;
		//cin >> i;
		Scanner scan(s);
		scan.useSeparators("()[], ");

		scan.next(); // vector
		scan.next(); // int size

		while (scan.hasNext()) r.push_back(scan.nextInt());

		//cout << "##" << r << endl;

		//printf("\nFIM================\n");
		return r;
	}
	;
	virtual string KeyA_toString(RepTSP o)
	{
		stringstream s;
		s << o;
		return s.str();
	}
	;

	virtual RankAndStop A_fromString(string s)
	{
		RankAndStop idd;
		Scanner scan(s);
		scan.useSeparators("()[], \t\r\npair");
		idd.first = scan.nextInt();
		idd.second.first = scan.nextDouble();
		idd.second.second = scan.nextDouble();
		return idd;
	}
	;
	virtual string A_toString(RankAndStop o)
	{
		stringstream s;
		s << o;
		return s.str();
	}
	;

	virtual int KeyB_fromString(string s)
	{
		return atoi(s.c_str());
	}
	;
	virtual string KeyB_toString(int o)
	{
		stringstream s;
		s << o;
		return s.str();
	}
	;

	virtual pair<RepTSP, double> B_fromString(string s) // TODO
	{
		//printf("\nINIT================\n%s\n",s.c_str());
		//cout << "Initializing!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;

		RepTSP r;
		double c;

		//cout << endl << "# "<< s << endl;

		Scanner scan(s);
		scan.useSeparators("()[], \t\n");

		scan.next(); // pair
		scan.next(); // vector

//		cout << scan.next(); // pair
//		cout << scan.next(); // -1

//		cout << scan.next(); // pair
//		cout << scan.next(); // vector

		int size = scan.nextInt(); // int size

		//cout << size << "!" << endl;

		for (int i = 0 ; i < size && scan.hasNext() ; i++) r.push_back(scan.nextInt());

		//int i;				cin >> i;

		//cout << "rest" << scan.rest() << endl;

		c = scan.nextDouble();

		//cout << c << endl;

		//cout << "##" << r << " c=" << c << endl;
		//printf("\nFIM================\n");
		return make_pair(r,c);
	}
	;
	virtual string B_toString(pair<RepTSP, double> o)
	{
		stringstream s;
		s << o;
		return s.str();
	}
	;

	virtual RepTSP C_fromString(string s)
	{
		RepTSP r;
		//cout << "# "<< s << endl;
		//int i;
		//cin >> i;
		Scanner scan(s);
		scan.useSeparators("()[], ");

		scan.next(); // vector
		scan.next(); // int size

		while (scan.hasNext()) r.push_back(scan.nextInt());

		//cout << "##" << r << endl;

		return r;
	}
	;
	virtual string C_toString(RepTSP o)
	{
		stringstream s;
		s << o;
		return s.str();
	}
	;

};

#endif

#endif /*TSP_REPRESENTATION_H_*/

