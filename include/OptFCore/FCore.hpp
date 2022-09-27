#ifndef OPTFRAME_FCORE_HPP
#define OPTFRAME_FCORE_HPP

// Functional Core Module: inclusion of "Functional OptFrame Core" components

#include "FConstructive.hpp"
#include "FEvaluator.hpp"
#include "FMove.hpp"
#include "FNS.hpp"
#include "FNSSeq.hpp"
//
#include "FDecoderRK.hpp" // Extra

namespace optframe {

// OptFrame Functional Core (FCore)
class FCore
{
public:
   static std::string version()
   {
      return "5.0-dev";
   }

   static std::string welcome()
   {
      return "Welcome to OptFrame Functional Core (FCore) - version " + version();
   }
};

} // optframe

#endif // OPTFRAME_FCORE_HPP