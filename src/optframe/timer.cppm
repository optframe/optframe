module;

// C includes
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

// #include <stdlib.h>

export module optframe.timer;

import optframe.component;

export import std;

#define OPTFRAME_CXX_MODULES 1

#include "OptFrame/Timer.hpp"
