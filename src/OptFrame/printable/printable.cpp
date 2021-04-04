
//#include "printable.h"
#include <OptFrame/printable/printable.h> // boring...

#include <iostream>

// This class provides "Semantic-Aware" Streams
// User can 'setStream' and redirect it to any other stream (default is 'cout')
// Some interesting usage can be found on 'printable.h' lib

namespace optframe {

SemStream cjson{ std::cout };
SemStream ctxt{ std::cout };

} // namespace optframe
