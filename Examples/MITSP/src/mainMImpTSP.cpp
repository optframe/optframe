
#include <math.h>
#include <stdio.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <random>
#include <tuple>
#include <vector>
//
#include <OptFrame/Experimental/NeighborhoodExploration.hpp>

using namespace std;
using namespace optframe;

using RepMiTSP = vector<int>;
using SolutionMiTSP = Solution<RepMiTSP>;
using EvaluationMiTSP = Evaluation<>;
using ESolutionMiTSP = pair<SolutionMiTSP, EvaluationMiTSP>;

// ================

class TSPProblemData {
 public:
  static mt19937 seed;
  static constexpr int N = 318;
  // Vetores que armazena a instancia em analise.
  // Vectors that contains the instance under analysis.
  static float xInstance[N];
  static float yInstance[N];

  static array<int, N> Solution;  // Vector that represents the TSP solution.
                                  //
                                  //
                                  //
  // Funcao que retorna o valor da distancia euclidiana entre dois pontos.
  // Function that returns the distance value between two coordinates.
  static float EuclidianDistance(float x1, float y1, float x2, float y2) {
    return round(sqrt(((x1 - x2) * (x1 - x2)) + ((y1 - y2) * (y1 - y2))));
  }

  static float N2OptGain(int vi, int vj) {
    int vi_anterior = vi - 1;
    int vj_posterior = vj + 1;

    if (vi == 0) vi_anterior = N - 1;

    if (vj == N - 1) vj_posterior = 0;

    if (((vi == 0) && (vj == N - 1)) || (vi_anterior == vj_posterior)) return 0;

    return -1 *
           (EuclidianDistance(xInstance[Solution[vi]], yInstance[Solution[vi]],
                              xInstance[Solution[vj_posterior]],
                              yInstance[Solution[vj_posterior]]) +
            EuclidianDistance(xInstance[Solution[vj]], yInstance[Solution[vj]],
                              xInstance[Solution[vi_anterior]],
                              yInstance[Solution[vi_anterior]]) -
            EuclidianDistance(xInstance[Solution[vi]], yInstance[Solution[vi]],
                              xInstance[Solution[vi_anterior]],
                              yInstance[Solution[vi_anterior]]) -
            EuclidianDistance(xInstance[Solution[vj]], yInstance[Solution[vj]],
                              xInstance[Solution[vj_posterior]],
                              yInstance[Solution[vj_posterior]]));
  }

  // Function that returns the cost of an actual solution.
  static float SolutionCost() {
    float Custo = 0;

    for (int i = 0; i < N - 1; i++)
      Custo = Custo + EuclidianDistance(xInstance[Solution[i]],
                                        yInstance[Solution[i]],
                                        xInstance[Solution[i + 1]],
                                        yInstance[Solution[i + 1]]);

    Custo = Custo + EuclidianDistance(
                        xInstance[Solution[N - 1]], yInstance[Solution[N - 1]],
                        xInstance[Solution[0]], yInstance[Solution[0]]);

    return Custo;
  }

  // Funtion that display an actual solution.
  static void ShowSolution() {
    printf("\nS -> ");

    for (int i = 0; i < N; i++) {
      printf("%d ", Solution[i]);
    }

    printf("\nCusto da Solução: %.0f\n", SolutionCost());
  }

  static void LoadInstance() {
    FILE* input;
    int n, c;

    // input = fopen("berlin52.tsp", "r");
    // input = fopen("pr76.tsp", "r");
    // input = fopen("eil101.tsp", "r");
    // input = fopen("bier127.tsp", "r");
    // input = fopen("kroA150.tsp", "r");
    // input = fopen("rat195.tsp", "r");
    // input = fopen("ts225.tsp", "r");
    // input = fopen("gil262.tsp", "r");
    // input = fopen("a280.tsp", "r");
    input = fopen("instances/lin318.tsp", "r");
    fscanf(input, "%d", &n);
    // printf("Lendo %d coordenadas\n", n);

    for (int i = 0; i < n; i++)
      fscanf(input, "%d %f %f", &c, &xInstance[i], &yInstance[i]);
  }

  static void CreateTSPSolution() {
    for (int i = 0; i < N; i++) Solution[i] = i;

    shuffle(Solution.begin(), Solution.end(), seed);
  }

};  // finish problem class

//--------
//--------
//--------

// ========================
//
//   begin OptFrame class
//
// ========================

// dynamic programming for 2-opt multi-improvement search
class MiTSP2OptDynProg : public NeighborhoodExploration<ESolutionMiTSP> {
 public:
  constexpr static int N = TSPProblemData::N;

  //
  op<RichMove<ESolutionMiTSP>> searchMove(
      const ESolutionMiTSP& se,
      const StopCriteria<EvaluationMiTSP>& stopCriteria) {
    // TODO:
    assert(false);
  }

  // ---------------
  // Log files: 2Opt
  // ---------------
  FILE *fi_2opt, *bi_2opt, *mi_2opt;
  FILE *fi_2opt_details, *bi_2opt_details, *mi_2opt_details;

  // =======================

  static pair<int, int>
      Optimal2OptMoves[N][N];  // Optimal 2-Opt moves for subproblem(a,b)
  static list<pair<int, int>> Selected2OptMoves;  // List of optimal 2-Opt moves

