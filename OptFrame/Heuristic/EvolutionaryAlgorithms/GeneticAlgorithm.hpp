#ifndef OPTFRAME_GENETICALGORITHM_HPP_
#define OPTFRAME_GENETICALGORITHM_HPP_

#include <algorithm>

#include "../../Heuristic.hpp"

#include "../../Population.hpp"

#include "../../InitialPopulation.h"

#include "Crossover.hpp"

#include "Mutation.hpp"

#include "Selection.hpp"

#include "Elitism.hpp"

//#define DBG_GA

/*
 * TODO
 *
 * Terminar de implementar as mensagens padrão que, no caso de estarem ativadas, devem ser impressas.
 * Ex. Qdo melhora-se a melhor solução, deve-se imprimir uma mensagem.
 *
 */

/*
 * TODO
 *
 * Criar um outro algoritmo genético que retorne apenas a melhor solução.
 *
 */

/*
 * ToDo : Analisar critérios parada do algoritmo dado um tempo máximo de execução.
 */

template<class R, class M>
class GeneticAlgorithm: public Heuristic<R, M>
{
protected:

   typedef Solution<R> chromossome;

   typedef vector<Evaluation<M>*> FitnessValues;

   Evaluator<R, M>& evaluator;

private:

   unsigned pSize;

   double crossoverRate, mutationRate, elitismRate;

   unsigned numGenerations;

   InitialPopulation<R>& initPop;

   Selection<R, M> *selection;

   Crossover<R, M> *cross;

   Mutation<R, M> *mut;

   Elitism<R, M> *elt;

public:

   GeneticAlgorithm(Evaluator<R, M>& _evaluator, InitialPopulation<R>& _initPop,
         double crossoverRate, double mutationRate, double elitismRate, unsigned populationSize,
         unsigned numGenerations, Selection<R, M>& _selection, Crossover<R, M>& _cross, Mutation<R,
               M>& _mut, Elitism<R, M>& _elt) :
      evaluator(_evaluator), initPop(_initPop), selection(&_selection), cross(&_cross), mut(&_mut),
            elt(&_elt)
   {
#ifdef DBG_GA
      cout << "Genetic Algorithm constructor started .. .\n";
#endif

      this->crossoverRate = crossoverRate;
      this->mutationRate = mutationRate;
      this->elitismRate = elitismRate;
      pSize = populationSize;
      this->numGenerations = numGenerations;

#ifdef DBG_GA
      cout << "Genetic Algorithm was correctly initialized.. .\n";
#endif

   }

   virtual void evaluateFitness(const Population<R> &p, FitnessValues &fv) const
   {
      /*
       * In the canonical genetic algorithm, fitness is defined by:
       *
       * fi/f
       *
       * where:
       *
       *    fi: is the evaluation associated with the i-th chromossome.
       *    f: is the average population evaluation.
       *
       */

      double sumEvals = 0;

      for (int i = 0; i < p.size(); i++)
      {
         fv.push_back(&evaluator.evaluate(p.at(i)));
         sumEvals += fv.at(i)->evaluation();
      }

      double avgEvalsPop = sumEvals / p.size();

      for (int i = 0; i < p.size(); i++)
      {
         fv.at(i)->setObjFunction(fv.at(i)->evaluation() / avgEvalsPop);
      }
   }

   // Means Sufficient OffSprings Created
   virtual const unsigned suffOffCreated(const Population<R> &p) const
   {
      return p.size() / 2;
   }

   virtual pair<const chromossome&, const chromossome&>& selectParents(const Population<R> &p) const
   {
      unsigned p1 = rand() % (p.size());
      unsigned p2 = p1;

      while (p1 == p2)
      {
         p2 = rand() % (p.size());
      }

      return *new pair<const chromossome&, const chromossome&> (p.at(p1), p.at(p2));
   }

