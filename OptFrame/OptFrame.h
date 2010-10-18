#ifndef OPTFRAME_H_
#define OPTFRAME_H_

#include <iostream>
#include <ostream>

#include <vector>
#include <map>

// =======================
// Duvida em templates? =)
// =======================
// http://www.vivaolinux.com.br/artigo/Compilando-Templates-C++/?pagina=1

// ==================================
//               Util
// ==================================

#include "Util/printable.h"
#include "Util/Matrix.hpp"
#include "Util/Scanner++/Scanner.h"
#include "Util/Timer.hpp"


// ==================================
//        Solution Interface
// ==================================

#include "Solution.h"

#include "Solution/CleanSolution.hpp"
#include "Solution/TestSolution.hpp"

// ==================================
//     Initial Solution Interface
// ==================================

#include "InitialSolution.h"


// ==================================
//               Moves
// ==================================

#include "Move.hpp"

// ==================================
//            Evaluation
// ==================================

#include "Evaluation.hpp"
#include "Evaluator.hpp"
#include "GenericEvaluator.hpp"


// ==================================
//       Neighborhood Structure
// ==================================

#include "NS.h"
#include "NSSeq.hpp"
#include "NSEnum.hpp"
#include "MRNS.hpp"

// ==================================
//           Util NS
// ==================================

#include "Util/NSEnumVectorShift.hpp"
#include "Util/NSSeqVVSwapIntra.hpp"

#include "Util/NSEnumVVShiftk.hpp"
#include "Util/NSEnumVVSwapk.hpp"

// ==================================
//            Heuristic
// ==================================

#include "Heuristic.hpp"
#include "HeuristicFactory.hpp"

// ==================================

#include "OptFrameTestModule.hpp"

#include "Modules/CheckModule.hpp"
#include "Modules/DefineModule.hpp"
#include "Modules/DictionaryModule.hpp"
#include "Modules/EmpiricalModule.hpp"
#include "Modules/EvaluateModule.hpp"
#include "Modules/ExecModule.hpp"
#include "Modules/HelpModule.hpp"
#include "Modules/PrintModule.hpp"
#include "Modules/ReadModule.hpp"
#include "Modules/TestModule.hpp"
#include "Modules/UsageModule.hpp"

// ==================================
//        Parallel Support
// ==================================

//#include "MaPI/MaPI.h"
//#include "MRNSEnum.hpp"


// ==================================
//            Serializer
// ==================================

//#include "Serializer/Serializer.hpp"


// ==================================


template<class R, class M>
class OptFrame
{
private:
	vector<OptFrameTestModule<R,M>*> modules;
	HeuristicFactory<R,M>* factory;
	map<string,string>* dictionary;
public:

	OptFrame(HeuristicFactory<R,M>* f)
	{
		loadDefaultModules();
		factory=f;
		dictionary = new map<string,string>;
	};

	string version()
	{
		return "OptFrame - Version 0.5";
	}

	void loadModule(OptFrameTestModule<R,M>* module)
	{
		modules.push_back(module);
	}

	void loadDefaultModules()
	{
		modules.clear();
		loadModule(new CheckModule<R,M>);
		loadModule(new DefineModule<R,M>);
		loadModule(new DictionaryModule<R,M>);
		loadModule(new EmpiricalModule<R,M>);
		loadModule(new EvaluateModule<R,M>);
		loadModule(new ExecModule<R,M>);
		loadModule(new HelpModule<R,M>);
		loadModule(new PrintModule<R,M>);
		loadModule(new ReadModule<R,M>);
		loadModule(new TestModule<R,M>);
		loadModule(new UsageModule<R,M>);
	}

	void execute()
	{
		cout << "Welcome to "<< version() << endl;

		Scanner scanner(&cin);
		string line;

		while(true)
		{
			cout << ">";
			line = scanner.nextLine();

			Scanner s2(line);

			string command = s2.next();

			if(command=="quit" || command=="q" || command=="exit")
				break;

			if(command=="version" || command=="v")
			{
				cout << version() << endl;
				continue;
			}

			bool notfound = true;

			for(int i=0;i<modules.size();i++)
				if(command==modules[i]->id())
				{
					string r = modules[i]->preprocess(dictionary,s2.rest());

					modules[i]->run(modules,factory,dictionary,r);
					notfound = false;
					break;
				}

			if(notfound)
				cout << "Sorry, i couldn't understand the command '"<< command <<"'."<< endl <<"Please, type 'help' or type the command again." << endl;
		}

		cout << "Goodbye." << endl;
	};

	void execute(string line)
	{
		cout << "Welcome to "<< version() << endl;

		Scanner s2(line);

		string command = s2.next();

		if(command=="quit" || command=="q" || command=="exit")
			return;

		if(command=="version" || command=="v")
		{
			cout << version() << endl;
			return;
		}

		bool notfound = true;

		for(int i=0;i<modules.size();i++)
			if(command==modules[i]->id())
			{
				string r = modules[i]->preprocess(dictionary,s2.rest());

				modules[i]->run(modules,factory,dictionary,r);
				notfound = false;
				break;
			}

		if(notfound)
			cout << "Sorry, i couldn't understand the command '"<< command <<"'."<< endl <<"Please, type 'help' or type the command again." << endl;

		cout << "Goodbye." << endl;
	};

};

#endif /*OPTFRAME_H_*/