  // ----------------------------
  //

  static void N2Opt(int vi, int vj) {
    int aux;

    for (int i = 0; i < (int)(vj - vi + 1) / 2; i++) {
      aux = TSPProblemData::Solution[i + vi];
      TSPProblemData::Solution[i + vi] = TSPProblemData::Solution[vj - i];
      TSPProblemData::Solution[vj - i] = aux;
    }
  }

  static void FirstImprovement2Opt(int id_test) {
    float gain;
    int i, j, flag = 1, cont = 0;

    // fprintf(fi_2opt, "%d ", id_test);

    // iniT = clock();

    while (flag == 1) {
      flag = 0;
      for (i = 0; i < N - 1; i++) {
        for (j = i + 1; j < N; j++) {
          gain = TSPProblemData::N2OptGain(i, j);
          if (gain > 0) {
            // fprintf(fi_2opt, "%.0f ", gain);
            N2Opt(i, j);
            flag = 1;
            cont++;
            break;
          }
        }
        if (flag == 1) break;
      }
    }

    // endT = clock();

    // fprintf(fi_2opt, "\n");

    // long double time_elapsed_ms = 1000.0 * (endT - iniT) / CLOCKS_PER_SEC;

    // fprintf(fi_2opt_details, "%d %d %0.f %.3Lf\n", id_test, cont,
    // SolutionCost(), time_elapsed_ms);
  }

  static void BestImprovement2Opt(int id_test) {
    float best, actual;
    int i, j, flag = 1, bestI, bestJ, cont = 0;

    // fprintf(bi_2opt, "%d ", id_test);

    // iniT = clock();

    while (flag == 1) {
      flag = 0;
      best = 0;
      actual = 0;
      bestI = -1;

      for (i = 0; i < N - 1; i++) {
        for (j = i + 1; j < N; j++) {
          actual = TSPProblemData::N2OptGain(i, j);

          if (actual > best) {
            best = actual;
            bestI = i;
            bestJ = j;
            flag = 1;
          }
        }
      }

      if (bestI != -1) {
        cont++;
        // fprintf(bi_2opt, "%.0f ", best);
        N2Opt(bestI, bestJ);
      }
    }

    // endT = clock();

    // fprintf(bi_2opt, "\n");

    // long double time_elapsed_ms = 1000.0 * (endT - iniT) / CLOCKS_PER_SEC;

    // fprintf(bi_2opt_details, "%d %d %0.f %.3Lf\n", id_test, cont,
    // SolutionCost(), time_elapsed_ms);
  }

  static void update2OptInternalMoves(pair<int, int> move) {
    list<pair<int, int>>::iterator w;
    for (w = Selected2OptMoves.begin(); w != Selected2OptMoves.end(); w++)
      if ((move.first < (*w).first) && (move.second > (*w).second)) {
        (*w).first = move.second - (*w).first + move.first;
        (*w).second = move.second - (*w).second + move.first;

        if ((*w).first > (*w).second) {
          // swap
          int aux = (*w).first;
          (*w).first = (*w).second;
          (*w).second = aux;
        }
      }
  }

  static void MultiImprovement2Opt(int id_test) {
    int cont = 0;
    float acc_gain;

    // fprintf(mi_2opt, "%d ", id_test);

    // iniT = clock();

    while (1) {
      acc_gain = 0;
      DynamicProgramming2Opt();

      if (!Selected2OptMoves.empty()) {
        // printf("\n%d\n",(int)Selected2OptMoves.size());
        cont++;
      } else
        break;

      list<pair<int, int>>::iterator w;
      for (w = Selected2OptMoves.begin(); w != Selected2OptMoves.end(); w++) {
        acc_gain =
            acc_gain + TSPProblemData::N2OptGain((*w).first, (*w).second);
        N2Opt((*w).first, (*w).second);
        update2OptInternalMoves(*w);
      }

      // fprintf(mi_2opt, "%.0f ", acc_gain);
    }

    // endT = clock();

    // fprintf(mi_2opt, "\n");

    // long double time_elapsed_ms = 1000.0 * (endT - iniT) / CLOCKS_PER_SEC;

    // fprintf(mi_2opt_details, "%d %d %0.f %.3Lf\n", id_test, cont,
    // SolutionCost(), time_elapsed_ms);
  }

  static void Mount2OptOptimalList(int a, int b) {
    if ((a > b) || (a < 0) || (b >= N)) return;

    if (Optimal2OptMoves[a][b].first == -1)
      Mount2OptOptimalList(a, b - 1);
    else {
      if (TSPProblemData::N2OptGain(Optimal2OptMoves[a][b].first,
                                    Optimal2OptMoves[a][b].second) >= 0)
        Selected2OptMoves.push_back(Optimal2OptMoves[a][b]);
      Mount2OptOptimalList(a, Optimal2OptMoves[a][b].first - 2);
      // Mount2OptOptimalList(Optimal2OptMoves[a][b].second+2, b);
      Mount2OptOptimalList(Optimal2OptMoves[a][b].first + 2,
                           Optimal2OptMoves[a][b].second - 2);
    }
  }

