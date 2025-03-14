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

#ifndef OPTFRAME_LIST_HPP_
#define OPTFRAME_LIST_HPP_

#include <iostream>
#include <string>
#include <vector>
//
#include <OptFrame/Scanner++/Scanner.hpp>

// using namespace std;
// using namespace scannerpp;

namespace optframe {

using scannerpp::Scanner;
class OptFrameList {
 public:
  static string intToString(int i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
  }

  static vector<std::string>* readList(
      const std::map<std::string, std::vector<std::string>>& ldictionary,
      string list) {
    Scanner scanner(list);
    return readList(ldictionary, scanner);
  }

  static vector<std::string>* readList(
      const std::map<std::string, std::vector<std::string>>& ldictionary,
      Scanner& scanner) {
    scanner.trimInput();

    if (!scanner.hasNextChar()) return nullptr;

    char character = *scanner.nextChar();
    int numberOfBrackets;

    if (character != '[')  // read from dictionary
    {
      stringstream ssword;
      while (character != ' ')  // or... there is no next char!
      {
        ssword << character;
        if (!scanner.hasNextChar()) break;
        character = *scanner.nextChar();
      }

      string word = ssword.str();

      if (ldictionary.count(word) == 0)  // will try numeric list
      {
        auto obegin = Scanner::parseInt(word);

        if (!obegin) {
          std::cout << "OptFrameList error: variable '" << word
                    << "' not found in list dictionary!" << std::endl;
          return nullptr;
        }

        int begin = *obegin;

        if (!scanner.hasNext()) return nullptr;

        string doubledots = scanner.next();

        if (doubledots != "..") return nullptr;

        if (!scanner.hasNext()) return nullptr;

        string send = scanner.next();

        auto oend = Scanner::parseInt(send);
        if (!oend) {
          return nullptr;
        }

        int end = *oend;

        vector<string>* numeric_list = new vector<string>();

        if (begin < end)
          for (int i = begin; i <= end; i++)
            numeric_list->push_back(intToString(i));
        else
          for (int i = begin; i >= end; i--)
            numeric_list->push_back(intToString(i));

        return numeric_list;
      } else {
        vector<string>* list =
            new vector<string>(ldictionary.find(word)->second);
        return list;
      }
    }

    vector<std::string>* list = new vector<std::string>;
    std::string word;

    numberOfBrackets = 0;

    if (!scanner.hasNextChar()) return nullptr;

    character = *scanner.nextChar();

    word = "";
    while ((character != ']') || ((character == ']') && numberOfBrackets > 0)) {
      if (character == '[') numberOfBrackets++;
      if (character == ']') numberOfBrackets--;

      if ((character == ',') && (numberOfBrackets == 0)) {
        list->push_back(word);
        word = "";
      } else {
        word += character;
      }

      if (!scanner.hasNextChar()) return nullptr;

      character = *scanner.nextChar();
    }
    list->push_back(word);

    for (unsigned int i = 0; i < list->size(); i++) {
      list->at(i) = scanner.trim(list->at(i));
    }

    // TODO: error in code, dont know why but list should be empty!
    if ((list->size() == 1) && (Scanner::trim(list->at(0)) == ""))
      list->pop_back();

    return list;
  }

  static vector<std::string>* readBlock(Scanner& scanner) {
    if (!scanner.hasNextChar()) return nullptr;

    char character = *scanner.nextChar();
    int numberOfBrackets;

    // trim input
    while (character == ' ') {
      if (!scanner.hasNextChar()) return nullptr;

      character = *scanner.nextChar();
    }

    if (character != '{')  // can't read block from dictionary
    {
      if (character == '[')
        std::cout << "OptFrameList::readBlock() error: trying to read block "
                     "from a possible list structure!"
                  << std::endl;
      return nullptr;
    }

    vector<string>* block = new vector<string>;
    string word;

    numberOfBrackets = 0;

    if (!scanner.hasNextChar()) return nullptr;

    character = *scanner.nextChar();

    word = "";
    while ((character != '}') || ((character == '}') && numberOfBrackets > 0)) {
      if (character == '{') numberOfBrackets++;
      if (character == '}') numberOfBrackets--;

      if ((character == ';') && (numberOfBrackets == 0)) {
        block->push_back(word);
        word = "";
      } else {
        word += character;
      }

      if (!scanner.hasNextChar()) return nullptr;

      character = *scanner.nextChar();
    }

    block->push_back(word);

    for (unsigned int i = 0; i < block->size(); i++)
      block->at(i) = scanner.trim(block->at(i));

    // allow last command with semicolon (optional)
    if ((block->size() > 0) && (block->at(block->size() - 1) == ""))
      block->pop_back();

    return block;
  }

  static string listToString(const vector<string>& list) {
    std::stringstream ss;

    ss << "[";
    for (unsigned i = 0; i < list.size(); i++) {
      ss << list.at(i);
      if (i != list.size() - 1) ss << ",";
    }
    ss << "]";

    return ss.str();
  }

  static string blockToString(const vector<string>& block) {
    std::stringstream ss;

    ss << "{";
    for (unsigned i = 0; i < block.size(); i++) {
      ss << block.at(i);
      if (i != block.size() - 1) ss << ";";
    }
    ss << "}";

    return ss.str();
  }
};

}  // namespace optframe

#endif /* OPTFRAME_LIST_HPP_ */
