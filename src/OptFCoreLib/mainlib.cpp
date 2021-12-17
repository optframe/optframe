

#include "optfxcore_lib.h"

#include <OptFrame/Util/Matrix.hpp>

#include <OptFCore/FConstructive.hpp>
#include <OptFCore/FEvaluator.hpp>

using namespace optframe;

// ===============

/*
class LSolTSP : public LSolution
{
public:
   std::vector<int> rep;
};
*/

using SolutionTSP = std::vector<int>;

// ===============

// TSP problem context and data reads
class ProblemContext
{
public:
   int n;               // number of clients
   Matrix<double> dist; // distance matrix (Euclidean)
   // load data from Scanner
   void load(Scanner& scanner)
   {
      n = *scanner.nextInt();      // reads number of clients
      dist = Matrix<double>(n, n); // initializes n x n matrix
      //
      vector<double> xvalues(n);
      vector<double> yvalues(n);
      //
      for (int i = 0; i < n; i++) {
         scanner.next();
         xvalues[i] = *scanner.nextDouble(); // reads x
         yvalues[i] = *scanner.nextDouble(); // reads y
      }
      // calculate distance values, for every client pair (i,j)
      for (int i = 0; i < n; i++)
         for (int j = 0; j < n; j++)
            dist(i, j) = distance(xvalues.at(i), yvalues.at(i), xvalues.at(j), yvalues.at(j));
   }
   // euclidean distance (double as return)
   double distance(double x1, double y1, double x2, double y2)
   {
      return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
   }
};
// Create TSP Problem Context
ProblemContext pTSP;

/*
class MyMoveSwap : public IMove
{
public:
   int i, j;

   MyMoveSwap(int _i, int _j)
     : i{ _i }
     , j{ _j }
   {
   }

   virtual IMove* lapply(LESolution& sol) override
   {
      sref<LSolTSP>& rep = (sref<LSolTSP>&)sol.first;

      return new MyMoveSwap(j, i);
   }
};
*/

class MyMoveSwap : public Move<LESolution>
{
public:
   int i, j;

   MyMoveSwap(int _i, int _j)
     : i{ _i }
     , j{ _j }
   {
   }

   bool canBeApplied(const LESolution& se) override
   {
      return (::abs(i - j) >= 2) && (i >= 1) && (j >= 1);
   }

   uptr<Move<LESolution>> applyUpdate(LESolution& se) override
   {
      //auto& se = (ESolutionTSP&) _se;
      // input cannot be outdated
      assert(!se.second.outdated);

      //auto& s = se.first;
      std::vector<int>& s = *std::any_cast<std::vector<int>>(&se.first); //((LSolTSP&)se.first).rep;

      int diff = -pTSP.dist(s[i - 1], s[i]) - pTSP.dist(s[i], s[(i + 1) % pTSP.n]) - pTSP.dist(s[j - 1], s[j]) - pTSP.dist(s[j], s[(j + 1) % pTSP.n]);
      diff += pTSP.dist(s[i - 1], s[j]) + pTSP.dist(s[j], s[(i + 1) % pTSP.n]) + pTSP.dist(s[j - 1], s[i]) + pTSP.dist(s[i], s[(j + 1) % pTSP.n]);
      // solution swap
      auto rev = this->apply(se);
      se.second.setObjFunction(se.second.evaluation() + diff);
      return rev;
   }

   virtual op<Evaluation<int>> cost(const LESolution& se, bool allowEstimated) override
   {
      //auto& se = (ESolutionTSP&) _se;
      assert(!se.second.outdated);
      //auto& s = se.first;
      const std::vector<int>& s = *std::any_cast<std::vector<int>>(&se.first); //((LSolTSP&)se.first).rep;
      //
      int diff = -pTSP.dist(s[i - 1], s[i]) - pTSP.dist(s[i], s[(i + 1) % pTSP.n]) - pTSP.dist(s[j - 1], s[j]) - pTSP.dist(s[j], s[(j + 1) % pTSP.n]);
      diff += pTSP.dist(s[i - 1], s[j]) + pTSP.dist(s[j], s[(i + 1) % pTSP.n]) + pTSP.dist(s[j - 1], s[i]) + pTSP.dist(s[i], s[(j + 1) % pTSP.n]);
      return std::make_optional(Evaluation<int>(diff));
   }

   uptr<Move<LESolution>> apply(LESolution& se) override
   {
      //auto& se = (ESolutionTSP&) _se;
      //LSolTSP& s = se.first;
      std::vector<int>& s = *std::any_cast<std::vector<int>>(&se.first);
      // perform swap of clients i and j
      /*
      int aux = se.first[j];
      se.first[j] = se.first[i];
      se.first[i] = aux;
      */
      int aux = s[j];
      s[j] = s[i];
      s[i] = aux;
      //
      return uptr<Move<LESolution>>(new MyMoveSwap{ j, i }); // return a reverse move ('undo' move)s
   }

