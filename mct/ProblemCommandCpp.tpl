#include "ProblemCommand.h"

using namespace Teste;

$projectProblemCommand::$projectProblemCommand()
{
    p = NULL;
}

$projectProblemCommand::~$projectProblemCommand()
{
    if(p)
        delete p;    
}
    
string $projectProblemCommand::id()
{
    string parentId = ProblemCommand<Rep$project , MY_ADS $commadproject>::id();
    parentId.append("$project"); // implements 'problem.project_name' module
    return parentId;
}
    
bool $projectProblemCommand::registerComponent(Component& component, string type, string name, HeuristicFactory<Rep$project , MY_ADS $commadproject>& hf, map<string, string>& dictionary)
{
    int idx = hf.addComponent(component, type);
    stringstream ss;
    ss << type << " " << idx;
    return defineText(name, ss.str(), dictionary);
}

bool $projectProblemCommand::load(string filename, HeuristicFactory<Rep$project , MY_ADS $commadproject>& hf, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
{
    File* file;

    try
    {
        file = new File(filename);
    }
    catch (FileNotFound& f)
    {
        cout << "File '" << filename <<"' not found" << endl;
        return false;
    }

    Scanner scanner(file);

    p = new ProblemInstance(scanner);

    // add everything to the HeuristicFactory 'hf'

    hf.addComponent(*new $projectEvaluator(*p));

    hf.addComponent(*new Constructive$constructive(*p));
    
    hf.addComponent(*new NSSeq$neighborhood);
		
    cout << "problem '" << filename << "' loaded successfully" << endl;
        
    return true;
}
    
bool $projectProblemCommand::unload(HeuristicFactory<Rep$project , MY_ADS $commadproject>& factory, map<string, string>& dictionary, map<string, vector<string> >& ldictionary)
{
    if(p)
        delete p;
    p = NULL;
       
    cout << "problem instance for $project unloaded successfully (use 'drop_all' if you want to remove all components)" << endl;
       
    return true;
}
