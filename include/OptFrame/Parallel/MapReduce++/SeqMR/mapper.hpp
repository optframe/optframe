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

#ifndef MAPPER_HPP
#define MAPPER_HPP
#include <iterator>

///SeqMR_Mapper.
template<class KeyA, class A, class KeyB, class B, class C>
class SeqMR_Mapper : public Mapper<KeyA, A, KeyB, B, C>
{
public:
   ///Constructor.
   SeqMR_Mapper(MapReduce<KeyA, A, KeyB, B, C>* _mapReduce)
     : mapReduce(_mapReduce){};
   ///Iterator mapping execution (implemented by library).
#ifndef MRI_USE_MULTIMAP
   virtual std::vector<std::pair<KeyB, B>> run(std::vector<pair<KeyA, A>>& as)
   {
      std::vector<std::pair<KeyB, B>> bs;

      typename std::vector<std::pair<KeyA, A>>::iterator itAs;
      for (itAs = as.begin(); itAs != as.end(); ++itAs) {
         std::vector<std::pair<KeyB, B>> bss = map(*itAs);
         typename std::vector<std::pair<KeyB, B>>::iterator itBss;
         for (itBss = bss.begin(); itBss != bss.end(); ++itBss)
            bs.push_back(*itBss);
      }
      return bs;
   };
#else
   virtual multimap<KeyB, B> run(std::vector<pair<KeyA, A>>& as)
   {
      multimap<KeyB, B> bs;

      typename std::vector<std::pair<KeyA, A>>::iterator itAs;
      for (itAs = as.begin(); itAs != as.end(); ++itAs) {
         std::vector<std::pair<KeyB, B>> bss = map(*itAs);
         typename std::vector<std::pair<KeyB, B>>::iterator itBss;
         for (itBss = bss.begin(); itBss != bss.end(); ++itBss)
            bs.insert(*itBss);
      }
      return bs;
   };
#endif
   ///Map function (implemented by user).
   virtual std::vector<std::pair<KeyB, B>> map(pair<KeyA, A>) = 0;

protected:
   MapReduce<KeyA, A, KeyB, B, C>* mapReduce;
};

///SeqMR_StrMapper.
class SeqMR_StrMapper : public SeqMR_Mapper<string, string, string, string, string>
{
public:
   ///Constructor.
   SeqMR_StrMapper(MapReduce<string, string, string, string, string>* _mapReduce)
     : SeqMR_Mapper<string, string, string, string, string>(_mapReduce){};
};

#endif /* MAPPER_HPP */
