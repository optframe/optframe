#ifndef ETII_PROBLEMMODULE_HPP_
#define ETII_PROBLEMMODULE_HPP_

#include <iostream>

#include <OptFrame/Scanner++/Scanner.hpp>

#include "Evaluation.h"
#include "Representation.h"
#include "Solution.h"

#include "ProblemInstance.h"

#include "Evaluator.h"

#include "DeltaMoveRotate.h"
#include "NSSeqRotate.h"

#include "DeltaMoveSwapCenter.h"
#include "NSSeqSwapCenter.h"

#include "DeltaMoveSwapCorner.h"
#include "NSSeqSwapCorner.h"

#include "NSSeqSwapRotateCenter.h"

#include "DeltaMoveSwapSide.h"
#include "NSSeqSwapSide.h"

#include "InitialSolutionGreedy.h"

#include <OptFrame/HeuristicFactory.hpp>

using namespace scannerpp;
using namespace optframe;

namespace EtII {

class EtIIProblemCommand
{
public:
   ProblemInstance* p;

   EtIIProblemCommand()
   {
      p = NULL;
   }

   virtual ~EtIIProblemCommand()
   {
      if (p)
         delete p;
   }

   string id()
   {
      return "problem.EtII";
   }

   bool registerComponent(sref<Component> component, string type, string name, HeuristicFactory<SolutionEtII>& hf, map<string, string>& dictionary)
   {
      int idx = hf.addComponent(component, type);
      stringstream ss;
      ss << type << " " << idx;
      return true; //defineText(name, ss.str(), dictionary);
   }

   bool load(string filename, HeuristicFactory<SolutionEtII>& hf, map<string, string>& dictionary, map<string, vector<string>>& ldictionary)
   {
      File file(filename);

      if (!file.isOpen()) {
         cout << "File '" << filename << "' not found" << endl;
         return false;
      }

      Scanner scanner(std::move(file));

      p = new ProblemInstance(scanner);

      // add everything to the HeuristicFactory 'hf'

      //EtIIInitialSolutionGreedy& is = *new EtIIInitialSolutionGreedy(*p, hf.getRandGen());
      sref<Constructive<SolutionEtII>> is(new EtIIInitialSolutionGreedy(*p, hf.getRandGen()));

      SolutionEtII s = *is->generateSolution(10); // TODO: fix time

      sref<NSSeqRotate<DeltaMoveRotate>> nsRotate{ new NSSeqRotate<DeltaMoveRotate>(hf.getRandGen()) };
      sref<NSSeqSwapCenter<DeltaMoveSwapCenter>> nsSwapCenter = new NSSeqSwapCenter<DeltaMoveSwapCenter>(hf.getRandGen());
      sref<NSSeqSwapCorner<DeltaMoveSwapCorner>> nsSwapCorner = new NSSeqSwapCorner<DeltaMoveSwapCorner>(hf.getRandGen());
      sref<NSSeqSwapRotateCenter<>> nsSwapRotateCenter = new NSSeqSwapRotateCenter<>(hf.getRandGen());
      sref<NSSeqSwapSide<DeltaMoveSwapSide>> nsSwapSide = new NSSeqSwapSide<DeltaMoveSwapSide>(hf.getRandGen());

      s.print();

      //EtIIEvaluator& eval = *new EtIIEvaluator(*p);
      //sref<GeneralEvaluator<ESolutionEtII>> eval(new EtIIEvaluator(*p));
      sref<Component> eval(new EtIIEvaluator(*p));

      Evaluation<> e = ((Evaluator<SolutionEtII, EvaluationEtII, ESolutionEtII>&)*eval).evaluate(s);

      e.print();
      cout << endl;

      hf.addComponent(is, "OptFrame:Constructive");
      hf.addComponent(eval, "OptFrame:GeneralEvaluator");
      hf.addComponent(nsRotate, "OptFrame:NS:NSFind:NSSeq");
      hf.addComponent(nsSwapCenter, "OptFrame:NS:NSFind:NSSeq");
      hf.addComponent(nsSwapCorner, "OptFrame:NS:NSFind:NSSeq");
      hf.addComponent(nsSwapRotateCenter, "OptFrame:NS:NSFind:NSSeq");
      hf.addComponent(nsSwapSide, "OptFrame:NS:NSFind:NSSeq");

      cout << "problem '" << filename << "' loaded successfully" << endl;

      return true;
   }

   bool unload(HeuristicFactory<SolutionEtII>& factory, map<string, string>& dictionary, map<string, vector<string>>& ldictionary)
   {
      if (p)
         delete p;
      p = NULL;

      return true;
   }
};

}

#endif /*ETII_PROBLEMMODULE_HPP_*/
