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

#include <OptFrame/Timer.hpp>
