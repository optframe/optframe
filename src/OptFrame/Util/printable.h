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

#ifndef OPTFRAME_PRINTABLE_H_
#define OPTFRAME_PRINTABLE_H_

#include <iostream>
#include <map>
#include <ostream>
#include <sstream> // stringstream
#include <vector>

#include "../myconcepts.h" // ostreamable concept!

using namespace std;

namespace optframe {

template<class T1, class T2>
ostream&
operator<<(ostream& os, const pair<T1, T2>& obj);

template<class T>
ostream&
operator<<(ostream& os, const std::optional<T>& obj)
{
   if(!obj)
      os << "nullopt";
   else
      os << obj;
   return os;
}


// ===================================================
//     Impressao de vectors
// ===================================================
template<class T>
ostream&
operator<<(ostream& os, const vector<T>& obj)
{
   // Se houver string interna, sera impressa com aspas
   //string_aspas_25072009 = true;

   os << "vector(" << obj.size() << ") [";

   if (obj.size() > 0) {
      for (unsigned int i = 0; i < obj.size() - 1; i++)
         os << obj.at(i) << " , ";
      os << obj.at(obj.size() - 1);
   }

   os << "]";

   // Libera a impressao normal de strings
   //string_aspas_25072009 = false;

   return os;
}

// ===================================================
//   Impressao de vectors (com parametro ponteiro)
// ===================================================
template<class T>
ostream&
operator<<(ostream& os, const vector<T*>& obj)
{
   // Se houver string interna, sera impressa com aspas
   //string_aspas_25072009 = true;

   os << "vector(" << obj.size() << ") [";

   if (obj.size() > 0) {
      for (unsigned int i = 0; i < obj.size() - 1; i++)
         if (obj.at(i) == nullptr)
            os << "nullptr"
               << " , ";
         else
            os << (*obj.at(i)) << " , ";

      if (obj.size() > 0) {
         if (obj.at(obj.size() - 1) == nullptr)
            os << "nullptr";
         else
            os << (*obj.at(obj.size() - 1));
      }
   }

   os << "]";

   // Libera a impressao normal de strings
   //string_aspas_25072009 = false;

   return os;
}

// ===================================================
//     Impressao de pares
// ===================================================
template<class T1, class T2>
ostream&
operator<<(ostream& os, const pair<T1, T2>& obj)
{
   // Se houver string interna, sera impressa com aspas
   //string_aspas_25072009 = true;

   os << "pair(" << obj.first << " , " << obj.second << ")";

   // Libera a impressao normal de strings
   //string_aspas_25072009 = false;

   return os;
}

// ===================================================
//  Impressao de pares (com segundo elemento ponteiro)
// ===================================================
template<class T1, class T2>
ostream&
operator<<(ostream& os, const pair<T1, T2*>& obj)
{
   // Se houver string interna, sera impressa com aspas
   //string_aspas_25072009 = true;

   os << "pair(" << obj.first << " , ";

   if (obj.second == nullptr)
      os << "nullptr"
         << ")";
   else
      os << (*obj.second) << ")";

   // Libera a impressao normal de strings
   //string_aspas_25072009 = false;

   return os;
}

// ===================================================
// Impressao de pares (com primeiro elemento ponteiro)
// ===================================================
template<class T1, class T2>
ostream&
operator<<(ostream& os, const pair<T1*, T2>& obj)
{
   // Se houver string interna, sera impressa com aspas
   //string_aspas_25072009 = true;

   os << "pair(";

   if (obj.first == nullptr)
      os << "nullptr";
   else
      os << (*obj.first);

   os << " , " << obj.second << ")";

   // Libera a impressao normal de strings
   //string_aspas_25072009 = false;

   return os;
}

// ===================================================
//  Impressao de pares (com ambos elementos ponteiros)
// ===================================================
template<class T1, class T2>
ostream&
operator<<(ostream& os, const pair<T1*, T2*>& obj)
{
   // Se houver string interna, sera impressa com aspas
   //string_aspas_25072009 = true;

   os << "pair(";

   if (obj.first == nullptr)
      os << "nullptr";
   else
      os << (*obj.first);

   os << " , ";

   if (obj.second == nullptr)
      os << "nullptr"
         << ")";
   else
      os << (*obj.second) << ")";

   // Libera a impressao normal de strings
   //string_aspas_25072009 = false;

   return os;
}

// ===================================================
//     Impressao de multimaps
// ===================================================
template<class Key, class T>
ostream&
operator<<(ostream& os, multimap<Key, T>& obj)
{
   // Se houver string interna, sera impressa com aspas
   //string_aspas_25072009 = true;

   os << "multimap(" << obj.size() << ") [";

   if (obj.size() > 0) {
      typename multimap<Key, T>::iterator it;
      int i = 0;
      for (it = obj.begin(); it != obj.end(); ++it) {
         os << *it;
         if (i != obj.size() - 1)
            os << " , ";
         i++;
      }
   }

   os << "]";

   // Libera a impressao normal de strings
   //string_aspas_25072009 = false;

   return os;
}

// ===================================================
//     Impressao de multimaps
// ===================================================
template<class Key, class T>
ostream&
operator<<(ostream& os, map<Key, T>& obj)
{
   // Se houver string interna, sera impressa com aspas
   //string_aspas_25072009 = true;

   os << "map(" << obj.size() << ") [";

   if (obj.size() > 0) {
      typename map<Key, T>::iterator it;
      int i = 0;
      for (it = obj.begin(); it != obj.end(); ++it) {
         os << *it;
         if (i != ((int)obj.size()) - 1)
            os << " , ";
         i++;
      }
   }

   os << "]";

   // Libera a impressao normal de strings
   //string_aspas_25072009 = false;

   return os;
}

template<class T>
std::string
toString(const std::vector<T>& v)
{
   std::stringstream ss;
   ss << v;
   return ss.str();
}


// COMPILATION TESTS FOR PRINTABLE
#ifndef NDEBUG

// TODO: use ostreamable<T> here to test structures, instead of printing to cout

class test_runtime_can_print
{
public:

   friend std::ostream& operator<<(std::ostream& myos, const test_runtime_can_print& me)
   {
      return myos;
   }
};

struct test_printability_disable_runtime
{
   void ftest()
   {
      std::pair<int,int> ppi;
      std::cout << ppi << std::endl;
      test_runtime_can_print t1;
      std::cout << t1 << std::endl;
      std::pair<test_runtime_can_print,test_runtime_can_print> tpp;
      std::cout << tpp << std::endl;
   }

};

#endif


} // namespace optframe

#endif /*OPTFRAME_PRINTABLE_H_*/
