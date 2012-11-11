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

#ifndef OPTFRAME_COMPONENT_HPP_
#define OPTFRAME_COMPONENT_HPP_

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

class OptFrameComponent
{
public:

   virtual ~OptFrameComponent()
   {
   }

   static string idComponent()
   {
      return "OptFrame:";
   }

   virtual string id() const
   {
      return idComponent();
   }


   virtual bool compatible(string s)
   {
      return s == id();
   }


   static vector<pair<string, string> >  params()
   {
	   return vector<pair<string, string> > ();
   }

   virtual vector<pair<string, string> > parameters() const
   {
	   return params();
   }

   virtual void print() const
   {
      cout << id() << endl;
   }

};

#endif /* OPTFRAME_COMPONENT_HPP_ */
