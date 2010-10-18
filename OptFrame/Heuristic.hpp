#ifndef OPTFRAME_HEURISTIC_HPP_
#define OPTFRAME_HEURISTIC_HPP_

#include <iostream>
#include <vector>

using namespace std;

#include "./Solution.hpp"
#include "Population.hpp"
#include "./Evaluation.hpp"
#include "./Util/Runnable/Runnable.hpp"

template<class R, class M = OPTFRAME_DEFAULT_MEMORY>
class Heuristic: public Runnable
{
   //typedef vector<Solution<R>*> Population;
   //typedef const vector<const Solution<R>*> ConstPopulation;
   typedef vector<Evaluation<M>*> FitnessValues;
   typedef const vector<const Evaluation<M>*> ConstFitnessValues;

protected:
   Solution<R>* log_solution;
   //Population log_solution_set;
   Population<R> log_solution_set;
   long log_timelimit;
   double log_efv;

public:

   static const int SAFE_SEARCH_TOLERANCE = 2; // tolerance in seconds

   Heuristic() :
      Runnable()
   {
      log_solution = NULL;
   }

   virtual ~Heuristic()
   {
   }

   /// SafeSearch guarantees that your method will not loop! But your method may become very slow... avoid using this method.
   /**
    safeSearch é um método Solução -> Solução, parametrizado pelo tempo
    máximo de execução desejado para a heurística.
    Para garantir que a heurística terminará no tempo limite desejado o
    método é iniciado em uma thread que é encerrada logo após esse tempo.\n
    Caso o método termine normalmente sua solução é retornada; caso contrário
    uma solução NULL é retornada.
    */

   virtual void run()
   {
      /*	if(log_solution) // if not null
       log_solution = exec(log_efv, log_timelimit,log_solution);
       else
       log_solution_set = exec(log_efv, log_timelimit,log_solution_set);
       */
   }

   /*

    virtual Solution<R>& safeSearch(double efv, long timelimit, Solution<R>* base)
    {
    if(timelimit<=0)
    return base->clone();

    log_solution = base->clone();
    log_solution_set.clear();
    log_timelimit = timelimit;
    log_efv = efv;

    start();

    for(int t=0;t<timelimit;t++)
    if(!finished())
    sleep(1);
    else
    return log_solution;

    for(int t=0;t<SAFE_SEARCH_TOLERANCE;t++)
    if(!finished())
    {
    cout << "Warning: Process not finished yet. Safe Search Tolerance ("<<t<<" secs.)"<<endl;
    sleep(1);
    }
    else
    return log_solution;

    stop();

    cout << "Warning: Safe Search saved method from looping! (maybe +1 solution memory lack)" << endl;
    return base->clone();
    }

    virtual vector<Solution<R>*> safeSearch(double efv, long timelimit, vector<Solution<R>*> base)
    {
    if(timelimit<=0)
    return base;

    vector<Solution<R>*> v;
    for(int i=0;i<base.size();i++)
    v.push_back(base[i]->clone());

    log_solution = NULL;
    log_solution_set = v;
    log_timelimit = timelimit;
    log_efv = efv;

    start();

    for(int t=0;t<timelimit;t++)
    if(!finished())
    sleep(1);
    else
    return log_solution_set;

    for(int t=0;t<SAFE_SEARCH_TOLERANCE;t++)
    if(!finished())
    {
    cout << "Warning: Process not finished yet. Safe Search Tolerance ("<<t<<" secs.)"<<endl;
    sleep(1);
    }
    else
    return log_solution_set;

    stop();

    cout << "Warning: Safe Search saved method from looping! (maybe +1 solution memory lack)" << endl;
    return base;
    }
    */

   // no-optimization
   Solution<R>& search(const Solution<R>& s, double timelimit = 100000000, double target_f = 0)
   {
      Solution<R>& s2 = s.clone();
      exec(s2, timelimit, target_f);
      return s2;
   }
   // optimizated version
   pair<Solution<R>&, Evaluation<M>&>& search(const Solution<R>& s, const Evaluation<M>& e, double timelimit = 100000000, double target_f = 0)
   {
      Solution<R>& s2 = s.clone();
      Evaluation<M>& e2 = e.clone();
      exec(s2, e2, timelimit, target_f);
      return *new pair<Solution<R>&, Evaluation<M>&> (s2, e2);
   }

   // no-optimization
   //Population& search(ConstPopulation& p, double timelimit = 100000000, double target_f = 0)
   Population<R>& search(Population<R> const & p, double timelimit = 100000000, double target_f = 0)
   {
      Population<R>* pop = new Population<R> ();

      for (unsigned i = 0; i < p.size(); i++)
         pop->push_back(p.at(i).clone());

      exec(*pop, timelimit, target_f);

      return *pop;
   }

   // optimizated version
   //pair<Population&, FitnessValues&>& search(ConstPopulation& p, ConstFitnessValues& ev,
   pair<Population<R>&, FitnessValues&>& search(Population<R> const & p, ConstFitnessValues& ev, double timelimit = 100000000, double target_f = 0)
   {
      //Population* p2 = new Population;
      Population<R>* p2 = new Population<R> ;

      for (unsigned i = 0; i < p.size(); i++)
         p2->push_back(&p[i]->clone());

      FitnessValues* ev2 = new FitnessValues;
      for (unsigned i = 0; i < p.size(); i++)
         ev2->push_back(&ev[i]->clone());

      exec(*p2, *ev2, timelimit, target_f);

      return *new pair<Population<R>&, FitnessValues&> (*p2, *ev2);
   }

   // core methods

   // 1
   virtual void exec(Solution<R>& s, double timelimit, double target_f)
   {
      // call 3
      //Population p;
      Population<R> p;

      //p.push_back(&s);
      p.push_back(s);

      exec(p, timelimit, target_f);
      // pop back? vector vai deletar o 's' sozinho?? testar! TODO
   }

   // 2
   virtual void exec(Solution<R>& s, Evaluation<M>& e, double timelimit, double target_f)
   {
      // call 4
      //Population p;
      Population<R> p;

      FitnessValues ev;

      //p.push_back(&s);
      p.push_back(s);

      ev.push_back(&e);

      exec(p, ev, timelimit, target_f);

      // pop back? TODO
      //s = v[0].first;
      //e = v[0].second;
   }

   // TODO: Nao consegui programar vector<Solution<R>& >& v (???)
   // TODO: E o que isso significa??? Usando ponteiro agora. em: vector<Solution<R>* >& v

   // 3
   //virtual void exec(Population& p, double timelimit, double target_f)
   virtual void exec(Population<R>& p, double timelimit, double target_f)
   {
      // call 1
      //Solution<R>& s = *(p[0]);
      Solution<R>& s = (p.at(0));

      exec(s, timelimit, target_f);
   }

   // 4
   //virtual void exec(Population& p, FitnessValues& ev, double timelimit, double target_f)
   virtual void exec(Population<R>& p, FitnessValues& ev, double timelimit, double target_f)
   {
      // call 2
      //Solution<R>& s = *(p[0]);
      Solution<R>& s = (p.at(0));

      Evaluation<M>& e = *(ev[0]);

      exec(s, e, timelimit, target_f);
   }

};

#endif /* OPTFRAME_HEURISTIC_HPP_ */
