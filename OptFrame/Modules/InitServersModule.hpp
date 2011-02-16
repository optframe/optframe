#ifndef InitServersMODULE_HPP_
#define InitServersMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class M>
class InitServersModule : public OptFrameModule<R,M>
{
public:
	string id()
	{
		return "initservers";
	}
	string usage()
	{
		return "initservers";
	}

	void run(vector<OptFrameModule<R,M>*>, HeuristicFactory<R,M>* factory, map<string,string>* dictionary, string rest)
	{
		factory->mapReduce->initServers(factory->argc,factory->argv);
	}

	virtual string preprocess(map<string,string>*, string input)
	{
		Scanner scanner(input);

		// First, remove the comments '%'

		string input2 = "";

		while(scanner.hasNextChar())
		{
			char c = scanner.nextChar();
			if(c != '%')
				input2 += c;
			else
				break;
		}

		return input2;
	}

};

#endif /* InitServersMODULE_HPP_ */