   void exec(Population<R> &p, double timelimit, double target_f)
   {
#ifdef DBG_GA
      cout << "GA exec(" << target_f << "," << timelimit << ")" << endl;
#endif

      //long tini = time(NULL);

      //long tnow = time(NULL);

#ifdef DBG_GA
      cout << "Executing Genetic Algorithm with specified parameters:\n" << "Crossover Rate: "
            << crossoverRate << endl << "Mutation Rate: " << mutationRate << endl
            << "Elitism Rate: " << elitismRate << endl << "Population Size: " << p.size() << endl
            << "Total of Generations: " << numGenerations << endl;

      cout << "Generating the Initial Population .. .\n";
#endif

      p = initPop.generatePopulation(pSize);

#ifdef DBG_GA
      cout << "Initial Population successfully generated.\n";

      cout << "Calculating the fitness of each chromossome from Initial Population.. .\n";
#endif

      FitnessValues fv;
      evaluateFitness(p, fv);

#ifdef DBG_GA
      cout << "Fitness values calculated.\n";
#endif

      chromossome* cStar = new chromossome(p.at(0));

      unsigned g = 0;

      unsigned suffIters = suffOffCreated(p);

      //while (g < numGenerations && ((tnow - tini) < timelimit))
      while (g < numGenerations)
      {
#ifdef DBG_GA
         cout << "GA generation: " << g << endl;
#endif

#ifdef DBG_GA
         /*
          cout << "Population in generation " << g << endl;

          for (unsigned i = 0; i < p.size(); i++)
          {
          p.at(i).print();
          cout << "Evaluation: " << (evaluator.evaluate((p.at(i)))).evaluation() << endl;
          }

          cout << endl;

          */

         /*
          for (unsigned i = 0; i < populationSize; i++)
          cout << (evaluator.evaluate(*(p.at(i)))).evaluation() << endl;
          */
#endif

         // ---

         // Finding the best chromossome in the initial population.

         unsigned pos_cStar = 0;
         cStar = new chromossome(p.at(pos_cStar));

         bool improved = false;

         for (int i = 1; i < fv.size(); i++)
         {
            if (evaluator.betterThan(*fv.at(i), *fv.at(pos_cStar)))
            {
               pos_cStar = i;
               improved = true;
            }
         }

         if (improved)
         {
            delete cStar;
            cStar = new chromossome(p.at(pos_cStar));
         }

         // ---
         Population<R> p_elitist;
         // ---

         // Recalculating the number of best individuals (chromossomes) to carried to the next generation.
         unsigned best_chromossomes_this_generation = elitismRate / 100 * p.size();

         if (best_chromossomes_this_generation >= 1)
         {
#ifdef DBG_GA
            cout << "ELITISM\n";
#endif
            p_elitist = elt->doElitism(p, fv, best_chromossomes_this_generation);
         }

         Population<R> intermediatePop;

         unsigned iter = 0;

         /*
          *
          *  ToDo : Corrigir a implementação do laço abaixo. Do jeito que está, sempre vão ser gerados
          *  todos os individuos especificados pelo <suffIters>. Passando uma taxa de cross baixa, o que
          *  vai acontecer é que o laço vai demorar mais para finalizar. Isso esta errado. Talvez seja necessário
          *  criar versões específicas para as possibilidades de se realizar as operações de cross e mutação.
          *
          */
         do
         {
            pair<const chromossome&, const chromossome&>& parents = selectParents(p);

            pair<chromossome&, chromossome*>* offspring = NULL;

            unsigned doCross = rand() % 101; // numbers 0 to 100

            // Crossover condition
            if (doCross < crossoverRate)
            {
#ifdef DBG_GA
               cout << " >> CROSSOVER";
#endif
               offspring = &cross->offspring(parents);
            }
            else
            {
               continue;
            }

            // Mutation condition
            unsigned doMut = rand() % 101; // numbers 0 to 100

            if (doMut < mutationRate)
            {
#ifdef DBG_GA
               cout << ", MUTATION";
#endif
               mut->mutate(offspring->first);

               if (offspring->second)
               {
                  mut->mutate(*offspring->second);
               }
            }

#ifdef DBG_GA
            cout << endl;
#endif

            intermediatePop.push_back(offspring->first);

            if (offspring->second)
            {
               intermediatePop.push_back(*offspring->second);
               iter++;
            }

            iter++;

         } while (iter < suffIters); // Suffient Offsprings created

#ifdef DBG_GA
         cout << "Sufficient Offsprings created in generation " << g << endl;
#endif

         FitnessValues fv_intPop;
         evaluateFitness(intermediatePop, fv_intPop);

#ifdef DBG_GA
         cout << "Intermediate Population Evaluated. " << endl;

         cout << "SELECTION\n";
#endif

         // The selection specified by the user can return a new population with different size (nb. of chromossomes).
         Population<R> next_p = selection->select(p, fv, intermediatePop, fv_intPop,
               best_chromossomes_this_generation);

         if (!p_elitist.empty())
         {
            for (unsigned i = 0; i < p_elitist.size(); i++)
            {
               next_p.push_back(p_elitist.at(i));
            }
         }

#ifdef DBG_GA
         cout << "Next Population Chosen and Evaluated." << endl;
#endif

         // ---

         // Checking the new population size.

         bool break_it = false;

         if (p.size() < next_p.size())
         {
            cout << "WARNNING: Population's size is increasing.\n";
         }
         else if (p.size() > next_p.size())
         {
            cout << "WARNNING: Population's size is decreasing.\n";

            /*
             *
             *  When the population's size is decreasing, verify at each generation if the size reached size one.
             *
             *  If yes, break the two main loops.
             *
             */

            if (next_p.size() == 1)
            {
               cout << "THIS EXECUTION WILL GO BREAK" << endl;
               break_it = true;
            }

         }

         // ---

         for (int i = 0; i < fv.size(); i++)
         {
            delete fv.at(i);
         }

         fv.clear();

         for (int i = 0; i < fv_intPop.size(); i++)
         {
            delete &intermediatePop.at(i);
            delete fv_intPop.at(i);
         }

         intermediatePop.clear();
         fv_intPop.clear();

         p = next_p;

         // Here, we have already past the unique solution to p. Then we stop the algorithm execution.
         if (break_it)
         {
            break;
         }

         evaluateFitness(p, fv);

         // ---

         g++;

         //tnow = time(NULL);

      }

      // Adding the best chromossome found in GA execution.
      p.push_back(*cStar);

#ifdef DBG_GA
      cout << "Best chromossome's evaluation found: " << (evaluator.evaluate(*(cStar))).evaluation() << endl;
#endif

   }

   void exec(Population<R> &p, FitnessValues &ev, double timelimit, double target_f)
   {
      exec(p, timelimit, target_f);
   }

};

#endif /*OPTFRAME_GENETICALGORITHM_HPP_*/
