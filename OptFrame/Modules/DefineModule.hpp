#ifndef DEFINEMODULE_HPP_
#define DEFINEMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class M>
class DefineModule : public OptFrameModule<R,M>
{
public:
	string id()
	{
		return "define";
	}
	string usage()
	{
		return "define new_name string_to_be_substituted_from_the_new_name";
	}

	void run(vector<OptFrameModule<R,M>*>, HeuristicFactory<R,M>*, map<string,string>* dictionary, string rest)
	{
		Scanner scanner(rest);

		string new_name = scanner.next();

		if(new_name != "")
		{
			string second_word = scanner.rest();

			Scanner s2(second_word);

			while(s2.hasNext())
				if(new_name==s2.next())
				{
					cout << "Recursive definitions are not allowed!" << endl;
					return;
				}

			(*dictionary)[new_name] = second_word;
			cout << "Word '"<<new_name<<"' now means:"<<(*dictionary)[new_name]<<endl;
		}
		else
			cout << "Usage: "<<usage()<<endl;
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

#endif /* DEFINEMODULE_HPP_ */