  static void DynamicProgramming2Opt() {
    float L[N][N];

    int a, b;
    int i, k;
    float best, actual, left, down, diagonal, cost;
    int bestI, bestJ;

    // Zerando toda a matriz da Programacao Dinamica.
    for (k = 0; k < N; k++) {
      for (i = 0; i < N; i++) L[k][i] = 0;
    }

    for (k = 0; k < N; k++) {
      for (a = 0; a < N - k; a++) {
        b = a + k;

        if (a >= b) continue;

        best = 0, bestI = -1, bestJ = -1;

        // Analise do caso em que o 2Opt(i,b) não pertence a solução ótima.
        if (L[a][b - 1] >= best) {
          best = L[a][b - 1];
          bestI = -1;
          bestJ = 0;
        }

        // Análise do caso em que o 2Opt(a,b) faz parte
        // da Solução juntamente com a solução dos sub-problemas já calculados.

        for (i = a; i < b; i++) {
          // for(j = i+1; j < b; j++){
          cost = 0;

          actual = TSPProblemData::N2OptGain(i, b);
          left = (((i - 2 >= a) && (i - 2 >= 0)) ? L[a][i - 2] : 0);
          // down = (j+2<=b? L[j+2][b]: 0);
          diagonal = (((i + 2 < N) && (b - 2 >= i + 2)) ? L[i + 2][b - 2] : 0);

          cost = (actual >= 0 ? actual : 0) + left + diagonal;

          if (cost >= best) {
            best = cost;
            bestI = i;
            bestJ = b;
          }
        }
        if (best >= L[a][b]) {
          L[a][b] = best;
          Optimal2OptMoves[a][b] = make_pair(bestI, bestJ);
        }
      }
    }
    Selected2OptMoves.clear();

    if ((L[1][N - 1] <= 0) && (L[0][N - 2] <= 0)) return;
    // printf("\n: %.1f : : %.1f :\n",L[1][N-1],L[0][N-2]);

    if (L[1][N - 1] >= L[0][N - 2])
      Mount2OptOptimalList(1, N - 1);
    else
      Mount2OptOptimalList(0, N - 2);

    // list<pair<int,int>>::iterator w;
    // for(w = OptimalMoves.begin(); w != OptimalMoves.end(); w++)
    //   printf("\n%d %d =
    //   %.0f",(*w).first,(*w).second,N2OptGain((*w).first,(*w).second));
    //
    //
  }  // end Dynamic Programming
};

// defining (after declaring)
mt19937 TSPProblemData::seed;  // = (mt19937)0;
array<int, TSPProblemData::N> TSPProblemData::Solution;
float TSPProblemData::xInstance[TSPProblemData::N];
float TSPProblemData::yInstance[TSPProblemData::N];

pair<int, int> MiTSP2OptDynProg::Optimal2OptMoves[MiTSP2OptDynProg::N]
                                                 [MiTSP2OptDynProg::N];
list<pair<int, int>> MiTSP2OptDynProg::Selected2OptMoves;

// ======================

int main() {
  cout << "Begin MI experiments (N = " << MiTSP2OptDynProg::N << ")" << endl;
  TSPProblemData::seed = (mt19937)time(0);

  clock_t ini_t_global, end_t_global;

  TSPProblemData::LoadInstance();

  // CreateTSPSolution();
  // ini_t_global = clock();
  // LocalSearch();
  // end_t_global = clock();

  // ================================
  // LocalSearch -> 2-Opt LocalSearch
  // ================================

  TSPProblemData::CreateTSPSolution();
  array<int, TSPProblemData::N> InitSolution;
  InitSolution = TSPProblemData::Solution;

  //
  int i = 0;
  //
  printf("::: TESTE %d :::\n", i);
  printf("\nNova solução inicial gerada\n");
  // ShowSolution();

  printf("\nRealizando Busca Local First Improvement\n");
  // MiTSP2OptDynProg::FirstImprovementOrOpt(2, i);
  MiTSP2OptDynProg::FirstImprovement2Opt(i);
  printf("Encontrada solução de custo: %f\n", TSPProblemData::SolutionCost());

  TSPProblemData::Solution = InitSolution;  // Resetando a Solução final.
  printf("\nRealizando Busca Local Best Improvement\n");
  // MiTSP2OptDynProg::BestImprovementOrOpt(2, i);
  MiTSP2OptDynProg::BestImprovement2Opt(i);
  printf("Encontrada solução de custo: %f\n", TSPProblemData::SolutionCost());

  TSPProblemData::Solution = InitSolution;  // Resetando a Solução final.
  printf("\nRealizando Busca Local Multi Improvement\n");
  // MiTSP2OptDynProg::MultiImprovementOrOpt(2, i);
  MiTSP2OptDynProg::MultiImprovement2Opt(i);
  printf("Encontrada solução de custo: %f\n", TSPProblemData::SolutionCost());

  printf("\n\n");

  long double time_elapsed_ms =
      1000.0 * (end_t_global - ini_t_global) / CLOCKS_PER_SEC;

  printf("\nTempo da Busca Local: %.2Lf", time_elapsed_ms);

  cout << "\n\nFim" << endl;

  cout << "finished successfully" << endl;
  return 0;
}

// =================== begin Janio ==============================

// ARQUIVOS DE CABECALHO

// PREAMBULO

// using namespace std;

//#define N 52
//#define N 76
//#define N 101
//#define N 127
//#define N 150
//#define N 195
//#define N 225
//#define N 262
//#define N 280
#define N 318

