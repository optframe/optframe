// SPDX-License-Identifier: LGPL-3.0-or-later OR MIT
// Copyright (C) 2007-2022 - OptFrame - https://github.com/optframe/optframe

#ifndef OPTFRAME_PRINTABLE_HPP_
#define OPTFRAME_PRINTABLE_HPP_

#include <iostream>
#include <map>
#include <optional>  // requires c++17
#include <ostream>
#include <sstream>  // stringstream
#include <string>
#include <utility>
#include <vector>
//
#include <OptFrame/SemStream.hpp>

// TODO(igormcoelho): std::ostreamable
// #include "../myconcepts.h" // std::ostreamable concept!

namespace optframe {

/*
ostream&
operator<<(std::ostream& os, const nullptr_t& nptr)
{
   os << "nullptr_t";
   return os;
}
*/

template <class T1, class T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& obj);

template <class T>
std::ostream& operator<<(std::ostream& os, const std::optional<T>& obj) {
  if (!obj)
    os << "nullopt";
  else
    os << obj;
  return os;
}

// ===================================================
//     Impressao de vectors
// ===================================================
template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& obj) {
  if (&os == &optframe::cjson) {
    // json
    os << "[";
    if (obj.size() > 0) {
      for (unsigned int i = 0; i < obj.size() - 1; i++)
        os << obj.at(i) << " , ";
      os << obj.at(obj.size() - 1);
    }
    os << "]";
  } else if (&os == &optframe::ctxt) {
    // txt
    if (obj.size() > 0) {
      for (int i = 0; i < ((int)obj.size()) - 1; i++) os << obj.at(i) << '\t';
      os << obj.at(obj.size() - 1);
    }
  } else {
    // human readable (default)
    os << "vector(" << obj.size() << ") [";
    if (obj.size() > 0) {
      for (unsigned int i = 0; i < obj.size() - 1; i++)
        os << obj.at(i) << " , ";
      os << obj.at(obj.size() - 1);
    }
    os << "]";
  }

  return os;
}

// ===================================================
//   Impressao de vectors (com parametro ponteiro)
// ===================================================
template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T*>& obj) {
  // Se houver string interna, sera impressa com aspas
  // string_aspas_25072009 = true;

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
  // string_aspas_25072009 = false;

  return os;
}

// ===================================================
//     Impressao de pares
// ===================================================
template <class T1, class T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2>& obj) {
  // Se houver string interna, sera impressa com aspas
  // string_aspas_25072009 = true;

  os << "pair(" << obj.first << " , " << obj.second << ")";

  // Libera a impressao normal de strings
  // string_aspas_25072009 = false;

  return os;
}

// ===================================================
//  Impressao de pares (com segundo elemento ponteiro)
// ===================================================
template <class T1, class T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1, T2*>& obj) {
  // Se houver string interna, sera impressa com aspas
  // string_aspas_25072009 = true;

  os << "pair(" << obj.first << " , ";

  if (obj.second == nullptr)
    os << "nullptr"
       << ")";
  else
    os << (*obj.second) << ")";

  // Libera a impressao normal de strings
  // string_aspas_25072009 = false;

  return os;
}

// ===================================================
// Impressao de pares (com primeiro elemento ponteiro)
// ===================================================
template <class T1, class T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1*, T2>& obj) {
  // Se houver string interna, sera impressa com aspas
  // string_aspas_25072009 = true;

  os << "pair(";

  if (obj.first == nullptr)
    os << "nullptr";
  else
    os << *(obj.first);

  os << " , " << obj.second << ")";

  // Libera a impressao normal de strings
  // string_aspas_25072009 = false;

  return os;
}

// ===================================================
//  Impressao de pares (com ambos elementos ponteiros)
// ===================================================
template <class T1, class T2>
std::ostream& operator<<(std::ostream& os, const std::pair<T1*, T2*>& obj) {
  // Se houver string interna, sera impressa com aspas
  // string_aspas_25072009 = true;

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
  // string_aspas_25072009 = false;

  return os;
}

// ===================================================
//     Impressao de multimaps
// ===================================================
template <class Key, class T>
std::ostream& operator<<(std::ostream& os, std::multimap<Key, T>& obj) {
  // Se houver string interna, sera impressa com aspas
  // string_aspas_25072009 = true;

  os << "multimap(" << obj.size() << ") [";

  if (obj.size() > 0) {
    typename std::multimap<Key, T>::iterator it;
    int i = 0;
    for (it = obj.begin(); it != obj.end(); ++it) {
      os << *it;
      if (i != obj.size() - 1) os << " , ";
      i++;
    }
  }

  os << "]";

  // Libera a impressao normal de strings
  // string_aspas_25072009 = false;

  return os;
}

// ===================================================
//     Impressao de multimaps
// ===================================================
template <class Key, class T>
std::ostream& operator<<(std::ostream& os, std::map<Key, T>& obj) {
  // Se houver string interna, sera impressa com aspas
  // string_aspas_25072009 = true;

  os << "map(" << obj.size() << ") [";

  if (obj.size() > 0) {
    typename std::map<Key, T>::iterator it;
    int i = 0;
    for (it = obj.begin(); it != obj.end(); ++it) {
      os << *it;
      if (i != ((int)obj.size()) - 1) os << " , ";
      i++;
    }
  }

  os << "]";

  // Libera a impressao normal de strings
  // string_aspas_25072009 = false;

  return os;
}

template <class T>
std::string toString(const std::vector<T>& v) {
  std::stringstream ss;
  ss << v;
  return ss.str();
}

// COMPILATION TESTS FOR PRINTABLE
#ifndef NDEBUG

// TODO(igormcoelho): use std::ostreamable<T> here to test structures, instead
// of printing to cout

class test_runtime_can_print {
 public:
  friend std::ostream& operator<<(std::ostream& myos,
                                  const test_runtime_can_print& me) {
    return myos;
  }
};

struct test_printability_disable_runtime {
  void ftest() {
    std::pair<int, int> ppi;
    std::cout << ppi << std::endl;
    test_runtime_can_print t1;
    std::cout << t1 << std::endl;
    std::pair<test_runtime_can_print, test_runtime_can_print> tpp;
    std::cout << tpp << std::endl;
  }
};

#endif

}  // namespace optframe

#endif /*OPTFRAME_PRINTABLE_HPP_*/
