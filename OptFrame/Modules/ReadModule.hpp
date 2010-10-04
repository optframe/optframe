#ifndef READMODULE_HPP_
#define READMODULE_HPP_

#include "../OptFrameModule.hpp"

template<class R, class M>
class ReadModule : public OptFrameModule<R,M>
{
public:
	string id()
	{
		return "read";
	}
	string usage()
	{
		return "read filename";
	}

	void run(vector<OptFrameModule<R,M>*> all_modules, HeuristicFactory<R,M>* factory, map<string,string>* dictionary, string input)
	{
		Scanner trim(input);
		if(!trim.hasNext()) // no file
		{
			cout << "Usage: " << usage() << endl;
			return;
		}

		// Open file
		Scanner* scanner;

		try
		{
			scanner = new Scanner(new File(input));
		}
		catch (FileNotFound e)
		{
			cout << "File '"<< e.getFile() << "' not found!" << endl;
			cout << "Usage: " << usage() << endl;
			return;
		}

		while(scanner->hasNext())
		{
			string line = scanner->nextLine();

			line = OptFrameModule<R,M>::defaultPreprocess(dictionary,line);

			Scanner s2(line);

			if(!s2.hasNext()) // no command found in the line
				continue;

			string command = s2.next();

			bool notfound = true;

			for(int i=0;i<all_modules.size();i++)
				if(command == all_modules[i]->id())
				{
					all_modules[i]->run(all_modules, factory, dictionary, s2.rest());
					notfound = false;
					break;
				}

			if(notfound)
				cout << "Warning: command '"<<command<<"' not found!"<<endl;
		}

		delete scanner;
	}

};

#endif /* READMODULE_HPP_ */