// Vetores que armazena a instancia em analise.
// Vectors that contains the instance under analysis.
//
// float xInstance[N];
// float yInstance[N];

// array<int, N> Solution; //Vector that represents the TSP solution.
//
//
// array <int,N> Solution = {19, 34, 2, 16, 11, 14, 45, 33, 26, 13, 6, 1, 44,
// 32, 37, 8, 46, 35, 48, 4, 15, 25, 28, 40, 39, 29, 27, 18, 36, 7, 17, 9, 10,
// 42, 3, 41, 24, 47, 5, 30, 23, 49, 38, 31, 0, 22, 12, 50, 43, 21, 51, 20};
// array <int,N> Solution = {26, 11, 44, 38, 34, 14, 42, 9, 37, 30, 15, 19, 24,
// 25, 31, 45, 2, 46, 35, 10, 7, 1, 4, 51, 32, 3, 40, 49, 28, 43, 33, 39, 41,
// 20, 23, 5, 21, 50, 36, 27, 48, 16, 12, 17, 18, 13, 47, 0, 8, 29, 6, 22};

tuple<int, int, int, int>
    Optimal3OptMoves[N][N];  // Optimal 3-Opt moves for subproblem(a,b)
list<tuple<int, int, int, int>>
    Selected3OptMoves;  // List of optimal 2-Opt moves

tuple<int, int, int>
    OptimalOrOpt2Moves[N][N];  // Optimal Or-Opt moves for subproblem(a,b)
list<tuple<int, int, int>> SelectedOrOpt2Moves;  // List of optimal Or-Opt moves

// Log files: 3Opt
FILE *fi_3opt, *bi_3opt, *mi_3opt;
FILE *fi_3opt_details, *bi_3opt_details, *mi_3opt_details;

// Log files: OrOpt
FILE *fi_oropt, *bi_oropt, *mi_oropt;
FILE *fi_oropt_details, *bi_oropt_details, *mi_oropt_details;

clock_t iniT, endT;

