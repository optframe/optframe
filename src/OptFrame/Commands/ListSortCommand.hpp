// OptFrame - Optimization Framework

// Copyright (C) 2009-2015
// http://optframe.sourceforge.net/
//
// This file is part of the OptFrame optimization framework. This framework
// is free software; you can redistribute it and/or modify it under the
// terms of the GNU Lesser General Public License v3 as published by the
// Free Software Foundation.

// This framework is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License v3 for more details.

// You should have received a copy of the GNU Lesser General Public License v3
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
// USA.

#ifndef OPTFRAME_LIST_SORT_MODULE_HPP_
#define OPTFRAME_LIST_SORT_MODULE_HPP_

#include<string>

#include<algorithm>

#include "../Command.hpp"

#include "ListSilentDefineCommand.hpp"

namespace optframe
{

template<class R, class ADS = OPTFRAME_DEFAULT_ADS, class DS = OPTFRAME_DEFAULT_DS>
class ListSortCommand: public Command<R, ADS, DS>
{
public:

	virtual ~ListSortCommand()
	{
	}

	string id()
	{
		return "list.sort";
	}

	string usage()
	{
		return "list.sort list new_name [index]";
	}

	string doubleToString(double d)
	{
		stringstream ss;
		ss << d;
		return ss.str();
	}

	static bool compareLess(const pair<double, vector<string> >& a, const pair<double, vector<string> >& b)
	{
		return a.first < b.first;
	}

	static bool compareGreater(const pair<double, vector<string> >& a, const pair<double, vector<string> >& b)
	{
		return a.first > b.first;
	}

	bool run(vector<Command<R, ADS, DS>*>& all_modules, vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& factory, map<string, string>& dictionary,  map< string,vector<string> >& ldictionary, string input)
	{
		//cout << "list_sort module input: '" << input << "'" << endl;

		Scanner scanner(input);

		if (!scanner.hasNext())
		{
			cout << "list_sort command: no such list!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		vector<string>  list;
		vector<string>* p_list = OptFrameList::readList(ldictionary, scanner);
		if(p_list)
		{
			list = vector<string>(*p_list);
			delete p_list;
		}
		else
			return false;

		if (!scanner.hasNext())
		{
			cout << "list_sort command: no new list name!" << endl;
			cout << "Usage: " << usage() << endl;
			return false;
		}

		string new_name = scanner.next();

		int index = -1;

		if (scanner.hasNext())
		{
			try
			{
				index = scanner.nextInt();
			}
			catch(ConversionError& e)
			{
				cout << "list_sort command: not an index number!" << endl;
				return false;
			}
		}

		if((new_name != "") && (new_name.at(0)=='['))
		{
			cout << "list_sort command: invalid list new name '" << new_name << "'" << endl;
			return false;
		}

		if( (index-1) >= ((int)list.size()) )
		{
			cout << "list_sort command: invalid index '" << index << "'!" << endl;
			return false;
		}

		vector<string> new_list;

		if(index > 0) // sort list of lists by index
		{
			vector<pair<double, vector<string> > > v;

			for(unsigned i=0; i<list.size(); i++)
			{
				Scanner scan(list[i]);

				vector<string>  list1;
				vector<string>* p_list1 = OptFrameList::readList(ldictionary, scan);
				if(p_list1)
				{
					list1 = vector<string>(*p_list1);
					delete p_list1;
				}
				else
				{
					cout << "list_sort command: error processing internal list id=" << i << endl;
					return false;
				}

				vector<string> otherValues;
				double d = 0;

				for(unsigned j=0; j<list1.size(); j++)
					if(((int)j) != (index-1))
						otherValues.push_back(list1[j]);
					else
					{
						try
						{
							d = Scanner::parseDouble(list1[j]);
						}
						catch(ConversionError& e)
						{
							cout << "list_sort command: problem in conversion of index value in sublist!" << endl;
							return false;
						}
					}

				v.push_back(make_pair(d, otherValues));
			}


			std::sort(v.begin(), v.end(), compareLess); // only '<' for now!

			for(unsigned i=0; i<v.size(); i++)
			{
				vector<string> sublist(v[i].second.size()+1);

				double d = v[i].first;
				for(unsigned j=0; j<sublist.size(); j++)
					if(((int)j) == (index-1))
						sublist[j] = doubleToString(d);
					else if(((int)j) < (index-1))
						sublist[j] = v[i].second.at(j);
					else // j > (index-1)
						sublist[j] = v[i].second.at(j-1);

				new_list.push_back(OptFrameList::listToString(sublist));
			}
		}
		else // sort simple list
		{
			vector<double> v;

			try
			{
				for(unsigned i=0; i<list.size(); i++)
					v.push_back(Scanner::parseDouble(list[i]));
			}
			catch(ConversionError& e)
			{
				cout << "list_sort command: not a list of numbers!" << endl;
				return false;
			}

			std::sort(v.begin(), v.end());

			for(unsigned i=0; i<v.size(); i++)
				new_list.push_back(doubleToString(v[i]));
		}

		stringstream ss;
		ss << new_name << " " << OptFrameList::listToString(new_list);

		//TODO: acessar dicionario de listas diretamente!! Bem mais eficiente!

		return Command<R, ADS, DS>::run_module("list.silent_define", all_modules, allFunctions, factory, dictionary, ldictionary, ss.str());
	}

	virtual string* preprocess(vector<PreprocessFunction<R, ADS, DS>*>& allFunctions, HeuristicFactory<R, ADS, DS>& hf, const map<string, string>& dictionary, const map<string, vector<string> >& ldictionary, string input)
	{
		return Command<R, ADS, DS>::defaultPreprocess(allFunctions, hf, dictionary, ldictionary, input);
	}


};

}

#endif /* OPTFRAME_LIST_SORT_MODULE_HPP_ */
