#include <solver_kp01_generic/solver_kp01_generic.h>

// this is C++ file

#include <iostream>

#include <nlohmann/json.hpp>

#include <solver_kp01_generic/solver_kp01.hpp> // implementation

// import everything on main()
using namespace std;
using namespace optframe;
using namespace scannerpp;
using namespace solver_kp01;

int
solver_kp01_generic(int format, const char* config, char* output)
{
   string sconfig(config);
   nlohmann::json j = nlohmann::json::parse(sconfig);

   std::cout << "will run solver kp01 generic" << std::endl;

   //std::cout << "JSON: '" << j << "'" << std::endl;
   string desc = j["description"];
   std::cout << "DESCRIPTION: '" << desc << "'" << std::endl;

   string problem_file = j["problem_file"];
   long seed = j["seed"];
   std::cout << "run config SEED -> " << seed << std::endl;
   srand(seed);

   double tlimit = j["timelimit"];

   nlohmann::json jstrategy = j["strategy"];
   std::cout << "STRATEGY: '" << jstrategy["name"] << "'" << std::endl;
   double alpha = jstrategy["alpha"];
   int iterT = jstrategy["iterT"];
   double Tmax = jstrategy["Tmax"];

   std::cout << "======== Carregando Problema ========" << std::endl;
   // semente pseudo-aleatória fixa em zero
   //srand(time(NULL));

   File f{ problem_file };

   if (!f.isOpen()) {
      std::cerr << "Problema '" << problem_file << "' não encontrado no diretório!" << std::endl;
      return 1;
   }

   Scanner scanner{ std::move(f) };

   pKP.load(scanner);
   std::cout << "número de elementos na mochila:" << pKP.n << std::endl;

   std::cout << "======== Testa Construtivo Aleatório ========" << std::endl;
   // invoca método 'generateSolution' do FCore 'FConstructive' para construtivo aleatório
   std::vector<bool> sol = *construtivoAleatorio.generateSolution(0.0);
   // imprime solução inicial
   std::cout << sol << std::endl;
   //
   std::cout << "======== Testa Avaliador ========" << std::endl;
   // avalia solução inicial e cria um par 'ESolution'
   ESolutionKP esol(sol, avaliador.evaluate(sol));
   // imprime avaliação da solução inicial
   esol.second.print();

   std::cout << "======== Executa Simulated Annealing ========" << std::endl;
   // Especifica um gerador aleatório para o Simulated Annealing
   RandGen rg;
   rg.setSeed(seed);

   //
   // Cria objeto da classe 'InitialSearch' (parecido com 'construtivoAleatório')
   BasicInitialSearch<ESolutionKP> initRand(construtivoAleatorio, avaliador);
   // Instancia um Simulated Annealing com alpha=98%, iterações na temp = 100, temperatura inicial = 99999
   BasicSimulatedAnnealing<ESolutionKP> sa{
      avaliador, initRand, nsFlip, alpha, iterT, Tmax, rg
   };

   Timer spentTime;
   // executa o SA e coleta o 'status' de saída
   // passa um 'Criterio de Parada' por tempo (= 10 segundos)
   auto status = sa.search({ tlimit }); // 10.0 seconds max
   // pega melhor solução do método SA
   ESolutionKP melhor = *status.best; //*sa.getBestSolution();
   std::cout << "======== Imprime melhor solução do SA ========" << std::endl;
   // imprime representação da melhor solução
   cout << melhor.first << endl;
   // imprime avaliação da melhor solução
   melhor.second.print();

   std::cout << "======== Fim da Execução ========" << std::endl;

   nlohmann::json joutput;
   joutput["time_spent"] = spentTime.now();
   joutput["status"] = status.status;
   joutput["best_value"] = status.best->second.evaluation();
   strcpy(output, joutput.dump().c_str());
   //output = "";

   return 0; // OK
}

int
info_kp01_generic(int format, const char* key, char* output)
{
   // considers 'format'
   // 0: plaintext
   // 1: json

   // only accepting 'json'
   if (format != 1) {
      //output = "";
      return 1;
   }

   nlohmann::json joutput;
   joutput["type"] = "heuristic";
   joutput["name"] = "solver_kp01_general";
   joutput["version"] = "0.0.0";

   strcmp(output, joutput.dump().c_str());
   // ignores 'key'
   /*
   output = "{ \
         \"type\": \"heuristic\", \
         \"name\": \"solver_kp01_general\", \
         \"version\": \"0.0.0\", \
         \"configs\": [] \
      }";
      */
   // "solvers" -> [] ... if 'type' is "meta" (meta-solver)
   return 0;
}