   bool
   operator==(const Move<LESolution>& other) const override
   {
      auto& fmove = (MyMoveSwap&)other;
      return (i == fmove.i) && (j == fmove.j);
   }
};

Evaluation<int>
fevaluate(const std::any _s)
{
   const std::vector<int>& s = *std::any_cast<std::vector<int>>(&_s);
   int f = 0;
   for (int i = 0; i < int(pTSP.n) - 1; i++)
      f += pTSP.dist(s[i], s[i + 1]);
   f += pTSP.dist(s[int(pTSP.n) - 1], s[0]);
   return Evaluation<int>{ f };
}

//std::vector<int>
std::any
frandom()
{
   vector<int> v(pTSP.n, -1); // get information from context
   for (unsigned i = 0; i < v.size(); i++)
      v[i] = i;
   // leave 0 on first position and shuffle the rest
   std::random_shuffle(v.begin() + 1, v.end());
   return v;
}

int
main()
{
   int n = 10;

   std::function<uptr<Move<LESolution>>(const LESolution& se)> fRand =
     [&n](const LESolution& se) -> uptr<Move<LESolution>> {
      int i = rand() % n;
      int j = i;
      while (j <= i) {
         i = rand() % n;
         j = rand() % n;
      }
      return uptr<Move<LESolution>>(new MyMoveSwap(i, j));
   };

   std::function<Generator<Move<LESolution>*>(const LESolution& se)>
     fIterator = [&n](const LESolution& se) -> Generator<Move<LESolution>*> {
      for (int i = 0; i < int(n) - 1; i++)
         for (int j = i + 1; j < n; j++)
            co_yield new MyMoveSwap(i, j); // implicit unique_ptr requirements
   };

   sref<NSSeq<LESolution>> insseq = generateFxNSSeq(fRand, fIterator);
   insseq->print();

   // ------

   // Evaluate
   sref<Evaluator<typename LESolution::first_type, typename LESolution::second_type>> ev{
      new FEvaluator<LESolution, MinOrMax::MINIMIZE>{ fevaluate }
   };

   // Generate random solution
   FConstructive<typename LESolution::first_type> crand{
      frandom
   };

   // ============================
   // initialize standard builders
   //
   initializeStandardBuilders();

   // ----------------------------

   int nsseq_id = registerComponent(insseq, "OptFrame:NS");
   std::cout << "nsseq_id = " << nsseq_id << std::endl;

   vsref<Component> nslist;
   nslist.push_back(insseq);

   int nsseqlist_id = registerComponentList(nslist, "OptFrame:NS");
   std::cout << "nsseqlist_id = " << nsseqlist_id << std::endl;

   //Component& cev = ev;
   //std::cout << "cev => " << cev.id() << std::endl;
   int ev_id = registerComponent(ev, "OptFrame:GeneralEvaluator");
   std::cout << "ev_id = " << ev_id << std::endl;
   assert(ev_id >= 0);

   sref<InitialSearch<LESolution>> initRand{
      new BasicInitialSearch<LESolution>(crand, ev)
   };

   int rand_id = registerComponent(initRand, "OptFrame:InitialSearch");
   std::cout << "rand_id = " << rand_id << std::endl;
   assert(rand_id >= 0);

   // ----------------------------

   // list builders
   vector<pair<string, vector<pair<string, string>>>> lbs = listBuilders("OptFrame:");
   std::cout << "# builders: " << lbs.size() << std::endl;
   for (unsigned i = 0; i < lbs.size(); i++) {
      std::cout << "builder '" << lbs[i].first << "'" << std::endl;
      for (unsigned j = 0; j < lbs[i].second.size(); j++)
         std::cout << "\tparameter " << j << ": '" << lbs[i].second[j].first << "'" << std::endl;
   }

   // list components
   vector<string> lcmps = listComponents("OptFrame:");
   std::cout << "# components: " << lcmps.size() << std::endl;
   for (unsigned i = 0; i < lcmps.size(); i++)
      std::cout << "component '" << lcmps[i] << "'" << std::endl;

   // =============================

   std::cout << std::endl
             << "will build SA!" << std::endl;
   sptr<SingleObjSearch<LESolution>> uSearch = buildSearch("OptFrame:ComponentBuilder:SingleObjSearch:SA:BasicSA "
                                                           " OptFrame:GeneralEvaluator 0 "
                                                           " OptFrame:InitialSearch 0 "
                                                           " OptFrame:NS[] 0 "
                                                           " 999.99 100 0.98");
   if (!uSearch)
      std::cout << "-> nullptr" << std::endl;
   else
      uSearch->print();

   clearFactory();

   // =============

   return 0;
}