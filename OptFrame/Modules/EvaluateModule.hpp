#ifndef EVALUATEMODULE_HPP_
#define EVALUATEMODULE_HPP_

#include "../OptFrameTestModule.hpp"

template<class R, class M>
class EvaluateModule : public OptFrameTestModule<R,M>
{
public:
	string id()
	{
		return "evaluate";
	}
	string usage()
	{
		string u = "evaluate ev id loadsol id";
		return u;
	}

	void run(vector<OptFrameTestModule<R,M>*> all_modules, HeuristicFactory<R,M>* factory, map<string,string>* dictionary, string input)
	{
		cout << "evaluate: "<<input<<endl;
		Scanner scanner(input);

		if(!scanner.hasNext())
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		Evaluator<R,M>* eval = factory->read_ev(&scanner);

		string sol = scanner.next();

		if(sol!="loadsol")
		{
			cout << "Second parameter must be a 'loadsol'!" << endl;
			cout << "Usage: "<<usage()<<endl;
			return;
		}

		string id = scanner.next();

		Scanner s2(sol+" "+id);
		Solution<R>* s = factory->read_loadsol(&s2);

		Evaluation<M>* e = eval->evaluate(s);
		e->print();

		delete e;
	}

};

#endif /* EVALUATEMODULE_HPP_ */
