// Scanner Class for C++ - Scanner++
// https://github.com/optframe/scannerpp
// version v1.0.4 (development)
// MIT License
#ifndef SCANNERPP_SCANNER_HPP_
#define SCANNERPP_SCANNER_HPP_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <istream>
#include <optional>
#include <sstream>
#include <string>

#include <map>
#include <vector>

#include <stdlib.h>

using namespace std; // TODO: remove!

namespace scannerpp {

class File final
{
public:
   std::ifstream* filestream{ nullptr };
   std::string filename;

   // standard constructive: pass 'filename'
   File(std::string _filename) noexcept
   {
      // check if file exists
      std::fstream fs;
      fs.open(_filename.c_str());
      if (fs.is_open()) {
         fs.close();
         // store definitive pointer
         filename = _filename;
         filestream = new std::ifstream(filename.c_str(), std::ifstream::in);
      }
   }

   /*
   // empty file (not open)
   File()
     : filename("")
   {
   }
*/

   // move constructor
   File(File&& f) noexcept
     : filestream(f.filestream)
     , filename(f.filename)
   {
      f.filestream = nullptr;
      f.filename = "";
   }

   ~File() noexcept
   {
      close();
      if (filestream)
         delete filestream;
   }

   // returns 'true' if file is open
   bool isOpen() noexcept
   {
      return filestream != nullptr;
   }

   int get() noexcept
   {
      return filestream && filestream->get();
   }

   bool eof() noexcept
   {
      return filestream && filestream->eof();
   }

   void close() noexcept
   {
      if (filestream)
         filestream->close();
      filename = "";
   }
};

class Scanner final
{
private:
   istream* input{ nullptr };
   File* inputfile{ nullptr };
   string sep;
   bool isString;

   string discarded;

   string contentString;

public:
   Scanner(File&& f) noexcept
   {
      // check if file is open
      if (f.isOpen()) {
         // file is fine
         isString = false;
         this->inputfile = new File(std::move(f));
         this->input = inputfile->filestream;
      } else {
         // file is not fine: fallback to empty string
         isString = true;
         this->contentString = "";
         this->input = new std::istringstream(this->contentString);
      }
      useDefaultSeparators();
   }

   Scanner(std::istream* input) noexcept
   {
      isString = false;
      this->input = input;
      useDefaultSeparators();
   }

   Scanner(std::string input) noexcept
   {
      isString = true;
      this->contentString = input;
      this->input = new std::istringstream(input);
      useDefaultSeparators();
   }

   // copy constructor
   Scanner(const Scanner& scanner) noexcept
   {
      contentString = scanner.contentString;
      isString = scanner.isString;
      discarded = scanner.discarded;

      if (scanner.inputfile) //for files
      {
         inputfile = new File(scanner.inputfile->filename);
         input = inputfile->filestream;
      }

      if (isString) // for string
         input = new istringstream(contentString);

      if (!isString) // for cin
         input = scanner.input;

      useDefaultSeparators();
   }

   ~Scanner() noexcept
   {
      if (inputfile) {
         delete inputfile;
         inputfile = nullptr;
         input = nullptr;
      }

      if (input && isString) {
         delete input;
      }
   }

   Scanner& operator=(const Scanner& scanner) noexcept
   {
      if (&scanner == this) // auto ref check
         return *this;

      // ==========
      // destructor
      // ==========

      if (inputfile) {
         delete inputfile;
         inputfile = nullptr;
         input = nullptr;
      }

      if (input && isString) {
         delete input;
      }
      // ==========

      contentString = scanner.contentString;
      isString = scanner.isString;
      discarded = scanner.discarded;

      if (scanner.inputfile) //for files
      {
         inputfile = new File(scanner.inputfile->filename);
         input = inputfile->filestream;
      }

      if (isString) // for string
         input = new istringstream(contentString);

      if (!isString) // for cin
         input = scanner.input;

      useDefaultSeparators();

      return *this;
   }

private:
   //char nextChar(istream& _input) const noexcept
   bool getChar(istream& _input, char& c) const noexcept
   {
      int x = _input.get();

      if (x <= 0) {
         //throw ConversionError("char");
         // TODO: use bool on return and char on parameter (better than optional)
         //exit(1);
         return false;
      }

      // returns char on parameter 'c' (no exception is ever thrown)
      c = x;
      //return x;
      return true;
   }

   void put_back(istream** input, string back) const
   {
      if ((*input)->eof()) {
         // change internal pointer to a renewed istream
         delete (*input);
         (*input) = new istringstream(back);
      } else {
         // just return everything back!

         for (int i = ((int)back.length()) - 1; i >= 0; i--) {
            (*input)->putback(back[i]);
            if ((*input)->fail()) {
               cout << "SCANNER ERROR PUTTING BACK CHAR '" << back[i] << "'" << endl;
               cout << "eof bit: '" << (*input)->eof() << "'" << endl;
               cout << "bad bit: '" << (*input)->bad() << "'" << endl;
               cout << "fail bit: '" << (*input)->fail() << "'" << endl;
               exit(1);
            }
         }
      }
   }

public:
   // useDefaultSeparators: chama o useSeparators para os caracteres:
   // espaco, quebra de linha (\n), tabulacao (\t) e retorno de carro (\r)

