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

#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

template<class KeyA, class A, class KeyB, class B, class C>
class MaPI_Serializer
{
public:

	virtual KeyA	KeyA_fromString(string s)	= 0;
	virtual string	KeyA_toString(KeyA o)	= 0;

	virtual A		A_fromString(string s)		= 0;
	virtual string	A_toString(A o)				= 0;

	virtual KeyB	KeyB_fromString(string s)	= 0;
	virtual string	KeyB_toString(KeyB o)		= 0;

	virtual B		B_fromString(string s)		= 0;
	virtual string	B_toString(B o)				= 0;

	virtual C		C_fromString(string s)		= 0;
	virtual string	C_toString(C o)				= 0;

};

#include "../MaPI/MaPI.h"
#include <iostream>

class MaPI_StrSerializer : public MaPI_Serializer<string,string,string,string,string>
{
public:

	virtual string	KeyA_fromString(string s)	{return s;};
	virtual string	KeyA_toString(string o)		{return o;};

	virtual string	A_fromString(string s)		{return s;};
	virtual string	A_toString(string o)		{return o;};

	virtual string	KeyB_fromString(string s)	{return s;};
	virtual string	KeyB_toString(string o)		{return o;};

	virtual string	B_fromString(string s)		{return s;};
	virtual string	B_toString(string o)		{return o;};

	virtual string	C_fromString(string s)		{return s;};
	virtual string	C_toString(string o)		{return o;};

};

#endif /* SERIALIZER_HPP */
