#ifndef PRINTMODULE_HPP_
#define PRINTMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class M>
class PrintModule : public OptFrameModule<R,M>
{
public:
	string id()
	{
		return "print";
	}
	string usage()
	{
		string u = "print loadsol id";

		return u;
	}

	void run(vector<OptFrameModule<R,M>*> all_modules, HeuristicFactory<R,M>* factory, map<string,string>* dictionary, string input)
	{
		cout << "print: "<<input<<endl;
		Scanner scanner(input);

		if(!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		string sol = scanner.next();

		if(sol!="loadsol")
		{
			cout << "First parameter must be a 'loadsol'!" << endl;
			cout << "Usage: "<<usage()<<endl;
			return;
		}

		string id = scanner.next();

		Scanner s2(sol+" "+id);
		Solution<R>* s = factory->read_loadsol(&s2);
		s->print();
	}

};

#endif /* PRINTMODULE_HPP_ */