   void useDefaultSeparators()
   {
      useSeparators(string("\n\r\t "));
   }

   // useSeparators: equivalente ao useDelimiter de Java
   // a diferenca e que Java trata a string como uma
   // expressao regular, e neste caso o useSeparators
   // apenas considera cada caractere da string separadamente
   // como um separador.

   void useSeparators(string s)
   {
      sep = s;
   }

   bool inSeparators(char c) const noexcept
   {
      for (unsigned int i = 0; i < sep.length(); i++)
         if (sep[i] == c)
            return true;
      return false;
   }

public:
   // inspecting file properties

   // returns filename, if some file is open
   string filename() const noexcept
   {
      return inputfile ? inputfile->filename : "";
   }

   // returns true if file is open
   bool fileIsOpen() const noexcept
   {
      return inputfile && inputfile->isOpen();
   }

public:
   // helper for discarded text on 'next'
   string getDiscarded() const
   {
      return discarded;
   }

public:
   // public interfaces: nextChar and hasNextChar

   bool hasNextChar() const noexcept
   {
      if (input->eof())
         return false;

      int x = input->peek();

      if (input->fail()) {
         //cout << "WARNING::SCANNER FAILED!" << endl;
         return false;
      }

      if (x > 0)
         return true;

      if (x == 0)
         return false;

      return false;
   }

   std::optional<char> nextChar()
   {
      int x = input->get();

      if (x <= 0)
         //throw ConversionError("char");
         return std::nullopt;

      return std::make_optional(x);
   }

   bool nextCharIs(char c) const
   {
      stringstream ss;
      ss << c;
      string s = ss.str();

      return nextCharIn(s);
   }

   bool nextCharIn(string s) const
   {
      if (!hasNextChar())
         return false;

      bool r = false;

      int x = input->get();

      if (x > 0) {
         char c = x;

         for (unsigned i = 0; i < s.length(); i++)
            if (c == s.at(i)) {
               r = true;
               break;
            }
      }

      input->putback((char)x);

      return r;
   }

public:
   // helper function that can be used externally to "clean" stream data
   void trimInput() noexcept
   {
      string s = " \t\n";

      if (!hasNextChar())
         return;

      int x = input->get();

      while (x > 0) {
         char c = x;

         bool t = false;

         for (unsigned i = 0; i < s.length(); i++)
            if (c == s.at(i)) {
               t = true;
               break;
            }

         if (!t) {
            input->putback((char)x);
            return;
         }

         if (!hasNextChar())
            return;

         x = input->get();
      }
   }

public:
   // next methods: for string
   std::string peekNext() const noexcept
   {
      if (input->eof())
         return "";

      std::string discarded = "";

      std::string x = "";

      istream* input = const_cast<istream*>(this->input);

      while (hasNextChar()) {
         char c = '\0';
         getChar(*input, c); // ignoring output (should be 'true', because of 'hasNextChar')

         if (!inSeparators(c)) {
            x = x + c;
            break;
         } else
            discarded += c;
      }

      while (hasNextChar()) {
         char c = '\0';
         getChar(*input, c); // ignoring output (should be 'true', because of 'hasNextChar')

         if (inSeparators(c)) {
            input->putback(c);
            break;
         }

         x = x + c;
      }

      discarded.append(x);

      put_back(const_cast<istream**>(&this->input), discarded);

      return x;
   }

   // next does not need 'optional' afterall... very good! it cannot throw.
   std::string next() noexcept
   {
      discarded = "";

      std::string x = "";

      while (hasNextChar()) {
         char c;
         if (!getChar(*input, c))
            break;

         if (!inSeparators(c)) {
            x = x + c;
            break;
         } else
            discarded += c;
      }

      while (hasNextChar()) {
         char c;
         if (!getChar(*input, c))
            break;

         if (inSeparators(c)) {
            input->putback(c);
            break;
         }

         x = x + c;
      }

      return x;
   }

   std::string nextLine() noexcept
   {
      string backup_sep = sep;
      useSeparators("\n");
      string linha = next();
      useSeparators(backup_sep);

      return linha;
   }

   // next numbers: nextInt, nextLong, nextFloat, nextDouble
public:
   std::optional<int> nextInt() noexcept
   {
      int x;
      istringstream myStream(next());
      if (myStream >> x)
         return std::make_optional(x);
      else
         //throw ConversionError("int");
         return std::nullopt;
   }

   std::optional<long> nextLong() noexcept
   {
      long x;
      istringstream myStream(next());
      if (myStream >> x)
         return std::make_optional(x);
      else
         //throw ConversionError("long");
         return std::nullopt;
   }

   std::optional<float> nextFloat() noexcept
   {
      float x;
      istringstream myStream(next());
      if (myStream >> x)
         return std::make_optional(x);
      else
         //throw ConversionError("float");
         return std::nullopt;
   }

