// OptFrame 4.2 - Optimization Framework
// Copyright (C) 2009-2021 - MIT LICENSE
// https://github.com/optframe/optframe
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef OPTFRAME_FUNCTION_CREATE_RAW_MODULE_HPP_
#define OPTFRAME_FUNCTION_CREATE_RAW_MODULE_HPP_

#include <string>

#include "../Command.hpp"
#include "../PreprocessFunction.hpp"

namespace optframe {

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class RawFunction : public PreprocessFunction<R, ADS, DS> {
  string name;
  vector<string> parameters;
  string body;

 public:
  RawFunction(string _name, vector<string>& _parameters, string& _body) {
    name = _name;
    parameters = _parameters;
    body = _body;
  }

 private:
  string var_preprocess(string var, string value, string command) {
    if (command.length() < var.length()) {
      // cout << "general module var_preprocess: command='" << command << "'
      // var='" << var << "'" << std::endl; cout << "general module
      // var_preprocess: no possible variable because command is smaller!" <<
      // std::endl;
      return command;  // no possible variable!
    }

    // cout << "general module var_preprocess: command='" << command << "'
    // var='" << var << "' value='" << value << "'" << std::endl;

    string new_command = "";
    string rest = "";
    int dollar_pos = -1;

    for (unsigned int i = 0; i < command.length(); i++)
      if (command[i] == '$') {
        dollar_pos = i;  // first dollar position
        break;
      }

    if (dollar_pos == -1)  // dollar not found
      return command;

    if (((int)command.size()) == (dollar_pos + 1))  // just one dollar
    {
      std::cout << "raw function warning: just one buck!" << std::endl;
      return command;
    }

    if (((dollar_pos + 1) < ((int)command.size())) &&
        (command[dollar_pos + 1] == '{'))  // var with brackets
    {
      // verifying if there is a close bracket for this var
      if (((dollar_pos + ((int)var.length()) + 1) < ((int)command.size())) &&
          (command[dollar_pos + var.length() + 1] == '}')) {
        // verifying if the found var is the same parameter var
        bool test_var = true;
        for (unsigned int i = 1; i < var.length();
             i++)  // ignoring the $ in the var (i = 1)
        {
          if (var[i] != command[dollar_pos + 1 + i]) {
            test_var = false;
            break;
          }
        }
        if (test_var)  // correctly variable found
        {
          // replacing variable
          new_command.append(
              command, 0, dollar_pos);  // first part of command without bracket
          new_command.append(value);    // value of variable
          rest.append(command.begin() + dollar_pos + var.length() + 2,
                      command.end());  // rest of command
          // verifying if have the same var in the rest of command
          rest = var_preprocess(var, value, rest);

          new_command.append(rest);
        } else  // can be another variable
        {
          // dont replace the variable
          new_command.append(command, 0, dollar_pos + var.length() + 2);

          rest.append(command.begin() + dollar_pos + var.length() + 2,
                      command.end());  // rest of command

          // verifying if have the var in the rest of command
          rest = var_preprocess(var, value, rest);

          new_command.append(rest);
        }

      } else  // can be another variable
      {
        // ignoring the variable found and continue finding for the parameter
        // variable in the rest of command
        new_command.append(command, 0, dollar_pos + 2);
        rest.append(command.begin() + dollar_pos + 2, command.end());

        rest = var_preprocess(var, value, rest);

        new_command.append(rest);
      }
    } else  // variable without brackets
      if ((dollar_pos + ((int)var.length())) < ((int)command.size())) {
        if (isalnum(command[dollar_pos + var.length()]) ||
            (command[dollar_pos + var.length()] ==
             '_'))  // don't have the same length, can be another variable
        {
          new_command.append(command, 0, dollar_pos + 1);
          // finding for the parameter variable in the rest of command
          rest.append(command.begin() + dollar_pos + 1, command.end());

          rest = var_preprocess(var, value, rest);

          new_command.append(rest);
        } else  // have the same length
        {
          // verifying if the found var is the same parameter var
          bool test_var = true;
          for (unsigned int i = 1; i < var.length();
               i++)  // ignoring the $ in the var (i = 1)
          {
            if (var[i] != command[dollar_pos + i]) {
              test_var = false;
              break;
            }
          }

          if (test_var)  // correctly variable found
          {
            // replacing variable
            new_command.append(
                command, 0,
                dollar_pos);            // first part of command without dollar
            new_command.append(value);  // value of variable
            rest.append(command.begin() + dollar_pos + var.length(),
                        command.end());  // rest of command
            // verifying if have the same var in the rest of command
            rest = var_preprocess(var, value, rest);

            new_command.append(rest);
          } else  // can be another variable
          {
            // dont replace the variable
            new_command.append(command, 0, dollar_pos + var.length());

            rest.append(command.begin() + dollar_pos + var.length(),
                        command.end());  // rest of command

            // verifying if have the var in the rest of command
            rest = var_preprocess(var, value, rest);

            new_command.append(rest);
          }
        }
      }

    if (new_command == "")
      return command;
    else
      return new_command;
  }

 public:
  virtual ~RawFunction() {}

  string id() { return name; }

  string usage() {
    string u = name;
    u.append(" ");
    for (unsigned int i = 0; i < parameters.size(); i++) {
      u.append(parameters[i]);
      u.append(" ");
    }

    return u;
  }

  string* run(
      std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
      HeuristicFactory<R, ADS, DS>& hf,
      const std::map<std::string, std::string>& dictionary,
      const std::map<std::string, std::vector<std::string>>& ldictionary,
      string input) {
    // CHECK IF EXPLICIT LIST IS PASSED AS PARAMETER (CAN'T DO THIS!!!) TODO: I
    // DONT KNOW WHY =(
    for (unsigned i = 0; i < input.size(); i++)
      if (input.at(i) == '[') {
        std::cout << "raw_function '" << id()
                  << "' error: can't have explicit list as parameter, sorry! "
                     "Please use 'list.silent_define' before calling this!"
                  << std::endl;
        return nullptr;
      }

    string backup_input = input;

    Scanner scanner(input);

    // cout << "raw_function '" << id() << "' run: '" << input << "'" <<
    // std::endl;

    vector<string> values;
    values.push_back(backup_input);  // "$_all_params"

    unsigned nsystem_params = values.size();

    for (unsigned int i = 0; i < (parameters.size() - nsystem_params); i++)
      if (!scanner.hasNext()) {
        std::cout << "Usage: " << usage() << std::endl;
        return nullptr;
      } else
        values.push_back(scanner.next());

    // cout << "FUNCTION '" << id() << "' (CREATED) VALUES: '" << values << "'"
    // << std::endl;

    stringstream input_body;  // add spaces before and after '(', ')', '[', ']',
                              // '{', '}', ';' and ','
    for (unsigned i = 0; i < body.size(); i++)
      if ((body.at(i) == '(') || (body.at(i) == ')') || (body.at(i) == '[') ||
          (body.at(i) == ']') || (body.at(i) == ','))
        input_body << ' ' << body.at(i) << ' ';
      else
        input_body << body.at(i);

    string command = input_body.str();
    command.append(" ");  // TODO: why we need this to find variable in the end?

    // cout << "FUNCTION '" << id() << "' (CREATED) COMMAND: '" << command <<
    // "'" << std::endl;

    for (unsigned int v = 0; v < values.size(); v++)
      command = var_preprocess(parameters[v], values[v], command);

    // cout << "FUNCTION '" << id() << "' (CREATED) BODY (after var_prep): '" <<
    // command << "'" << std::endl;

    Scanner scanBody(Scanner::trim(command));

    if (!scanBody.hasNext()) return new string("");

    string nameOrValue = scanBody.next();

    if (!scanBody.hasNext()) return new string(nameOrValue);  // it was value!

    string brackets = scanBody.next();

    if ((brackets == "(") &&
        PreprocessFunction<R, ADS, DS>::functionExists(
            nameOrValue, allFunctions))  // found a function!
    {
      pair<std::string, std::string>* p =
          PreprocessFunction<R, ADS, DS>::run_function(
              nameOrValue, allFunctions, hf, dictionary, ldictionary,
              scanBody.rest());

      if (p) {
        string value = Scanner::trim(p->first);
        string rest = Scanner::trim(p->second);

        delete p;

        // DO NOT WARN REST!
        // if(rest != "")
        //	cout << "raw_function (" << name << ") warning: rest is '" <<
        // rest << "'" << std::endl;

        return new string(value);
      } else {
        std::cout << "preprocessing error in raw_function '" << name << "'"
                  << std::endl;
        return nullptr;  // error in valid function!
      }
    } else {
      std::stringstream ss;
      ss << nameOrValue << scanBody.getDiscarded() << brackets;
      ss << scanBody.rest();

      return new string(ss.str());
    }
  }

  // FUNCTIONS ALWAYS NEED DEFAULT PREPROCESSING!!
  /*
       virtual string* preprocess(std::vector<PreprocessFunction<R, ADS, DS>*>&,
     std::map<std::string, std::string>&,  map< string,vector<string> >&, string
     input)
       {
               // disable preprocess!!
               return new string(input);
       }
       */
};

template <class R, class ADS = OPTFRAME_DEFAULT_ADS,
          class DS = OPTFRAME_DEFAULT_DS>
class FunctionCreateRawCommand : public Command<R, ADS, DS> {
 private:
  bool functionExists(string functionName,
                      vector<PreprocessFunction<R, ADS, DS>*>& allFunctions) {
    for (unsigned i = 0; i < allFunctions.size(); i++)
      if (allFunctions[i]->id() == functionName) return true;
    return false;
  }

