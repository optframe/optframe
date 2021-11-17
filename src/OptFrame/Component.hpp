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

#ifndef OPTFRAME_COMPONENT_HPP_
#define OPTFRAME_COMPONENT_HPP_

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <OptFrame/SemStream.hpp>

namespace optframe {

//using std::string = std::string;

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

	void append(std::string s)
	{
		(*data) << s;
	}

	string log()
	{
		return data->str();
	}

	bool toFile(std::string file, bool append = true)
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

enum StringFormat
{
   Undefined = 0, // undefined... typically 'Human'
   Human = 1,     // human readable
   NoSpaces = 2,  // raw: no spaces (undescore is allowed)
   JSON = 3       // json format
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

   static std::string idComponent()
   {
      return "OptFrame";
   }

   virtual std::string id() const
   {
      return idComponent();
   }

   virtual bool compatible(std::string s)
   {
      return (s == id()) || (s == idComponent()); // equal to this component or "OptFrame:" base
   }

   //StringFormat mlogType{ StringFormat::Human };

   // returns 'false' if unsupported
   virtual bool toStream(std::ostream& os) const
   {
      return false;
   }

   // returns "" if unsupported
   virtual std::string toString() const
   {
      return id();
   }

   /*
   // returns "" if unsupported
   virtual std::string toStringFormat(StringFormat fmt) const
   {
      return "";
   }
*/
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
