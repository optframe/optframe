#ifndef USAGEMODULE_HPP_
#define USAGEMODULE_HPP_

#include "../OptFrameTestModule.hpp"

template<class R, class M>
class UsageModule : public OptFrameTestModule<R,M>
{
public:
	string id()
	{
		return "usage";
	}
	string usage()
	{
		return "usage module_name";
	}

	void run(vector<OptFrameTestModule<R,M>*> all_modules, HeuristicFactory<R,M>*, map<string,string>* dictionary, string rest)
	{
		Scanner scanner(rest);

		string command = scanner.next();

		bool notfound = true;

		for(int i=0;i<all_modules.size();i++)
			if(command == all_modules[i]->id())
			{
				cout << "Usage: " << all_modules[i]->usage() << endl;
				notfound = false;
				break;
			}

		if(notfound)
			cout << "Module '"<<command<<"' not found!"<<endl;
	}

};

#endif /* USAGEMODULE_HPP_ */
