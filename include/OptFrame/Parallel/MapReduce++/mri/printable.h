// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

// Last update Sabir Ribas (2009-02-06)
// By Igor Machado Coelho
// Em 25 de Julho de 2009

#ifndef PRINTABLE_H_
#define PRINTABLE_H_

#include <iostream>
#include <map>
#include <ostream>
#include <vector>

using namespace std;

// Variaveis estaticas com nomes grandes para evitar conflitos externos!
static bool string_aspas_25072009 = false;
static bool string_aspas_25072009_ctrl = true;

// ===================================================
// Reescreve a impressao de strings, para
// que dentro de vectors e pares as strings
// aparecam entre aspas duplas.
// Fora desta classe nenhuma outra string e afetada.
// ===================================================
ostream&
operator<<(ostream& os, const string& obj)
{
   if (string_aspas_25072009 && string_aspas_25072009_ctrl) {
      string_aspas_25072009_ctrl = false;

      os << "\"" << obj << "\"";
   } else {
      string_aspas_25072009_ctrl = true;

      for (unsigned int i = 0; i < obj.length(); i++)
         if ((obj[i] == '\"') || (obj[i] == '\\'))
            os << '\\' << obj[i];
         else
            os << obj[i];
   }
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
   string_aspas_25072009 = true;

   os << "vector(" << obj.size() << ") [";

   if (obj.size() > 0) {
      for (unsigned int i = 0; i < obj.size() - 1; i++)
         os << obj.at(i) << " , ";
      os << obj.at(obj.size() - 1);
   }

   os << "]";

   // Libera a impressao normal de strings
   string_aspas_25072009 = false;

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
   string_aspas_25072009 = true;

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
   string_aspas_25072009 = false;

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
   string_aspas_25072009 = true;

   os << "pair(" << obj.first << " , " << obj.second << ")";

   // Libera a impressao normal de strings
   string_aspas_25072009 = false;

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
   string_aspas_25072009 = true;

   os << "pair(" << obj.first << " , ";

   if (obj.second == nullptr)
      os << "nullptr"
         << ")";
   else
      os << (*obj.second) << ")";

   // Libera a impressao normal de strings
   string_aspas_25072009 = false;

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
   string_aspas_25072009 = true;

   os << "pair(";

   if (obj.first == nullptr)
      os << "nullptr";
   else
      os << (*obj.first);

   os << " , " << obj.second << ")";

   // Libera a impressao normal de strings
   string_aspas_25072009 = false;

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
   string_aspas_25072009 = true;

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
   string_aspas_25072009 = false;

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
   string_aspas_25072009 = true;

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
   string_aspas_25072009 = false;

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
   string_aspas_25072009 = true;

   os << "map(" << obj.size() << ") [";

   if (obj.size() > 0) {
      typename map<Key, T>::iterator it;
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
   string_aspas_25072009 = false;

   return os;
}

#endif /*PRINTABLE_H_*/
