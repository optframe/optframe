#ifndef OPTFRAME_SEMSTREAM_HPP
#define OPTFRAME_SEMSTREAM_HPP

#include <iostream>

// This class provides "Semantic-Aware" Streams
// User can 'setStream' and redirect it to any other stream (default is 'cout')
// Some interesting usage can be found on 'printable.h' lib

namespace optframe {

struct SemStream : private std::streambuf
  , public std::ostream
{
public:
   std::ostream* os;

   SemStream(std::ostream& _os)
     : std::ostream{ this }
     , os{ &_os }
   {}

private:
   int overflow(int c) override
   {
      (*os).put(c);
      return 0;
   }

public:
   void setStream(std::ostream& _os)
   {
      os = &_os;
   }
};

// public/global instances (useful to global 'printable' instances, which are very useful...)
// TODO: is this bad or REALLY bad?
// if necessary, we can use some #DEFINE to disable this
//
// optframe semantic streams (json, raw "txt", ...)
//
SemStream cjson{ std::cout };
SemStream ctxt{ std::cout };

} // namespace optframe

#endif // OPTFRAME_SEMSTREAM_HPP