/*

void
N3Opt(int vi, int vj, int vk, int type)
{

   int tmp[vk - vi + 1];
   int range1, range2;
   int i;
   range1 = vj - vi + 1;
   range2 = vk - vj;

   if (type != 4) {
      for (i = 0; i < vk - vi + 1; i++)
         tmp[i] = Solution[vi + i];

      for (i = 0; i < range2; i++)
         Solution[vi + i] = tmp[range1 + i];

      for (i = 0; i < range1; i++)
         Solution[vi + range2 + i] = tmp[i];

      //Sem inverter nenhum dos ranges, uma primeira situação de 3Opt esta
solucionada.

      if (type == 2)
         N2Opt(vi, vi + range2 - 1);

      if (type == 3)
         N2Opt(vi + range2, vk);
   } else {
      N2Opt(vi, vj);
      N2Opt(vj + 1, vk);
   }
}

float
N3OptGain(int vi, int vj, int vk, int type)
{

   int before, after;

   before = vi - 1;
   after = vk + 1;

   if (vi == 0)
      before = N - 1;

   if (vk == N - 1)
      after = 0;

   if (((vi == 0) && (vk == N - 1)) || (before == after))
      return 0;

   float BrokenEdges = EuclidianDistance(xInstance[Solution[before]],
yInstance[Solution[before]], xInstance[Solution[vi]], yInstance[Solution[vi]]) +
EuclidianDistance(xInstance[Solution[vj]], yInstance[Solution[vj]],
xInstance[Solution[vj + 1]], yInstance[Solution[vj + 1]]) +
EuclidianDistance(xInstance[Solution[vk]], yInstance[Solution[vk]],
xInstance[Solution[after]], yInstance[Solution[after]]);

   if (type == 1)
      return -1 * (EuclidianDistance(xInstance[Solution[before]],
yInstance[Solution[before]], xInstance[Solution[vj + 1]], yInstance[Solution[vj
+ 1]]) + EuclidianDistance(xInstance[Solution[vk]], yInstance[Solution[vk]],
xInstance[Solution[vi]], yInstance[Solution[vi]]) +
EuclidianDistance(xInstance[Solution[vj]], yInstance[Solution[vj]],
xInstance[Solution[after]], yInstance[Solution[after]]) - BrokenEdges);

   if (type == 2)
      return -1 * (EuclidianDistance(xInstance[Solution[before]],
yInstance[Solution[before]], xInstance[Solution[vk]], yInstance[Solution[vk]]) +
EuclidianDistance(xInstance[Solution[vj + 1]], yInstance[Solution[vj + 1]],
xInstance[Solution[vi]], yInstance[Solution[vi]]) +
EuclidianDistance(xInstance[Solution[vj]], yInstance[Solution[vj]],
xInstance[Solution[after]], yInstance[Solution[after]]) - BrokenEdges);

   if (type == 3)
      return -1 * (EuclidianDistance(xInstance[Solution[before]],
yInstance[Solution[before]], xInstance[Solution[vj + 1]], yInstance[Solution[vj
+ 1]]) + EuclidianDistance(xInstance[Solution[vk]], yInstance[Solution[vk]],
xInstance[Solution[vj]], yInstance[Solution[vj]]) +
EuclidianDistance(xInstance[Solution[vi]], yInstance[Solution[vi]],
xInstance[Solution[after]], yInstance[Solution[after]]) - BrokenEdges);

   if (type == 4)
      return -1 * (EuclidianDistance(xInstance[Solution[before]],
yInstance[Solution[before]], xInstance[Solution[vj]], yInstance[Solution[vj]]) +
EuclidianDistance(xInstance[Solution[vi]], yInstance[Solution[vi]],
xInstance[Solution[vk]], yInstance[Solution[vk]]) +
EuclidianDistance(xInstance[Solution[vj + 1]], yInstance[Solution[vj + 1]],
xInstance[Solution[after]], yInstance[Solution[after]]) - BrokenEdges);
}

void
Mount3OptOptimalList(int a, int b)
{

   if ((a >= b) || (a < 0) || (b >= N))
      return;

   int i, j, k, type;
   i = get<0>(Optimal3OptMoves[a][b]);
   j = get<1>(Optimal3OptMoves[a][b]);
   k = get<2>(Optimal3OptMoves[a][b]);
   type = get<3>(Optimal3OptMoves[a][b]);

   //printf("\n\na=%d --> i=%d j=%d k=%d <-- b=%d\n\n",a,i,j,k,b);

   if (i == -1)
      Mount3OptOptimalList(a, b - 1);
   else {
      if (N3OptGain(i, j, k, type) >= 0)
         Selected3OptMoves.push_back(Optimal3OptMoves[a][b]);

      Mount3OptOptimalList(a, i - 2);
      //Mount3OptOptimalList(i+1,j-1);
      //Mount3OptOptimalList(j+1,k-1);
      //Mount3OptOptimalList(k + 2, b);
   }
}

void
DynamicProgramming3Opt()
{

   float L[N][N];

   int a, b;
   int d, i, j;
   float best, actual, left, cost, bestCost, actualType;
   int bestI, bestJ, bestK, bestType;

   //Zerando toda a matriz da Programacao Dinamica.
   for (d = 0; d < N; d++) {
      for (i = d; i < N; i++)
         L[d][i] = 0;
   }


   //   loops encadeados que percorrem a matriz de memoization no sentido
   //   paralelo a diagonal principal.

   for (d = 0; d < N; d++) {
      for (a = 0; a < N - d; a++) {
         b = a + d;

         if (a >= b)
            continue;

         best = 0;
         bestI = -1, bestJ = -1, bestK = -1, bestType = -1;

         //Analise do caso em que o 2Opt(i,j,b) não pertence a solução ótima.
         if (L[a][b - 1] >= best) {
            best = L[a][b - 1];
            bestI = -1;
         }


         //       Tabela de memoization simula uma árvore de recorrência
preenchida
         //       e calculada de forma bottom-up.

         for (i = a; i < b; i++) {
            for (j = i; j < b; j++) {
               //for(k = j+2; k <= b; k++){
               bestCost = 0;

               if (b - i < 2)
                  continue;

               //Existem 4 tipos distintos de 3-Opt, nem sempre o melhor faz
parte da solução ótima. for (int type = 1; type <= 4; type++) {

                  cost = 0;
                  actual = N3OptGain(i, j, b, type);
                  left = (((a <= i - 2) && (i - 2 >= 0)) ? L[a][i - 2] : 0);
                  //down = ( ((k+2 <= b) && (k+2 <= N-1)) ? L[k+2][b]: 0);
                  //insideRange1 = ((i+1 <= j-1)? L[i+1][j-1]: 0);
                  //insideRange2 = ((j+1 <= k-1)? L[j+1][k-1]: 0);

                  //cost = (actual>=0?actual:0) + left + down + insideRange1 +
insideRange2; cost = (actual >= 0 ? actual : 0) + left;

                  if (cost >= bestCost) {
                     bestCost = cost;
                     actualType = type;
                  }
               }

               if (bestCost >= best) {
                  best = bestCost;
                  bestI = i;
                  bestJ = j;
                  bestK = b;
                  bestType = actualType;
               }
               // } Fim do for suprimido
            }
         }
         if (best >= L[a][b]) {
            L[a][b] = best;
            Optimal3OptMoves[a][b] = make_tuple(bestI, bestJ, bestK, bestType);
         }
      }
   }

   Selected3OptMoves.clear();

   if ((L[1][N - 1] <= 0) && (L[0][N - 2] <= 0))
      return;
   //printf("\n: %.1f : : %.1f :\n",L[1][N-1],L[0][N-2]);

   if (L[1][N - 1] >= L[0][N - 2])
      Mount3OptOptimalList(1, N - 1);
   else
      Mount3OptOptimalList(0, N - 2);

   //list<tuple<int,int,int,int>>::iterator w;
   //for(w = Selected3OptMoves.begin(); w != Selected3OptMoves.end(); w++)
   //  printf("\n|%d<->%d| |%d<->%d| tipo(%d) =
%.0f",get<0>(*w),get<1>(*w),get<1>(*w)+1,get<2>(*w),get<3>(*w),
N3OptGain(get<0>(*w),get<1>(*w),get<2>(*w),get<3>(*w)));
}

void
MountOrOpt2OptimalList(int a, int b)
{

   if ((a >= b) || (a < 0) || (b >= N))
      return;

   int i, j, k;
   i = get<0>(OptimalOrOpt2Moves[a][b]);
   j = get<1>(OptimalOrOpt2Moves[a][b]);
   k = get<2>(OptimalOrOpt2Moves[a][b]);

   //printf("\n\na=%d --> i=%d j=%d k=%d <-- b=%d\n\n",a,i,j,k,b);

   if (i == -1)
      MountOrOpt2OptimalList(a, b - 1);
   else {
      if (N3OptGain(i, k, j, 1) >= 0)
         SelectedOrOpt2Moves.push_back(OptimalOrOpt2Moves[a][b]);

      MountOrOpt2OptimalList(a, i - 2);
   }
}

void
DynamicProgrammingOrOpt(int k)
{

   float L[N][N];

   int a, b;
   int d, i;
   float best, actual, left, down, cost;
   int bestI, bestJ, bestK, actualK;
   int shift;
   float after, before;

   //Zerando toda a matriz da Programacao Dinamica.
   for (d = 0; d < N; d++) {
      for (i = d; i < N; i++)
         L[d][i] = 0;
   }


    //  loops encadeados que percorrem a matriz de memoization no sentido
    //  paralelo a diagonal principal.

   for (d = 0; d < N; d++) {
      for (a = 0; a < N - d; a++) {
         b = a + d;

         if (a >= b)
            continue;

         best = 0;
         bestI = -1, bestJ = -1;
         bestK = -1;

         //Analise do caso em que o OrOpt2(i,b) não pertence a solução ótima.
         if (L[a][b - 1] >= best) {
            best = L[a][b - 1];
            bestI = -1;
         }


          //      Tabela de memoization simula uma árvore de recorrência
preenchida
           //     e calculada de forma bottom-up.


         for (i = a; i < b; i++) {

            if (b - i < k)
               continue;

            cost = 0;

            before = N3OptGain(i, i + k - 1, b, 1);
            after = N3OptGain(i, b - k, b, 1);

            if (before >= after) {
               actualK = i + k - 1;
               actual = before;
            } else {
               actualK = b - k;
               actual = after;
            }

            left = (((a <= i - 2) && (i - 2 >= 0)) ? L[a][i - 2] : 0);
            cost = (actual >= 0 ? actual : 0) + left;

            if (cost >= best) {
               best = cost;
               bestI = i;
               bestJ = b;
               bestK = actualK;
            }
         }

         if (best >= L[a][b]) {
            L[a][b] = best;
            OptimalOrOpt2Moves[a][b] = make_tuple(bestI, bestJ, bestK);
         }
      }
   }

   SelectedOrOpt2Moves.clear();

   if ((L[1][N - 1] <= 0) && (L[0][N - 2] <= 0))
      return;
   //printf("\n: %.1f : : %.1f :\n",L[1][N-1],L[0][N-2]);

   if (L[1][N - 1] >= L[0][N - 2])
      MountOrOpt2OptimalList(1, N - 1);
   else
      MountOrOpt2OptimalList(0, N - 2);

   //list<tuple<int,int,int>>::iterator w;
   //for(w = SelectedOrOpt2Moves.begin(); w != SelectedOrOpt2Moves.end(); w++)
   //  printf("\nRange |%d<->%d| shifted to after %d = %.0f",get<0>(*w),
get<2>(*w),get<1>(*w),N3OptGain(get<0>(*w), get<2>(*w),get<1>(*w),1));

   //printf("\n\n");
}

void
MultiImprovement2Opt(int id_test)
{

   int cont = 0;
   float acc_gain;

   fprintf(mi_2opt, "%d ", id_test);

   iniT = clock();

   while (1) {
      acc_gain = 0;
      DynamicProgramming2Opt();

      if (!Selected2OptMoves.empty()) {
         //printf("\n%d\n",(int)Selected2OptMoves.size());
         cont++;
      } else
         break;

      list<pair<int, int>>::iterator w;
      for (w = Selected2OptMoves.begin(); w != Selected2OptMoves.end(); w++) {
         acc_gain = acc_gain + N2OptGain((*w).first, (*w).second);
         N2Opt((*w).first, (*w).second);
         update2OptInternalMoves(*w);
      }

      fprintf(mi_2opt, "%.0f ", acc_gain);
   }

   endT = clock();

   fprintf(mi_2opt, "\n");

   long double time_elapsed_ms = 1000.0 * (endT - iniT) / CLOCKS_PER_SEC;

   fprintf(mi_2opt_details, "%d %d %0.f %.3Lf\n", id_test, cont, SolutionCost(),
time_elapsed_ms);
}

void
MultiImprovement3Opt(int id_test)
{

   int cont = 0;
   float acc_gain;

   fprintf(mi_3opt, "%d ", id_test);

   iniT = clock();

   while (1) {
      acc_gain = 0;
      DynamicProgramming3Opt();

      if (!Selected3OptMoves.empty())
         cont++;
      else
         break;

      list<tuple<int, int, int, int>>::iterator w;
      for (w = Selected3OptMoves.begin(); w != Selected3OptMoves.end(); w++) {
         acc_gain = acc_gain + N3OptGain(get<0>(*w), get<1>(*w), get<2>(*w),
get<3>(*w)); N3Opt(get<0>(*w), get<1>(*w), get<2>(*w), get<3>(*w));
      }
      fprintf(mi_3opt, "%.0f ", acc_gain);
   }

   endT = clock();

   fprintf(mi_3opt, "\n");

   long double time_elapsed_ms = 1000.0 * (endT - iniT) / CLOCKS_PER_SEC;

   fprintf(mi_3opt_details, "%d %d %0.f %.3Lf\n", id_test, cont, SolutionCost(),
time_elapsed_ms);
}

void
MultiImprovementOrOpt(int k, int id_test)
{

   int cont = 0;
   float acc_gain;

   fprintf(mi_oropt, "%d ", id_test);

   iniT = clock();

   while (1) {
      acc_gain = 0;
      DynamicProgrammingOrOpt(k);

      if (!SelectedOrOpt2Moves.empty())
         cont++;
      else
         break;

      list<tuple<int, int, int>>::iterator w;
      for (w = SelectedOrOpt2Moves.begin(); w != SelectedOrOpt2Moves.end(); w++)
{ acc_gain = acc_gain + N3OptGain(get<0>(*w), get<2>(*w), get<1>(*w), 1);
         N3Opt(get<0>(*w), get<2>(*w), get<1>(*w), 1);
      }
      fprintf(mi_oropt, "%.0f ", acc_gain);
   }

   endT = clock();

   fprintf(mi_oropt, "\n");

   long double time_elapsed_ms = 1000.0 * (endT - iniT) / CLOCKS_PER_SEC;

   fprintf(mi_oropt_details, "%d %d %0.f %.3Lf\n", id_test, cont,
SolutionCost(), time_elapsed_ms);
}

void
FirstImprovementOrOpt(int k, int id_test)
{

   float gain;
   int i, j, flag = 1, cont = 0;

   fprintf(fi_oropt, "%d ", id_test);

   iniT = clock();

   while (flag == 1) {

      flag = 0;
      //Verifying shifts to right
      for (i = 0; i < N - k - 1; i++) {
         for (j = i + k; j < N; j++) {

            //if(j - i <= k)
            //        continue;

            gain = N3OptGain(i, i + k - 1, j, 1);
            if (gain > 0) {
               fprintf(fi_oropt, "%.0f ", gain);
               N3Opt(i, i + k - 1, j, 1);
               flag = 1;
               cont++;
               break;
            }
         }
         if (flag == 1)
            break;
      }

      //Now, we are verifying shifts to left
      if (flag == 0) {
         for (j = N - k; j > 0; j--) {
            for (i = j - 1; i >= 0; i--) {

               gain = N3OptGain(i, j - 1, j + k - 1, 1);
               if (gain > 0) {
                  fprintf(fi_oropt, "%.0f ", gain);
                  N3Opt(i, j - 1, j + k - 1, 1);
                  flag = 1;
                  cont++;
                  break;
               }
            }

            if (flag == 1)
               break;
            //else printf("\nWARNING Cheguei flag = %d\n",flag);
         }
      }

      if (flag == 1)
         continue;

      //printf("\nCheguei flag = %d\n",flag);
   }

   endT = clock();

   fprintf(fi_oropt, "\n");

   long double time_elapsed_ms = 1000.0 * (endT - iniT) / CLOCKS_PER_SEC;

   fprintf(fi_oropt_details, "%d %d %0.f %.3Lf\n", id_test, cont,
SolutionCost(), time_elapsed_ms);
}

void
FirstImprovement3Opt(int id_test)
{

   float gain, bestGain;
   int i, j, k, type, flag = 1, cont = 0, bestType;

   fprintf(fi_3opt, "%d ", id_test);
   iniT = clock();

   while (flag == 1) {
      flag = 0;
      bestType = -1;
      bestGain = 0;
      for (i = 0; i < N - 1; i++) {
         for (j = i + 1; j < N; j++) {
            for (k = j + 1; k < N; k++) {

               if (k - i < 2)
                  continue;

               for (type = 1; type <= 4; type++) {

                  gain = N3OptGain(i, j, k, type);

                  if (gain > bestGain) {
                     bestGain = gain;
                     bestType = type;
                  }
               }

               if (bestGain > 0) {
                  fprintf(fi_3opt, "%.0f ", bestGain);
                  N3Opt(i, j, k, bestType);
                  flag = 1;
                  cont++;
                  break;
               }
            }
            if (flag == 1) {
               i = j = k = N;
            }
         }
      }
   }

   endT = clock();

   fprintf(fi_3opt, "\n");

   long double time_elapsed_ms = 1000.0 * (endT - iniT) / CLOCKS_PER_SEC;

   fprintf(fi_3opt_details, "%d %d %0.f %.3Lf\n", id_test, cont, SolutionCost(),
time_elapsed_ms);
}

void
BestImprovement3Opt(int id_test)
{
   float best, actual, actualCost;
   int i, j, k, flag = 1, bestI, bestJ, bestK, type, actualType, bestType, cont
= 0;

   fprintf(bi_3opt, "%d ", id_test);
   iniT = clock();

   while (flag == 1) {
      flag = 0;
      best = 0;
      actual = 0;
      bestI = -1;

      for (i = 0; i < N - 1; i++) {
         for (j = i + 1; j < N - 1; j++) {
            for (k = j + 1; k < N; k++) {

               if (k - i < 2)
                  continue;

               actual = 0;

               for (type = 1; type <= 4; type++) {
                  actualCost = N3OptGain(i, j, k, type);
                  if (actualCost > actual) {
                     actual = actualCost;
                     actualType = type;
                  }
               }
               if (actual > best) {
                  best = actual;
                  bestI = i;
                  bestJ = j, bestK = k, bestType = actualType;
                  flag = 1;
               }
            }
         }
      }

      if (bestI != -1) {
         cont++;
         fprintf(bi_3opt, "%.0f ", best);
         N3Opt(bestI, bestJ, bestK, bestType);
      }
   }
   endT = clock();

   fprintf(bi_3opt, "\n");

   long double time_elapsed_ms = 1000.0 * (endT - iniT) / CLOCKS_PER_SEC;

   fprintf(bi_3opt_details, "%d %d %0.f %.3Lf\n", id_test, cont, SolutionCost(),
time_elapsed_ms);
}

void
BestImprovementOrOpt(int k, int id_test)
{
   float best, actual;
   int i, j, flag = 1, bestI, bestJ, cont = 0;

   fprintf(bi_oropt, "%d ", id_test);

   while (flag != 0) {
      flag = 0;
      best = 0;
      actual = 0;
      bestI = -1;

      for (i = 0; i < N - k - 1; i++) {
         for (j = i + k; j < N; j++) {

            actual = N3OptGain(i, i + k - 1, j, 1);

            if (actual > best) {
               best = actual;
               bestI = i;
               bestJ = j;
               flag = 1;
            }
         }
      }

      for (j = N - k; j > 0; j--) {
         for (i = j - 1; i >= 0; i--) {

            actual = N3OptGain(i, j - 1, j + k - 1, 1);

            if (actual > best) {
               best = actual;
               bestI = i;
               bestJ = j;
               flag = 2;
            }
         }
      }

      if (flag == 1) {
         cont++;
         fprintf(bi_oropt, "%.0f ", best);
         N3Opt(bestI, bestI + k - 1, bestJ, 1);
      } else if (flag == 2) {
         cont++;
         fprintf(bi_oropt, "%.0f ", best);
         N3Opt(bestI, bestJ - 1, bestJ + k - 1, 1);
      }
   }

   endT = clock();

   fprintf(bi_oropt, "\n");

   long double time_elapsed_ms = 1000.0 * (endT - iniT) / CLOCKS_PER_SEC;

   fprintf(bi_oropt_details, "%d %d %0.f %.3Lf\n", id_test, cont,
SolutionCost(), time_elapsed_ms);
}

void
LocalSearch()
{

   array<int, N> InitSolution;
   //    fi_2opt = fopen("fi_2opt.log","w+");
   //    bi_2opt = fopen("bi_2opt.log","w+");
   //    mi_2opt = fopen("mi_2opt.log","w+");
   //
   //    fi_2opt_details = fopen("fi_2opt_details.log","w+");
   //    bi_2opt_details = fopen("bi_2opt_details.log","w+");
   //    mi_2opt_details = fopen("mi_2opt_details.log","w+");

   //    fi_3opt = fopen("fi_3opt.log","w+");
   //    bi_3opt = fopen("bi_3opt.log","w+");
   //    mi_3opt = fopen("mi_3opt.log","w+");
   //
   //    fi_3opt_details = fopen("fi_3opt_details.log","w+");
   //    bi_3opt_details = fopen("bi_3opt_details.log","w+");
   //    mi_3opt_details = fopen("mi_3opt_details.log","w+");

   fi_oropt = fopen("fi_oropt.log", "w+");
   bi_oropt = fopen("bi_oropt.log", "w+");
   mi_oropt = fopen("mi_oropt.log", "w+");

   fi_oropt_details = fopen("fi_oropt_details.log", "w+");
   bi_oropt_details = fopen("bi_oropt_details.log", "w+");
   mi_oropt_details = fopen("mi_oropt_details.log", "w+");

   for (int i = 1; i <= 1; i++) {
      CreateTSPSolution();
      InitSolution = Solution;

      printf("::: TESTE %d :::\n", i);
      printf("\nNova solução inicial gerada\n");
      //ShowSolution();

      printf("\nRealizando Busca Local First Improvement\n");
      FirstImprovementOrOpt(2, i);
      printf("Encontrada solução de custo: %f\n", SolutionCost());

      Solution = InitSolution; // Resetando a Solução final.
      printf("\nRealizando Busca Local Best Improvement\n");
      BestImprovementOrOpt(2, i);
      printf("Encontrada solução de custo: %f\n", SolutionCost());

      Solution = InitSolution; // Resetando a Solução final.
      printf("\nRealizando Busca Local Multi Improvement\n");
      MultiImprovementOrOpt(2, i);
      printf("Encontrada solução de custo: %f\n", SolutionCost());

      printf("\n\n");
   }
}

*/

// ===================

/*
int
main()
{

   seed = (mt19937)time(0);

   clock_t ini_t_global, end_t_global;

   LoadInstance();
   //CreateTSPSolution();
   ini_t_global = clock();
   LocalSearch();
   end_t_global = clock();

   long double time_elapsed_ms = 1000.0 * (end_t_global - ini_t_global) /
CLOCKS_PER_SEC;

   printf("\nTempo da Busca Local: %.2Lf", time_elapsed_ms);

   cout << "\n\nFim" << endl;

   return 0;
}
*/
