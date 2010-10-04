#ifndef DICTIONARYMODULE_HPP_
#define DICTIONARYODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class M>
class DictionaryModule : public OptFrameModule<R,M>
{
public:
	string id()
	{
		return "dictionary";
	}
	string usage()
	{
		return "dictionary";
	}

	void run(vector<OptFrameModule<R,M>*> all_modules, HeuristicFactory<R,M>*, map<string,string>* dictionary, string)
	{
		cout << (*dictionary) << endl;
	}

};

#endif /* DICTIONARYMODULE_HPP_ */
