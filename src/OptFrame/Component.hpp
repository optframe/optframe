// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_COMPONENT_HPP_
#define OPTFRAME_COMPONENT_HPP_

#include <cstdlib>
#include <iostream>

namespace optframe {

using string = std::string;

// OPTFRAME ALGORITHM COMPREHENSION FLAG
#ifndef OPTFRAME_FAST
#define OPTFRAME_AC
#endif

/*
struct Log
{
	stringstream data;
   //ostream* data{&std::cout};
   //bool must_free {false};

	void clear()
	{
		data->clear();
	}

	void append(string s)
	{
		(*data) << s;
	}

	string log()
	{
		return data->str();
	}

	bool toFile(string file, bool append = true)
	{
		FILE* f;
		if(append)
			f = fopen(file.c_str(), "a");
		else
			f = fopen(file.c_str(), "w");

		if(!f)
			return false;

		fprintf(f, "%s", data.str().c_str());

		fclose(f);

		return true;
	}
};
*/

// |      1      |      2      |      3      |      4      |
// |    error    |   warning   | information | debug/verb. |

enum LogLevel
{
   Silent = 0,
   Error = 1,
   Warning = 2,
   Info = 3,
   Debug = 4
};

class Component
{
public:
   //Log* logdata;
   //
   // 'logdata' is for User logs
   std::ostream* logdata{ &std::cout };
   // 'mlog' is for machine logs (disabled by default)
   std::ostream* mlog{ nullptr };

   static bool safe_delete(Component* c)
   {
      if (c) {
         delete c;
         return true;
      } else
         return false;
   }

   static void safe_print(Component* c)
   {
      //assert(c);
      if (c)
         c->print();
      else
         std::cout << "nullptr Component" << std::endl;
   }

public:
   // Set user log stream recursive: must be implemented on each component. Returns 'false' is not implemented.
   virtual bool setLogR(std::ostream* _logdata)
   {
      this->logdata = _logdata;
      return false;
   }

   // Set machine log recursive: must be implemented on each component. Returns 'false' is not implemented.
   virtual bool setMachineLogR(std::ostream* _mlog)
   {
      this->mlog = _mlog;
      return false;
   }

   /*
	void initializeLog()
	{
		logdata = new Log;
	}

	void destroyLog()
	{
		if(logdata)
		{
			delete logdata;
			logdata = nullptr;
		}
	}
*/
   int verboseLevel;

   bool error;
   bool warning;
   bool information;
   // debug and verbose should be "almost the same"
#ifdef NDEBUG
   constexpr static bool debug{ false };
#else
   bool debug; // perform dead code elimination, having always debug{true} as constexpr, when NDEBUG is ON
#endif
   bool verbose; // no dead code elimination, even when NDEBUG is ON

   // Mode: SILENT  = 0
   // |      1      |      2      |      3      |      4      |
   // |    error    |   warning   | information | debug/verb. |
   // |      -      |      -      |      -      |      -      |

   // Mode: ERROR = 1
   // |      1      |      2      |      3      |      4      |
   // |    error    |   warning   | information | debug/verb. |
   // |      x      |      -      |      -      |      -      |

   // Mode: WARNING = 2 (DEFAULT)
   // |      1      |      2      |      3      |      4      |
   // |    error    |   warning   | information | debug/verb. |
   // |      x      |      x      |      -      |      -      |

   // Mode: INFORMATION = 3
   // |      1      |      2      |      3      |      4      |
   // |    error    |   warning   | information | debug/verb. |
   // |      x      |      x      |      x      |      -      |

   // Mode: VERBOSE/DEBUG = 4
   // |      1      |      2      |      3      |      4      |
   // |    error    |   warning   | information | debug/verb. |
   // |      x      |      x      |      x      |      x      |

   Component()
   {
      // TODO: create 'logless' implementation on OptFrame (is it faster?)
      setMessageLevel(LogLevel::Info);
      //logdata = nullptr;
   }

   virtual ~Component()
   {
   }

   static string idComponent()
   {
      return "OptFrame";
   }

   virtual string id() const
   {
      return idComponent();
   }

   virtual bool compatible(string s)
   {
      return (s == id()) || (s == idComponent()); // equal to this component or "OptFrame:" base
   }

   virtual string toString() const
   {
      return id();
   }

   virtual void print() const
   {
      (*logdata) << this->toString() << std::endl;
   }

   // -----------

   void setVerbose()
   {
      setMessageLevel(LogLevel::Debug);
   }

   // set verbose level recursive: returns 'false' if not supported.
   virtual bool setVerboseR()
   {
      this->setVerbose();
      return false;
   }

   // -----------

   // set silent level recursive: returns 'false' if not supported.
   virtual void setSilent()
   {
      setMessageLevel(LogLevel::Silent);
   }

   // set silent level recursive: returns 'false' if not supported.
   virtual bool setSilentR()
   {
      this->setSilent();
      return false;
   }

   // -----------

   void setMessageLevel(LogLevel vl)
   {
      error = warning = information = verbose = false;
#ifndef NDEBUG
      debug = false;
#endif

      verboseLevel = vl;
      switch (verboseLevel) {
         case LogLevel::Silent: // SILENT 0
            break;
         case LogLevel::Error: // ERROR 1
            error = true;
            break;
         case LogLevel::Warning: // WARNING 2
            warning = true;
            break;
         case LogLevel::Debug: // VERBOSE 4
#ifdef NDEBUG
            std::cerr << "WARNING: LogLevel::Debug is disabled due to NDEBUG flag!" << std::endl;
#else
            debug = true;
#endif
            break;
         default: // 3: INFORMATION (default)
            information = true;
      }

      verbose = debug; // always the same
      error = error || warning || information || debug;
      warning = warning || information || debug;
      information = information || debug;
   }

   bool getVerboseLevel()
   {
      return verboseLevel;
   }
};

} // namespace optframe

#endif /* OPTFRAME_COMPONENT_HPP_ */
