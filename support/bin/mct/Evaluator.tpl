#pragma once

#ifndef $project_EVALUATOR_H_
#define $project_EVALUATOR_H_

//#include <cmath>
#include <stdlib.h>
#include <math.h>

#include <OptFrame/Core/Evaluation.hpp>
#include <OptFrame/Core/Evaluator.hpp>

#include "Representation.h"
#include "ProblemInstance.h"


namespace $project
{

// If you need ADS, you can get it from Solution$project (if subsumed by XBaseSolution<R, ADS>)
class MyEvaluator: public Evaluator<Solution$project>
{
private:
	ProblemInstance& p$project;

	// Your private vars

public:
	MyEvaluator(ProblemInstance& _p$project);
	
	virtual ~MyEvaluator();

    // you can replace OptFrameADS with your prefered ADS from inside S (if subsumed by XBaseSolution<R, ADS>)
	Evaluation<> evaluate(const Solution$project& s) override;
	
	virtual bool isMinimization() const override;
};


static_assert(std::is_base_of<Evaluator< Solution$project >, MyEvaluator >::value,  "not inherited from Evaluator");
static_assert(std::is_base_of<GeneralEvaluator< ESolution$project >, MyEvaluator >::value,  "not inherited from GeneralEvaluator");

} // namespace $project 

#endif /*$project_EVALUATOR_H_*/
