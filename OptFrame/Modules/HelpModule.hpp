#ifndef HELPMODULE_HPP_
#define HELPMODULE_HPP_

#include "../OptFrameTestModule.hpp"

template<class R, class M>
class HelpModule : public OptFrameTestModule<R,M>
{
public:
	string id()
	{
		return "help";
	}
	string usage()
	{
		return "help";
	}

	void run(vector<OptFrameTestModule<R,M>*> all_modules, HeuristicFactory<R,M>*, map<string,string>* dictionary, string)
	{
		cout << "Available modules are:" << endl;
		for(int i=0;i<all_modules.size();i++)
			cout << all_modules[i]->id() << endl;
		cout << "Type 'usage module_name' to learn how to use the module." << endl;
	}

};

#endif /* HELPMODULE_HPP_ */