   std::optional<double> nextDouble() noexcept
   {
      double x;
      istringstream myStream(next());
      if (myStream >> x)
         return std::make_optional(x);
      else
         //throw ConversionError("double");
         return std::nullopt;
   }

public:
   static std::optional<int> parseInt(std::string s)
   {
      Scanner scanner(s);
      if (!scanner.hasNextInt())
         return std::nullopt;
      return scanner.nextInt();
   }

   static std::optional<double> parseDouble(std::string s)
   {
      Scanner scanner(s);
      if (!scanner.hasNextDouble())
         return std::nullopt;
      return scanner.nextDouble();
   }

   // trim methods
public:
   static bool trimChar(char c) noexcept
   {
      return (c == ' ') || (c == '\t') || (c == '\n');
   }

   static string trim(string word) noexcept
   {
      if (word.length() == 0)
         return "";

      int i = 0;
      char c = word.at(i);
      string aux_word = "";

      //removing initial spaces
      while (trimChar(c) && (i < ((int)word.length()) - 1)) {
         i++;
         c = word.at(i);
      }

      if (trimChar(c)) // last char
         i++;

      while (i < ((int)word.length())) {
         aux_word += word.at(i);
         i++;
      }

      word = aux_word;

      // may be empty at this point
      if (word.length() == 0)
         return "";

      //removing final spaces
      i = ((int)word.length()) - 1;
      c = word.at(i);

      while (trimChar(c) && (i > 0)) {
         i--;
         c = word.at(i);
      }

      aux_word = "";

      for (int j = 0; j <= i; j++)
         aux_word += word.at(j);

      return aux_word;
   }

public:
   // XML method: it can segfault! beware!
   pair<string, map<string, string>> nextXMLTag()
   {
      string x = "";

      while (hasNextChar()) {
         char c = *nextChar();

         if (c == '<') {
            x = x + c;
            break;
         }
      }

      while (hasNextChar()) {
         char c = *nextChar();
         x = x + c;

         if (c == '>')
            break;
      }

      cout << "base: " << x << endl;

      if (x.size() < 2 || x.at(0) != '<' || x.at(x.size() - 1) != '>')
         return make_pair("", map<string, string>());

      Scanner scanner(x);
      scanner.useSeparators("<>");

      string tagname = "";
      map<string, string> attr;

      if (scanner.hasNext()) {
         string tag = scanner.next();
         //cout << "tag: " << tag << endl;

         Scanner sc_tag(tag);
         sc_tag.useSeparators(" ");

         if (sc_tag.hasNext())
            tagname = sc_tag.next();

         //cout << "tagname: " << tagname << endl;

         // TODO usar trim

         sc_tag.useSeparators(" =");
         while (sc_tag.hasNext()) {
            string at_name = sc_tag.next();
            //cout << "at_name: " << at_name << "\t";
            sc_tag.useSeparators("=\"");

            string at_value = sc_tag.next();
            //cout << "at_value: " << at_value << "\t";

            attr[at_name] = at_value;

            sc_tag.useSeparators("\" =");
         }
      }

      return make_pair(tagname, attr);
   }

public:
   // general hasNext methods

   bool hasNext() const noexcept
   {
      if (!hasNextChar())
         return false;

      istream* input = const_cast<istream*>(this->input);

      vector<char> buffer;

      char novo;
      if (!getChar(*input, novo))
         return false; // no next (and no exception!)

      bool next = true;

      while (inSeparators(novo)) {
         buffer.push_back(novo);

         if (!(hasNextChar())) {
            next = false;
            break;
         }

         if (!getChar(*input, novo))
            return false; // no next (and no exception!)
      }

      if (next) // ha proximo!
         input->putback(novo);

      //devolver o buffer ao IO
      while (buffer.size() > 0) {
         input->putback(buffer.at(buffer.size() - 1));
         buffer.erase(buffer.begin() + (buffer.size() - 1));
      }

      return next;
   }

   bool hasNextLine() const
   {
      return hasNext();
   }

   bool hasNextInt() const
   {
      int x;
      if (hasNextX(x)) {
         double d;
         if (hasNextX(d))
            return (x == d);
      }
      return false;
   }

   bool hasNextLong() const
   {
      long x;
      if (hasNextX(x)) {
         double d;
         if (hasNextX(d))
            return (x == d);
      }
      return false;
   }

   bool hasNextFloat() const
   {
      float x;
      return hasNextX(x);
   }

   bool hasNextDouble() const
   {
      double x;
      return hasNextX(x);
   }

private:
   // helper for 'hasNext' functions
   template<class X>
   inline bool hasNextX(X& x) const
   {
      string s = peekNext();

      if (s == "")
         return false;

      std::istringstream ss(s);

      return bool(ss >> x);
   }

public:
   // Returns the rest of the input as string
   std::string rest()
   {
      string backup_sep = sep;

      useSeparators("");

      string rest = "";

      if (hasNext())
         rest = next();

      sep = backup_sep;

      return rest;
   }
};

} // namespace scannerpp

#endif /*SCANNERPP_SCANNER_HPP_*/