 public:
  virtual ~FunctionCreateRawCommand() {}

  string id() { return "function.create_raw"; }

  string usage() {
    return "function.create_raw name list_of_$parameters = body_of_function";
  }

  bool run(std::vector<Command<R, ADS, DS>*>& modules,
           vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
           HeuristicFactory<R, ADS, DS>& factory,
           std::map<std::string, std::string>& dictionary,
           std::map<std::string, std::vector<std::string>>& ldictionary,
           string input) {
    Scanner scanner(input);
    // cout << "create_module run: '" << input << "'" << std::endl;

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string name = scanner.next();

    if (functionExists(name, allFunctions)) {
      std::cout << "function.create_raw command: couldn't create function '"
                << name << "' because it already exists!" << std::endl;
      return false;
    }

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    vector<string>* plist1 = OptFrameList::readList(ldictionary, scanner);
    vector<string> parameters;
    if (plist1) {
      parameters = vector<string>(*plist1);
      vector<string> system_params;
      string name_all_params = "$_all_params_";  // + FUNCTION_NAME
      name_all_params.append(name);
      system_params.push_back(name_all_params);
      parameters.insert(parameters.begin(), system_params.begin(),
                        system_params.end());
      delete plist1;
    } else
      return false;

    for (unsigned int i = 0; i < parameters.size(); i++)
      if (parameters[i][0] != '$') {
        std::cout << "Missing operator $ in variable: " << parameters[i]
                  << std::endl;
        return false;
      }

    if (!scanner.hasNext()) {
      std::cout << "Usage: " << usage() << std::endl;
      return false;
    }

    string eq = scanner.next();
    if (eq != "=") {
      std::cout << "function.create_raw error: expected '=' and found '" << eq
                << std::endl;
      return false;
    }

    string body = Scanner::trim(scanner.rest());

    allFunctions.push_back(new RawFunction<R, ADS, DS>(name, parameters, body));

    // cout << "raw_function '" << name << "' loaded with parameters '" <<
    // parameters << "' and body: '" << body << "'" << std::endl;

    return true;
  }

  virtual string* preprocess(
      std::vector<PreprocessFunction<R, ADS, DS>*>& allFunctions,
      HeuristicFactory<R, ADS, DS>& hf,
      const std::map<std::string, std::string>& dictionary,
      const std::map<std::string, std::vector<std::string>>& ldictionary,
      string input) {
    // disable preprocess!!
    return new string(input);
  }
};

}  // namespace optframe

#endif /* OPTFRAME_FUNCTION_CREATE_RAW_MODULE_HPP_ */
