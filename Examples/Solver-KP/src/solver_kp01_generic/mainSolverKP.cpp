// mainKP-fcore-ex.cpp
#include <fstream>
#include <iomanip> // setw
#include <iostream>
#include <nlohmann/json.hpp>

#include <solver_kp01_generic/solver_kp01_generic.h> // general solver

// import everything on main()
using namespace std;

class SolverKP01Generic
{
public:
   int runArgs(int argc, char** argv)
   {
      std::cout << "loading parameters" << std::endl;
      string local = argv[0];
      vector<string> params(argc);
      for (int i = 1; i < argc; i++) {
         params[i] = argv[i];
         if (params[i] == "--help") {
            std::cout << "help: solver_kp01_generic --config default_kp01.json" << std::endl;
            return 0;
         }
         if (params[i] == "--info") {
            info(1, ""); // info in 'json'
            return 0;
         }
      }

      string config_file;
      for (unsigned i = 0; i < params.size(); i++) {
         if (params[i] == "--config")
            config_file = params[i + 1];
      }

      if (config_file == "") {
         std::cout << "No run configuration. Try '--config', '--info' or '--help'. Aborting!" << std::endl;
         return 1;
      } else {
         std::cout << "loading file '" << config_file << "'" << std::endl;
         std::ifstream ijson(config_file);
         if (!ijson.is_open()) {
            std::cout << "file not found!" << std::endl;
            return 1;
         }
         nlohmann::json j;
         ijson >> j;

         std::stringstream ss;
         ss << std::setw(4) << j << std::endl;
         string config = ss.str();

         string sout = solve(1, config);
         // TODO: parse solve output
         std::cout << "output = '" << sout << "'" << std::endl;
         return 0;
      }

      return 0;
   }

   // format = 1 (json), = 0 (default)
   string solve(int format, string config)
   {
      string output(1000, '\0');

      solver_kp01_generic(format, config.c_str(), (char*)output.c_str());
      return output;
   }

   string info(int format, string key)
   {
      string output(1000, '\0');

      info_kp01_generic(format, key.c_str(), (char*)output.c_str());
      return output;
   }
};

int
main(int argc, char** argv)
{
   SolverKP01Generic solverKP;
   return solverKP.runArgs(argc, argv);
}