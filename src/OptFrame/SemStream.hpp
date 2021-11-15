
// This is a fundamental class for providing Semantic Streams on OptFrame
// The stream indicates its purpose, so formatting can change according to stream

#ifndef OPTFRAME_SEMSTREAM_HPP
#define OPTFRAME_SEMSTREAM_HPP

#include <iostream>

namespace optframe {

// semantic streams (optframe::cjson, ctxt, etc)
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

//extern SemStream cjson; // only in .cpp
//extern SemStream ctxt;  // only in .cpp
inline SemStream cjson{ std::cout }; // (C++17 extern linkage is implicit)
inline SemStream ctxt{ std::cout };  // (C++17 extern linkage is implicit)

}

#endif