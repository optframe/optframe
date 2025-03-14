/*
 * OptimalLinearRegression.hpp
 *
 *  Created on: 26/10/2014
 *      Author: vitor
 */

#ifndef HFM_OPTIMALLINEARREGRESSION_HPP_
#define HFM_OPTIMALLINEARREGRESSION_HPP_

#include <ilcplex/ilocplex.h>
#include <ilcplex/ilocplexi.h>

#include "../../OptFrame/Search/LocalSearch.hpp"
#include "./Evaluator.hpp"

using namespace optframe;

ILOSTLBEGIN

namespace HFM {

class OptimalLinearRegression : public LocalSearch<RepHFM> {
 private:
  HFMEvaluator& eval;
  HFM_ProblemInstance& pEFP;

 public:
  OptimalLinearRegression(HFMEvaluator& _eval, HFM_ProblemInstance& _pEFP)
      : eval(_eval), pEFP(_pEFP) {}

  ~OptimalLinearRegression() {}

  void exec(Solution<RepHFM>& s, double timelimit, double target_f) {
    std::cout << "Exec Optimal Linear Regression" << std::endl;

    vector<vector<double> > methods;
    vector<double> realValues;

    realValues.push_back(0.9);
    realValues.push_back(1.5);
    realValues.push_back(2.1);
    realValues.push_back(2.2);

    vector<double> a;
    a.push_back(7.4);
    a.push_back(44);
    a.push_back(16.4);
    a.push_back(19);

    vector<double> b;
    b.push_back(2.2);
    b.push_back(33);
    b.push_back(0.2);
    b.push_back(80);

    vector<double> c;
    c.push_back(5.8);
    c.push_back(70);
    c.push_back(6.8);
    c.push_back(45);

    vector<double> d;
    d.push_back(1.1);
    d.push_back(10);
    d.push_back(9);
    d.push_back(138);
    methods.push_back(a);
    methods.push_back(b);
    methods.push_back(c);
    methods.push_back(d);

    int method = methods.size();
    int samples = realValues.size();

    IloEnv env;

    try {
      IloModel model(env);

      IloNumVarArray alpha(env);
      IloNumVarArray nErro(env);
      IloNumVarArray pErro(env);

      // IloObjective objFunc = IloMinimize(env);
      IloExpr objFunc(env);
      for (int s = 0; s < samples; s++) {
        std::stringstream ss;
        ss << "nErro" << s;
        string name = ss.str();
        nErro.add(IloNumVar(env, 0.0, IloInfinity, ILOFLOAT, name.c_str()));
        ss.str("");
        ss << "pErro" << s;
        name = ss.str();
        pErro.add(IloNumVar(env, 0.0, IloInfinity, ILOFLOAT, name.c_str()));
        // std::cout << nErro[s] << "\t" << pErro[s] << std::endl;
        objFunc += nErro[s] + pErro[s];
      }

      IloObjective obj = IloMinimize(env, objFunc);
      model.add(obj);

      for (int m = 0; m < method; m++) {
        std::stringstream ss;
        ss << "alpha" << m;
        string name = ss.str();
        alpha.add(
            IloNumVar(env, -IloInfinity, IloInfinity, ILOFLOAT, name.c_str()));
      }

      for (int s = 0; s < samples; s++) {
        IloExpr constraint(env);
        for (int m = 0; m < method; m++) {
          constraint += alpha[m] * methods[m][s];
        }
        constraint -= pErro[s];
        constraint += nErro[s];

        model.add(constraint == realValues[s]);
      }

      IloCplex cplex(model);

      //			cplex.exportModel("modelTeste.lp");
      //			cout << "model exported sucessfully!!!" << std::endl;
      //			getchar();
      IloRangeArray rng(env);
      // cplex.importModel(model, "", obj, var, rng);
      cplex.setParam(cplex.TiLim, 20);

      if (!cplex.solve()) {
        env.error() << "Failed to optimize LP" << std::endl;
        throw(-1);
      }

      IloNumArray vals(env);

      cplex.getValues(vals, alpha);
      env.out() << "Solution status = " << cplex.getStatus() << std::endl;
      env.out() << "Solution value  = " << cplex.getObjValue() << std::endl;
      env.out() << "Solution vector = " << vals << std::endl;

      getchar();

    } catch (IloException& e) {
      cerr << "Concert exception caught: " << e << std::endl;
    } catch (...) {
      cerr << "Unknown exception caught" << std::endl;
    }
  }

  // 2
  void exec(Solution<RepHFM>& s, Evaluation<>& e, double timelimit,
            double target_f) {
    exec(s, timelimit, target_f);
    Evaluation<>& e2 = eval.evaluate(s.getR());
    e = e2;
    delete &e2;
  }
};

}  // namespace HFM

#endif /* HFM_OPTIMALLINEARREGRESSION_HPP_ */
