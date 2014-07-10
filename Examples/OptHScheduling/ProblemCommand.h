// OptFrame - Optimization Framework

// Copyright (C) 2009, 2010, 2011
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

#ifndef OptHS_PROBLEMMODULE_HPP_
#define OptHS_PROBLEMMODULE_HPP_

#include <iostream>

#include "../../OptFrame/Scanner++/Scanner.h"

#include "Representation.h"
#include "Memory.h"
#include "Solution.h"
#include "Evaluation.h"

#include "ProblemInstance.h"

#include "Evaluator.h"

#include "ConstructiveRandom.h"
#include "NSSwap.h"

#include "../../OptFrame/HeuristicFactory.hpp"

using namespace scannerpp;
using namespace optframe;

namespace OptHS
{

class OptHSProblemCommand
{
public:

    ProblemInstance* p;
    
    OptHSProblemCommand()
    {
        p = NULL;
    }

    virtual ~OptHSProblemCommand()
    {
        if(p)
           delete p;    
    }

    string id()
    {
    	return "problem.OptHS";
    }

    bool registerComponent(Component& component, string type, string name, HeuristicFactory<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>& hf, map<string, string>& dictionary)
    {
       int idx = hf.addComponent(component, type);
       stringstream ss;
       ss << type << " " << idx;
       return true; //defineText(name, ss.str(), dictionary);
    }

	bool load(string members, HeuristicFactory<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
	{
	    Scanner scanner(members);

        p = new ProblemInstance(scanner);

        // add everything to the HeuristicFactory 'hf'

        ConstructiveRandom& is = * new ConstructiveRandom(*p, hf.getRandGen());
        hf.addComponent(is);

        OptHSEvaluator& eval = *new OptHSEvaluator(*p);
        hf.addComponent(eval, Evaluator<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>::idComponent());
		
        NSSwap* ns = new NSSwap(*p, hf.getRandGen());
        hf.addComponent(*ns);

        cout << "problem '" << members << "' loaded successfully" << endl;
        
        return true;
    }
    
    bool unload(HeuristicFactory<RepOptHS, OPTFRAME_DEFAULT_ADS, MemOptHS>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
    {
       if(p)
          delete p;
       p = NULL;
       
       cout << "problem instance for OptHS unloaded successfully (use 'drop_all' if you want to remove all components)" << endl;

       return true;
    }
};

}

#endif /*OptHS_PROBLEMMODULE_HPP_*/

